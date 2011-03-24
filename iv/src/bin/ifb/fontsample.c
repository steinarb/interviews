/*
 * Copyright (c) 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * display a sample of a named Font
 */

#include "fontsample.h"

#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/pattern.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>
#include <InterViews/textbuffer.h>
#include <InterViews/world.h>
#include <ctype.h>
#include <string.h>

static const int BufferSize = 1000;
static const int SampleLines = 2;
static const int TabWidth = 8;

FontSample::FontSample(
    const char* sample
) : TextEditor(SampleLines, strlen(sample), TabWidth, Reversed) {
    Init(sample);
}

FontSample::FontSample(
    const char* name, const char* sample
) : TextEditor(SampleLines, strlen(sample), TabWidth, Reversed) {
    SetInstance(name);
    Init(sample);
}

FontSample::~FontSample () {
    delete text;
    delete sample;
    delete fontname;
    Unref(font);
}

void FontSample::Init (const char* samp) {
    SetClassName("FontSample");
    font = nil;
    fontname = new char [BufferSize];
    sample = new char [BufferSize];
    text = new TextBuffer(sample, 0, BufferSize);
    text->Insert(0, samp, strlen(samp));
    Edit(text);
    input = new Sensor;
    input->Catch(KeyEvent);
    input->Catch(DownEvent);
}

void FontSample::Reconfig () {
    Painter* p = new Painter(output);
    p->Reference();
    Unref(output);
    output = p;
    output->SetFont(font);
    TextEditor::Reconfig();
    shape->vshrink = shape->height - shape->vunits;
}

void FontSample::ShowFont (const char* name) {
    if (strcmp(name, fontname) != 0) {
        strcpy(fontname, name);
	if (Font::exists(World::current()->display(), fontname)) {
	    font = new Font(fontname);
	    font->ref();
	} else {
	    font = nil;
	}
        if (output != nil) {
            if (font != nil) {
                output->FillBg(true);
                output->SetPattern(new Pattern);
                Reconfig();
                Resize();
            } else {
                Select(Dot());
                output->ClearRect(canvas, 0, 0, xmax, ymax);
                output->FillBg(false);
                output->SetPattern(new Pattern(Pattern::gray));
            }
            Draw();
        }
    }
}

void FontSample::Handle (Event& e) {
    if (font == nil) {
        return;
    }
    switch (e.eventType) {
    case KeyEvent:
        if (e.len > 0) {
            char c = e.keystring[0];
            switch(c) {
            case '\006':
                ForwardCharacter(1);
                break;
            case '\002':
                BackwardCharacter(1);
                break;
            case '\016':
                ForwardLine(1);
                break;
            case '\020':
                BackwardLine(1);
                break;
            case '\001':
                BeginningOfLine();
                break;
            case '\005':
                EndOfLine();
                break;
            case '\004':
            case '\010':
            case '\177':
                if (Dot() != Mark()) {
                    DeleteSelection();
                } else {
                    DeleteText(c=='\004' ? 1 : -1);
                }
                break;
            default:
                if (c == '\015' || !iscntrl(c)) {
                    if (Dot() != Mark()) {
                        DeleteSelection();
                    }
                    InsertText(c=='\015' ? "\n" : &c, 1);
                }
            }
        }
        ScrollToSelection();
        break;
    case DownEvent:
        Select(Locate(e.x, e.y));
        do {
            ScrollToView(e.x, e.y);
            SelectMore(Locate(e.x, e.y));
            Poll(e);
        } while (e.leftmouse || e.middlemouse || e.rightmouse);
        break;
    }
}

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
 * a RadioButton with an editable value
 */

#include "stringbutton.h"

#include <InterViews/event.h>
#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/shape.h>
#include <InterViews/textdisplay.h>
#include <ctype.h>
#include <string.h>

StringButton::StringButton (
    ButtonState* s, const char* sample
) : RadioButton(sample, s, nil) {
    Init();
}

StringButton::StringButton (
    const char* name, ButtonState* s, const char* sample
) : RadioButton(sample, s, nil) {
    SetInstance(name);
    Init();
}

StringButton::~StringButton () {
    delete text;
    delete display;
    text = nil;
}

void StringButton::Init () {
    text = new char[1000];
    text[0] = '\0';
    value = (void*)text;
    display = new TextDisplay();        
    display->ReplaceText(0, text, strlen(text));
    display->CaretStyle(NoCaret);
    offset = 0;
}

void StringButton::Reconfig () {
    RadioButton::Reconfig();
    shape->width += 4;
    shape->height += 4;
}

void StringButton::Resize () {
    int h = output->GetFont()->Height();
    offset = h;
    display->LineHeight(h);
    display->Resize(offset+2, 2, xmax-2, ymax-2);
}

void StringButton::Handle (Event& e) {
    if (e.eventType == DownEvent && e.x > ymax) {
        hit = true;
        Refresh();
        boolean done = false;
        int p = strlen(text);
        display->Draw(output, canvas);
        display->CaretStyle(BarCaret);
        do {
            switch (e.eventType) {
            case KeyEvent:
                char c = e.keystring[0];
                if (e.len > 0) {
                    if (c == '\015') {
                        done = true;
                    } else if (c == '\177') {
                        if (p > 0) {
                            --p;
                            text[p] = '\0';
                            display->DeleteText(0, p, 1);
                        }
                    } else if (!iscntrl(c)) {
                        text[p] = c;
                        display->InsertText(0, p, &c, 1);
                        ++p;
                        text[p] = '\0';
                    }
                }
                break;
            case DownEvent:
                if (e.target != this) {
                    UnRead(e);
                    done = true;
                }
                break;
            }
            display->Caret(0, p);
            if (!done) {
                Read(e);
            }
        } while (!done);
        display->CaretStyle(NoCaret);
        hit = false;
        Refresh();
        subject->SetValue(value);
        subject->Notify();
    } else {
        RadioButton::Handle(e);
    }
}

void StringButton::Redraw (Coord l, Coord b, Coord r, Coord t) {
    display->Draw(output, canvas);
    display->Redraw(l, b, r, t);
    output->Rect(canvas, offset, 0, xmax, ymax);
    Refresh();
}

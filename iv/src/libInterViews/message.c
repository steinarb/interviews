/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
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
 * Message class implementation.
 */

#include <InterViews/message.h>
#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/shape.h>
#include <stdlib.h>

Message::Message(const char* msg, Alignment al, int pad, int hstr, int vstr) {
    Init(msg, al, pad, hstr, vstr);
}

Message::Message(
    const char* name,
    const char* msg, Alignment al, int pad, int hstr, int vstr
) : (name) {
    Init(msg, al, pad, hstr, vstr);
}

void Message::Init (const char* t, Alignment a, int p, int hstr, int vstr) {
    SetClassName("Message");
    text = t;
    alignment = a;
    pad = p;
    shape->hstretch = hstr;
    shape->vstretch = vstr;
    highlighted = false;
}

void Message::Reconfig () {
    const char* a = GetAttribute("text");
    if (a != nil) {
	text = a;
    }
    a = GetAttribute("padding");
    if (a != nil) {
	pad = atoi(a);
    }
    Font* f = output->GetFont();
    shape->width = pad + f->Width(text) + pad;
    shape->height = pad + f->Height() + pad;
    shape->hshrink = pad + pad;
    shape->vshrink = pad + pad;
}

void Message::Realign (Alignment a) {
    alignment = a;
    Draw();
}

void Message::Redraw (Coord l, Coord b, Coord r, Coord t) {
    Coord x = 0, y = 0;
    Align(alignment, shape->width, shape->height, x, y);
    output->Clip(canvas, l, b, r, t);
    if (highlighted) {
	output->SetColors(output->GetBgColor(), output->GetFgColor());
    }
    output->ClearRect(canvas, l, b, r, t);
    output->Text(canvas, text, x + pad, y + pad);
    if (highlighted) {
	output->SetColors(output->GetBgColor(), output->GetFgColor());
    }
    output->NoClip();
}

void Message::Highlight (boolean b) {
    if (highlighted != b) {
	highlighted = b;
	Draw();
    }
}

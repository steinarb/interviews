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
 * Banner implementation.
 */

#include <InterViews/banner.h>
#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/shape.h>

static const int pad = 2*pixels;	/* space around banner text */

Banner::Banner (char* lt, char* m, char* rt) {
    Init(lt, m, rt);
}

Banner::Banner (const char* name, char* lt, char* m, char* rt) {
    SetInstance(name);
    Init(lt, m, rt);
}

Banner::Banner (Painter* out, char* lt, char* m, char* rt) : (nil, out) {
    Init(lt, m, rt);
    Reconfig();
}

void Banner::Init (char* lt, char* m, char* rt) {
    SetClassName("Banner");
    left = lt;
    middle = m;
    right = rt;
    highlight = false;
    inverse = nil;
}

void Banner::Reconfig () {
    int w;

    Font* f = output->GetFont();
    lw = left == nil ? 0 : f->Width(left);
    mw = middle == nil ? 0 : f->Width(middle);
    rw = right == nil ? 0 : f->Width(right);
    if (mw > 0) {
	w = mw + 2*max(lw, rw);
    } else {
	w = lw + rw;
    }
    shape->width = 2*pad + w + f->Width("    ");
    shape->height = f->Height() + 2*pad;
    shape->Rigid(0, hfil, 0, 0);
    Unref(inverse);
    inverse = new Painter(output);
    inverse->Reference();
    inverse->SetColors(output->GetBgColor(), output->GetFgColor());
}

Banner::~Banner () {
    Unref(inverse);
}

void Banner::Redraw (Coord x1, Coord y1, Coord x2, Coord y2) {
    Painter* p = highlight ? inverse : output;
    p->ClearRect(canvas, x1, y1, x2, y2);
    if (right != nil && rx <= x2) {
	p->MoveTo(rx, pad);
	p->Text(canvas, right);
    }
    if (middle != nil && mx + mw >= x1 && mx <= x2) {
	p->MoveTo(mx, pad);
	p->Text(canvas, middle);
    }
    if (left != nil && lx + lw >= x1) {
	p->MoveTo(lx, pad);
	p->Text(canvas, left);
    }
}

void Banner::Resize () {
    lx = pad;
    mx = (xmax - mw) / 2;
    rx = xmax - rw + 1 - pad;
}

void Banner::Update () {
    if (canvas != nil) {
	Reconfig();
	Resize();
	Draw();
    }
}

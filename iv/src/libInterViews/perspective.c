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
 * Perspective definition.
 */

#include <InterViews/perspective.h>
#include <InterViews/interactor.h>

class ViewList {
public:
    Interactor* view;
    ViewList* next;

    ViewList (Interactor* i) { view = i; next = nil; }
};

Perspective::Perspective () {
    views = nil;
    x0 = 0; y0 = 0;
    width = 0; height = 0;
    curx = 0; cury = 0;
    curwidth = 0; curheight = 0;
    sx = 0; sy = 0;
    lx = 0; ly = 0;
    Reference();
}

Perspective::Perspective (Perspective& p) {
    views = nil;
    x0 = p.x0;
    y0 = p.y0;
    width = p.width;
    height = p.height;
    curx = p.curx;
    cury = p.cury;
    curwidth = p.curwidth;
    curheight = p.curheight;
    sx = p.sx;
    sy = p.sy;
    lx = p.lx;
    ly = p.ly;
    Reference();
}

Perspective::~Perspective () {
    register ViewList* e;
    register ViewList* next;

    for (e = views; e != nil; e = next) {
	next = e->next;
	delete e;
    }
}

void Perspective::Init (Coord ix0, Coord iy0, Coord iwidth, Coord iheight) {
    x0 = ix0; y0 = iy0;
    width = iwidth; height = iheight;
    curx = x0; cury = y0;
}

void Perspective::Attach (Interactor* i) {
    register ViewList* e;

    e = new ViewList(i);
    e->next = views;
    views = e;
    Reference();
}

void Perspective::Detach (Interactor* i) {
    register ViewList* e;
    register ViewList* prev;

    prev = nil;
    for (e = views; e != nil; e = e->next) {
	if (e->view == i) {
	    if (prev == nil) {
		views = e->next;
	    } else {
		prev->next = e->next;
	    }
	    e->view = nil;
	    e->next = nil;
	    delete e;
	    Unreference();
	    break;
	}
	prev = e;
    }
}

void Perspective::Update () {
    register ViewList* e;

    for (e = views; e != nil; e = e->next) {
	e->view->Update();
    }
}

boolean Perspective::operator == (Perspective& p) {
    return
	x0 == p.x0 && y0 == p.y0 &&
	width == p.width && height == p.height &&
	curx == p.curx && cury == p.cury &&
	curwidth == p.curwidth && curheight == p.curheight &&
	sx == p.sx && sy == p.sy &&
	lx == p.lx && ly == p.ly;
}

boolean Perspective::operator != (Perspective& p) {
    return
	x0 != p.x0 || y0 != p.y0 ||
	width != p.width || height != p.height ||
	curx != p.curx || cury != p.cury ||
	curwidth != p.curwidth || curheight != p.curheight ||
	sx != p.sx || sy != p.sy ||
	lx != p.lx || ly != p.ly;
}

Perspective& Perspective::operator = (Perspective& p) {
    x0 = p.x0;
    y0 = p.y0;
    width = p.width;
    height = p.height;
    curx = p.curx;
    cury = p.cury;
    curwidth = p.curwidth;
    curheight = p.curheight;
    sx = p.sx;
    sy = p.sy;
    lx = p.lx;
    ly = p.ly;
    return *this;
}

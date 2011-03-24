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

// $Header: ipolygons.c,v 1.7 89/10/09 14:48:22 linton Exp $
// implements classes IFillRect and IFillPolygon.

#include "ipaint.h"
#include "ipolygons.h"

// IFillRect passes its arguments to FillRect.

IFillRect::IFillRect (Coord x0, Coord y0, Coord x1, Coord y1, Graphic* gs)
: (x0, y0, x1, y1, gs) {
}

// contains returns true if the IFillRect contains the given point
// unless the pattern is the "none" pattern.

boolean IFillRect::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	contains = FillRect::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillRect intersects the given box
// unless the pattern is the "none" pattern.

boolean IFillRect::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	intersects = FillRect::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillRect unless the pattern is the "none" pattern.

void IFillRect::draw (Canvas* c, Graphic* gs) {
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	FillRect::draw(c, gs);
    }
}

// IFillPolygon passes its arguments to FillPolygon.

IFillPolygon::IFillPolygon (Coord* x, Coord* y, int n, Graphic* gs)
: (x, y, n, gs) {
}

// contains returns true if the IFillPolygon contains the given point
// unless the pattern is the "none" pattern.

boolean IFillPolygon::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	contains = FillPolygon::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillPolygon intersects the given
// box unless the pattern is the "none" pattern.

boolean IFillPolygon::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	intersects = FillPolygon::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillPolygon unless the pattern is the "none"
// pattern.

void IFillPolygon::draw (Canvas* c, Graphic* gs) {
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	FillPolygon::draw(c, gs);
    }
}

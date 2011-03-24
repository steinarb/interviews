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

// $Header: iellipses.c,v 1.7 89/10/09 14:48:11 linton Exp $
// implements classes IFillEllipse and IFillCircle.

#include "iellipses.h"
#include "ipaint.h"

// IFillEllipse passes its arguments to FillEllipse.

IFillEllipse::IFillEllipse (Coord x0, Coord y0, int rx, int ry, Graphic* gs)
: (x0, y0, rx, ry, gs) {
}

// contains returns true if the IFillEllipse contains the given point
// unless the pattern is the "none" pattern.

boolean IFillEllipse::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	contains = FillEllipse::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillEllipse intersects the given
// box unless the pattern is the "none" pattern.

boolean IFillEllipse::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	intersects = FillEllipse::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillEllipse unless the pattern is the "none" pattern.

void IFillEllipse::draw (Canvas* c, Graphic* gs) {
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	FillEllipse::draw(c, gs);
    }
}

// IFillCircle passes its arguments to FillCircle.

IFillCircle::IFillCircle (Coord x0, Coord y0, int r, Graphic* gs)
: (x0, y0, r, gs) {
}

// contains returns true if the IFillCircle contains the given point
// unless the pattern is the "none" pattern.

boolean IFillCircle::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	contains = FillCircle::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillCircle intersects the given box
// unless the pattern is the "none" pattern.

boolean IFillCircle::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	intersects = FillCircle::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillCircle unless the pattern is the "none"
// pattern.

void IFillCircle::draw (Canvas* c, Graphic* gs) {
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	FillCircle::draw(c, gs);
    }
}

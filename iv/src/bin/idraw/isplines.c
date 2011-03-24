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

// $Header: isplines.c,v 1.8 89/10/09 14:48:26 linton Exp $
// implements classes IFillBSpline and IFillClosedBSpline.

#include "ipaint.h"
#include "isplines.h"
#include <InterViews/Graphic/util.h>

// IFillBSpline repeats the first and last points three times to
// ensure that the spline will pass through the first and last points.

IFillBSpline::IFillBSpline (Coord* ax, Coord* ay, int n, Graphic* gs)
: (ax, ay, n, gs) {
    delete x;
    delete y;
    count = n + 4;
    x = new Coord[count];
    y = new Coord[count];
    x[0] = ax[0];
    x[1] = ax[0];
    x[count - 1] = ax[n - 1];
    x[count - 2] = ax[n - 1];
    y[0] = ay[0];
    y[1] = ay[0];
    y[count - 1] = ay[n - 1];
    y[count - 2] = ay[n - 1];
    CopyArray(ax, ay, n, &x[2], &y[2]);
}

// contains returns true if the IFillBSpline contains the given point
// unless the brush is an arrow or the pattern is the "none" pattern.

boolean IFillBSpline::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IBrush* brush = (IBrush*) gs->GetBrush();
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!brush->LeftArrow() && !brush->RightArrow() && !pattern->None()) {
	contains = FillBSpline::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillBSpline intersects the given
// box unless the brush is an arrow or the pattern is the "none"
// pattern.

boolean IFillBSpline::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IBrush* brush = (IBrush*) gs->GetBrush();
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!brush->LeftArrow() && !brush->RightArrow() && !pattern->None()) {
	intersects = FillBSpline::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillBSpline unless the brush is an arrow or the
// pattern is the "none" pattern.

void IFillBSpline::draw (Canvas* c, Graphic* gs) {
    IBrush* brush = (IBrush*) gs->GetBrush();
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!brush->LeftArrow() && !brush->RightArrow() && !pattern->None()) {
	FillBSpline::draw(c, gs);
    }
}

// IFillClosedBSpline passes its arguments to FillBSpline.

IFillClosedBSpline::IFillClosedBSpline (Coord* x, Coord* y, int n, Graphic* gs)
: (x, y, n, gs) {
}

// contains returns true if the IFillClosedBSpline contains the given
// point unless the pattern is the "none" pattern.

boolean IFillClosedBSpline::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	contains = FillBSpline::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillClosedBSpline intersects the
// given box unless the pattern is the "none" pattern.

boolean IFillClosedBSpline::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	intersects = FillBSpline::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillClosedBSpline unless the pattern is the "none"
// pattern.

void IFillClosedBSpline::draw (Canvas* c, Graphic* gs) {
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!pattern->None()) {
	FillBSpline::draw(c, gs);
    }
}

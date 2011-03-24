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

// $Header: ilines.c,v 1.7 89/10/09 14:48:15 linton Exp $
// implements class IFillMultiLine.

#include "ilines.h"
#include "ipaint.h"

// IFillMultiLine passes its arguments to FillPolygon.

IFillMultiLine::IFillMultiLine (Coord* x, Coord* y, int n, Graphic* gs)
: (x, y, n, gs) {
}

// contains returns true if the IFillMultiLine contains the given point
// unless the brush is an arrow or the pattern is the "none" pattern.

boolean IFillMultiLine::contains (PointObj& po, Graphic* gs) {
    boolean contains = false;
    IBrush* brush = (IBrush*) gs->GetBrush();
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!brush->LeftArrow() && !brush->RightArrow() && !pattern->None()) {
	contains = FillPolygon::contains(po, gs);
    }
    return contains;
}

// intersects returns true if the IFillMultiLine intersects the given
// box unless the brush is an arrow or the pattern is the "none"
// pattern.

boolean IFillMultiLine::intersects (BoxObj& userb, Graphic* gs) {
    boolean intersects = false;
    IBrush* brush = (IBrush*) gs->GetBrush();
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!brush->LeftArrow() && !brush->RightArrow() && !pattern->None()) {
	intersects = FillPolygon::intersects(userb, gs);
    }
    return intersects;
}

// draw draws the IFillMultiLine unless the brush is an arrow or the
// pattern is the "none" pattern (a IFillPolygon wouldn't have cared
// about the brush being an arrow).

void IFillMultiLine::draw (Canvas* c, Graphic* gs) {
    IBrush* brush = (IBrush*) gs->GetBrush();
    IPattern* pattern = (IPattern*) gs->GetPattern();
    if (!brush->LeftArrow() && !brush->RightArrow() && !pattern->None()) {
	FillPolygon::draw(c, gs);
    }
}

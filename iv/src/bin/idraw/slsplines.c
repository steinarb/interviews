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

// $Header: slsplines.c,v 1.13 89/10/09 14:49:42 linton Exp $
// implements classes BSplineSelection and ClosedBSplineSelection.

#include "isplines.h"
#include "slsplines.h"
#include <InterViews/rubcurve.h>
#include <stream.h>

// BSplineSelection creates its components.

BSplineSelection::BSplineSelection (Coord* x, Coord* y, int n, Graphic* gs)
: (gs) {
    Init(x, y, n);
}

// BSplineSelection reads data to initialize its graphic state and
// create its components.

BSplineSelection::BSplineSelection (istream& from, State* state) : (nil) {
    bspline = nil;
    ReadGS(from, state);
    Coord* x;
    Coord* y;
    int n;
    ReadPoints(from, x, y, n);
    Init(x, y, n);
}

// Copy returns a copy of the BSplineSelection.

Graphic* BSplineSelection::Copy () {
    Coord* x;
    Coord* y;
    int n = GetOriginal(x, y);
    Graphic* copy = new BSplineSelection(x, y, n, this);
    return copy;
}

// GetOriginal returns the control points that were passed to the
// BSplineSelection's constructor.

int BSplineSelection::GetOriginal (const Coord*& x, const Coord*& y) {
    return ((BSpline*) bspline)->GetOriginal(x, y);
}

// Init creates the graphic's components and stores the arrowheads'
// endpoints.

void BSplineSelection::Init (Coord* x, Coord* y, int n) {
    myname = "BSpl";
    ifillbspline = new IFillBSpline(x, y, n);
    bspline = new BSpline(x, y, n);
    lx0 = x[0];
    ly0 = y[0];
    lx1 = x[1];
    ly1 = y[1];
    rx0 = x[n-1];
    ry0 = y[n-1];
    rx1 = x[n-2];
    ry1 = y[n-2];
}

// CreateRubberVertex creates and returns the right kind of
// RubberVertex to represent the BSplineSelection's shape.

RubberVertex* BSplineSelection::CreateRubberVertex (Coord* x, Coord* y,
int n, int rubpt) {
    return new RubberSpline(nil, nil, x, y, n, rubpt);
}

// CreateReshapedCopy creates and returns a reshaped copy of itself
// using the passed points and its graphic state.

Selection* BSplineSelection::CreateReshapedCopy (Coord* x, Coord* y, int n) {
    return new BSplineSelection(x, y, n, this);
}

// uncacheChildren uncaches the graphic's components' extents.

void BSplineSelection::uncacheChildren () {
    if (bspline != nil) {
	uncacheExtentGraphic(ifillbspline);
	uncacheExtentGraphic(bspline);
    }
}

// getExtent returns the graphic's extent including a tolerance for
// the arrowheads.

void BSplineSelection::getExtent (float& l, float& b, float& cx, float& cy,
float& tol, Graphic* gs) {
    Extent e;
    if (extentCached()) {
	getCachedExtent(e.left, e.bottom, e.cx, e.cy, e.tol);
    } else {
	FullGraphic gstmp;
	concatGSGraphic(ifillbspline, this, gs, &gstmp);
	getExtentGraphic(
            ifillbspline, e.left, e.bottom, e.cx, e.cy, e.tol, &gstmp
        );
	Extent te;
	concatGSGraphic(bspline, this, gs, &gstmp);
	getExtentGraphic(
            bspline, te.left, te.bottom, te.cx, te.cy, te.tol, &gstmp
        );
	e.Merge(te);
	cacheExtent(e.left, e.bottom, e.cx, e.cy, e.tol);
    }
    float right = 2*e.cx - e.left;
    float top = 2*e.cy - e.bottom;
    float dummy1, dummy2;
    transformRect(e.left, e.bottom, right, top, l, b, dummy1, dummy2, gs);
    transform(e.cx, e.cy, cx, cy, gs);
    tol = MergeArrowHeadTol(e.tol, gs);
}

// contains returns true if the graphic contains the point.

boolean BSplineSelection::contains (PointObj& po, Graphic* gs) {
    BoxObj b;
    getBox(b, gs);
    if (b.Contains(po)) {
	if (containsGraphic(ifillbspline, po, gs)) {
	    return true;
	} else if (containsGraphic(bspline, po, gs)) {
	    return true;
	} else if (LeftAcont(lx0, ly0, lx1, ly1, po, gs)) {
	    return true;
	} else if (RightAcont(rx0, ry0, rx1, ry1, po, gs)) {
	    return true;
	}
    }
    return false;
}

// intersects returns true if the graphic intersects the box.

boolean BSplineSelection::intersects (BoxObj& userb, Graphic* gs) {
    BoxObj b;
    getBox(b, gs);
    if (b.Intersects(userb)) {
	if (intersectsGraphic(ifillbspline, userb, gs)) {
	    return true;
	} else if (intersectsGraphic(bspline, userb, gs)) {
	    return true;
	} else if (LeftAints(lx0, ly0, lx1, ly1, userb, gs)) {
	    return true;
	} else if (RightAints(rx0, ry0, rx1, ry1, userb, gs)) {
	    return true;
	}
    }
    return false;
}

// draw draws the graphic.

void BSplineSelection::draw (Canvas* c, Graphic* gs) {
    drawGraphic(ifillbspline, c, gs);
    drawGraphic(bspline, c, gs);
    drawLeftA(lx0, ly0, lx1, ly1, c, gs);
    drawRightA(rx0, ry0, rx1, ry1, c, gs);
}

// drawClipped draws the graphic if it intersects the clipping box.

void BSplineSelection::drawClipped (Canvas* c, Coord l, Coord b, Coord r,
Coord t, Graphic* gs) {
    BoxObj box;
    getBox(box, gs);

    BoxObj clipBox(l, b, r, t);
    if (clipBox.Intersects(box)) {
	draw(c, gs);
    }
}

// ClosedBSplineSelection creates the closed B-spline's filled
// interior and outline.

ClosedBSplineSelection::ClosedBSplineSelection (Coord* x, Coord* y, int n,
Graphic* gs) : (gs) {
    myname = "CBSpl";
    Append(new IFillClosedBSpline(x, y, n));
    Append(new ClosedBSpline(x, y, n));
}

// ClosedBSplineSelection reads data to initialize its graphic state
// and create the closed B-spline's filled interior and outline.

ClosedBSplineSelection::ClosedBSplineSelection (istream& from, State* state)
: (nil) {
    myname = "CBSpl";
    ReadGS(from, state);
    Coord* x;
    Coord* y;
    int n;
    ReadPoints(from, x, y, n);
    Append(new IFillClosedBSpline(x, y, n));
    Append(new ClosedBSpline(x, y, n));
}

// Copy returns a copy of the ClosedBSplineSelection.

Graphic* ClosedBSplineSelection::Copy () {
    Coord* x;
    Coord* y;
    int n = GetOriginal(x, y);
    Graphic* copy = new ClosedBSplineSelection(x, y, n, this);
    return copy;
}

// GetOriginal returns the control points that were passed to the
// ClosedBSplineSelection's constructor.

int ClosedBSplineSelection::GetOriginal (const Coord*& x, const Coord*& y) {
    return ((ClosedBSpline*) Last())->GetOriginal(x, y);
}

// CreateRubberVertex creates and returns the right kind of
// RubberVertex to represent the ClosedBSplineSelection's shape.

RubberVertex* ClosedBSplineSelection::CreateRubberVertex (Coord* x, Coord* y,
int n, int rubpt) {
    return new RubberClosedSpline(nil, nil, x, y, n, rubpt);
}

// CreateReshapedCopy creates and returns a reshaped copy of itself
// using the passed points and its graphic state.

Selection* ClosedBSplineSelection::CreateReshapedCopy (Coord* x, Coord* y,
int n) {
    return new ClosedBSplineSelection(x, y, n, this);
}

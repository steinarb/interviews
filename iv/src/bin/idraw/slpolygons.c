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

// $Header: slpolygons.c,v 1.10 89/10/09 14:49:38 linton Exp $
// implements classes RectSelection and PolygonSelection.

#include "ipolygons.h"
#include "rubbands.h"
#include "slpolygons.h"
#include <stream.h>

// RectSelection creates the rectangle's filled interior and outline.

RectSelection::RectSelection (Coord l, Coord b, Coord r, Coord t, Graphic* gs)
: (gs) {
    myname = "Rect";
    Append(new IFillRect(l, b, r, t));
    Append(new Rect(l, b, r, t));
}

// RectSelection reads data to initialize its graphic state and create
// its filled interior and outline.

RectSelection::RectSelection (istream& from, State* state) : (nil) {
    myname = "Rect";
    ReadGS(from, state);
    Skip(from);
    Coord l, b, r, t;
    from >> l >> b >> r >> t;
    Append(new IFillRect(l, b, r, t));
    Append(new Rect(l, b, r, t));
}

// Copy returns a copy of the RectSelection.

Graphic* RectSelection::Copy () {
    Coord l, b, r, t;
    GetOriginal2(l, b, r, t);
    return new RectSelection(l, b, r, t, this);
}

// GetOriginal2 returns the two corners that were passed to the
// RectSelection's constructor.

void RectSelection::GetOriginal2 (Coord& l, Coord& b, Coord& r, Coord& t) {
    ((Rect*) Last())->GetOriginal(l, b, r, t);
}

// GetOriginal returns the two corners that were passed to the
// RectSelection's constructor plus the other two opposite corners.

int RectSelection::GetOriginal (const Coord*& x, const Coord*& y) {
    static Coord sx[4], sy[4];
    GetOriginal2(sx[0], sy[0], sx[2], sy[2]);
    sx[1] = sx[0];
    sy[1] = sy[2];
    sx[3] = sx[2];
    sy[3] = sy[0];
    x = sx;
    y = sy;
    return 4;
}

// WriteData writes the RectSelection's data and Postscript code to
// draw it.

void RectSelection::WriteData (ostream& to) {
    Coord l, b, r, t;
    GetOriginal2(l, b, r, t);
    to << "Begin " << startdata << " Rect\n";
    WriteGS(to);
    to << startdata << "\n";
    to << l << " " << b << " " << r << " " << t << " Rect\n";
    to << "End\n\n";
}

// CreateRubberVertex creates and returns the right kind of
// RubberVertex to represent the RectSelection's shape.

RubberVertex* RectSelection::CreateRubberVertex (Coord* x, Coord* y,
int n, int rubpt) {
    return new RubberPolygon(nil, nil, x, y, n, rubpt);
}

// CreateReshapedCopy creates and returns a reshaped copy of itself
// using the passed points and its graphic state.  It returns a
// PolygonSelection because the points may not shape a rect any more.

Selection* RectSelection::CreateReshapedCopy (Coord* x, Coord* y, int n) {
    return new PolygonSelection(x, y, n, this);
}

// PolygonSelection creates the polygon's filled interior and outline.

PolygonSelection::PolygonSelection (Coord* x, Coord* y, int n, Graphic* gs)
: (gs) {
    myname = "Poly";
    Append(new IFillPolygon(x, y, n));
    Append(new Polygon(x, y, n));
}

// PolygonSelection reads data to initialize its graphic state and
// create its filled interior and outline.

PolygonSelection::PolygonSelection (istream& from, State* state) : (nil) {
    myname = "Poly";
    ReadGS(from, state);
    Coord* x;
    Coord* y;
    int n;
    ReadPoints(from, x, y, n);
    Append(new IFillPolygon(x, y, n));
    Append(new Polygon(x, y, n));
}

// Copy returns a copy of the PolygonSelection.

Graphic* PolygonSelection::Copy () {
    Coord* x;
    Coord* y;
    int n = GetOriginal(x, y);
    Graphic* copy = new PolygonSelection(x, y, n, this);
    return copy;
}

// GetOriginal returns the vertices that were passed to the
// PolygonSelection's constructor.

int PolygonSelection::GetOriginal (const Coord*& x, const Coord*& y) {
    return ((Polygon*) Last())->GetOriginal(x, y);
}

// CreateRubberVertex creates and returns the right kind of
// RubberVertex to represent the PolygonSelection's shape.

RubberVertex* PolygonSelection::CreateRubberVertex (Coord* x, Coord* y,
int n, int rubpt) {
    return new RubberPolygon(nil, nil, x, y, n, rubpt);
}

// CreateReshapedCopy creates and returns a reshaped copy of itself
// using the passed points and its graphic state.

Selection* PolygonSelection::CreateReshapedCopy (Coord* x, Coord* y, int n) {
    return new PolygonSelection(x, y, n, this);
}

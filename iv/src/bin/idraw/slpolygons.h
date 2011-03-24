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

// $Header: slpolygons.h,v 1.8 89/10/09 14:49:40 linton Exp $
// declares classes RectSelection and PolygonSelection.

#ifndef slpolygons_h
#define slpolygons_h

#include "selection.h"

// A RectSelection draws a rectangle with an outline and a filled
// interior.

class RectSelection : public NPtSelection {
public:

    RectSelection(Coord, Coord, Coord, Coord, Graphic* = nil);
    RectSelection(istream&, State*);

    Graphic* Copy();
    void GetOriginal2(Coord&, Coord&, Coord&, Coord&);
    int GetOriginal(const Coord*&, const Coord*&);

protected:

    void WriteData(ostream&);
    RubberVertex* CreateRubberVertex(Coord*, Coord*, int, int);
    Selection* CreateReshapedCopy(Coord*, Coord*, int);

};

// A PolygonSelection draws a polygon with an outline and a filled
// interior.

class PolygonSelection : public NPtSelection {
public:

    PolygonSelection(Coord*, Coord*, int, Graphic* = nil);
    PolygonSelection(istream&, State*);

    Graphic* Copy();
    int GetOriginal(const Coord*&, const Coord*&);

protected:

    RubberVertex* CreateRubberVertex(Coord*, Coord*, int, int);
    Selection* CreateReshapedCopy(Coord*, Coord*, int);

};

#endif

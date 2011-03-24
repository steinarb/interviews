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

// $Header: sllines.h,v 1.8 89/10/09 14:49:32 linton Exp $
// declares classes LineSelection and MultiLineSelection.

#ifndef sllines_h
#define sllines_h

#include "selection.h"

// A LineSelection draws a line.

class LineSelection : public NPtSelection {
public:

    LineSelection(Coord, Coord, Coord, Coord, Graphic* = nil);
    LineSelection(istream&, State*);

    Graphic* Copy();
    void GetOriginal2(Coord&, Coord&, Coord&, Coord&);
    int GetOriginal(const Coord*&, const Coord*&);

protected:

    void WriteData(ostream&);
    RubberVertex* CreateRubberVertex(Coord*, Coord*, int, int);
    Selection* CreateReshapedCopy(Coord*, Coord*, int);

    void uncacheChildren();
    void getExtent(float&, float&, float&, float&, float&, Graphic*);
    boolean contains(PointObj&, Graphic*);
    boolean intersects(BoxObj&, Graphic*);
    void draw(Canvas*, Graphic*);
    void drawClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);

    Graphic* line;		// draws the line

};

// A MultiLineSelection draws a set of connected lines with a filled
// interior.

class MultiLineSelection : public NPtSelection {
public:

    MultiLineSelection(Coord*, Coord*, int, Graphic* = nil);
    MultiLineSelection(istream&, State*);

    Graphic* Copy();
    int GetOriginal(const Coord*&, const Coord*&);

protected:

    void Init(Coord*, Coord*, int);
    RubberVertex* CreateRubberVertex(Coord*, Coord*, int, int);
    Selection* CreateReshapedCopy(Coord*, Coord*, int);

    void uncacheChildren();
    void getExtent(float&, float&, float&, float&, float&, Graphic*);
    boolean contains(PointObj&, Graphic*);
    boolean intersects(BoxObj&, Graphic*);
    void draw(Canvas*, Graphic*);
    void drawClipped(Canvas*, Coord, Coord, Coord, Coord, Graphic*);

    Graphic* ifillmultiline;	// fills a set of connected lines
    Graphic* multiline;		// draws a set of connected lines
    Coord lx0, ly0, lx1, ly1;	// stores endpoints of left arrowhead
    Coord rx0, ry0, rx1, ry1;	// stores endpoints of right arrowhead

};

#endif

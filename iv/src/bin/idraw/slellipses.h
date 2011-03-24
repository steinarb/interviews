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

// $Header: slellipses.h,v 1.7 89/10/09 14:49:28 linton Exp $
// declares classes EllipseSelection and CircleSelection.

#ifndef slellipses_h
#define slellipses_h

#include "selection.h"

// A EllipseSelection draws a ellipse with an outline and a filled
// interior.

class EllipseSelection : public Selection {
public:

    EllipseSelection(Coord, Coord, Coord, Coord, Graphic* = nil);
    EllipseSelection(istream&, State*);

    Graphic* Copy();
    void GetOriginal(Coord&, Coord&, int&, int&);

protected:

    void WriteData(ostream&);

};

// A CircleSelection draws a circle with an outline and a filled
// interior.

class CircleSelection : public Selection {
public:

    CircleSelection(Coord, Coord, int, Graphic* = nil);
    CircleSelection(istream&, State*);

    Graphic* Copy();
    void GetOriginal(Coord&, Coord&, int&);

protected:

    void WriteData(ostream&);

};

#endif

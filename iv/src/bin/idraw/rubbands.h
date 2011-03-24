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

// $Header: rubbands.h,v 1.9 89/10/09 14:49:20 linton Exp $
// defines classes IStretchingRect, RubberMultiLine, and RubberPolygon.

#ifndef rubbands_h
#define rubbands_h

#include <InterViews/rubcurve.h>
#include <InterViews/rubline.h>
#include <InterViews/rubrect.h>

// An IStretchingRect uses its first few Track calls to decide which
// side it will let the user drag.

class IStretchingRect : public StretchingRect {
public:

    IStretchingRect(Painter*, Canvas*, Coord, Coord, Coord, Coord,
		    Coord = 0, Coord = 0);

    void Track(Coord, Coord);
    void DefineSide(Coord, Coord);
    Alignment CurrentSide(boolean landscape);

protected:

    boolean firsttime;		// stores true until after first call of Track
    boolean undefinedside;	// stores true until side has been determined
    Coord cx, cy;		// stores original center of rectangle
    Coord origx, origy;		// stores point passed by first call of Track

};

// A RubberMultiLine lets the user drag one of its vertices.

class RubberMultiLine : public RubberVertex {
public:
    RubberMultiLine(
        Painter*, Canvas*, Coord px[], Coord py[], int n, int pt,
	Coord offx = 0, Coord offy = 0
    );
    void Draw();
};

// A RubberPolygon lets the user drag one of its vertices.

class RubberPolygon : public RubberVertex {
public:
    RubberPolygon(
        Painter*, Canvas*, Coord px[], Coord py[], int n, int pt,
	Coord offx = 0, Coord offy = 0
    );
    void Draw();
};

#endif

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

// $Header: isplines.h,v 1.7 89/10/09 14:48:27 linton Exp $
// declares classes IFillBSpline and IFillClosedBSpline.

#ifndef isplines_h
#define isplines_h

#include <InterViews/Graphic/splines.h>

// An IFillBSpline knows when NOT to draw itself.

class IFillBSpline : public FillBSpline {
public:

    IFillBSpline(Coord*, Coord*, int, Graphic* = nil);

protected:

    boolean contains(PointObj&, Graphic*);
    boolean intersects(BoxObj&, Graphic*);
    void draw(Canvas*, Graphic*);

};

// An IFillClosedBSpline knows when NOT to draw itself.

class IFillClosedBSpline : public FillBSpline {
public:

    IFillClosedBSpline(Coord*, Coord*, int, Graphic* = nil);

protected:

    boolean contains(PointObj&, Graphic*);
    boolean intersects(BoxObj&, Graphic*);
    void draw(Canvas*, Graphic*);

};

#endif

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

/*
 * Rubberbanding for simple lines.
 */

#ifndef rubline_h
#define rubline_h

#include <InterViews/rubband.h>

class RubberLine : public Rubberband {
public:
    RubberLine(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1,
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetOriginal(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    virtual void Draw();
protected:
    Coord fixedx, fixedy;
    Coord movingx, movingy;
};

class RubberAxis : public RubberLine {
public:
    RubberAxis(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1,
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
};

class SlidingLine : public RubberLine {
public:
    SlidingLine(
	Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1,
	Coord rfx, Coord rfy, Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
protected:
    Coord refx;
    Coord refy;
};

class ScalingLine : public RubberLine {
public:
    ScalingLine(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1,
	Coord cx, Coord cy, Coord offx = 0, Coord offy = 0
    );
    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    float CurrentScaling();
protected:
    Coord centerx, centery;
    int width, height;
};

class RotatingLine : public RubberLine {
public:
    RotatingLine(
	Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1, 
	Coord cx, Coord cy, Coord rfx, Coord rfy, 
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    float OriginalAngle();
    float CurrentAngle();
protected:
    void Transform (
	Coord& x, Coord& y,
	double a0, double a1, double b0, double b1, double c0, double c1
    );
protected:
    Coord centerx, centery, refx, refy;
};

#endif

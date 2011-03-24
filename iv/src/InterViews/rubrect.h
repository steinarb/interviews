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
 * Rubberbanding for rectangles.
 */

#ifndef rubrect_h
#define rubrect_h

#include <InterViews/rubband.h>

class RubberRect : public Rubberband {
public:
    RubberRect(
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

class RubberSquare : public RubberRect {
public:
    RubberSquare(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1,
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
};

class SlidingRect : public RubberRect {
public:
    SlidingRect(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1, 
	Coord rfx, Coord rfy, Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
protected:
    Coord refx;
    Coord refy;
};

class StretchingRect : public RubberRect {
public:
    StretchingRect (
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1, Side s,
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    float CurrentStretching();
protected:
    Side side;
};

class ScalingRect : public RubberRect {
public:
    ScalingRect(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1,
	Coord cx, Coord cy, Coord offx = 0, Coord offy = 0
    );

    virtual void GetCurrent(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    float CurrentScaling();
protected:
    Coord centerx, centery;
    int width, height;
};

class RotatingRect : public Rubberband {
public:
    RotatingRect(
        Painter*, Canvas*, Coord x0, Coord y0, Coord x1, Coord y1, 
	Coord cx, Coord cy, Coord rfx, Coord rfy, 
	Coord offx = 0, Coord offy = 0
    );

    virtual void Draw();
    virtual void GetOriginal(Coord& x0, Coord& y0, Coord& x1, Coord& y1);
    virtual void GetCurrent(
        Coord& leftbotx, Coord& leftboty,
	Coord& rightbotx, Coord& rightboty,
	Coord& righttopx, Coord& righttopy,
	Coord& lefttopx, Coord& lefttopy
    );
    float CurrentAngle();
protected:
    void Transform (
	Coord& x, Coord& y,
	double a0, double a1, double b0, double b1, double c0, double c1
    );
protected:
    Coord left, right, centerx, refx;
    Coord bottom, top, centery, refy;
};

#endif

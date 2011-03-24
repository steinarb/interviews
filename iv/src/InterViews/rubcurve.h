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
 * Rubberbanding for curves.
 */

#ifndef rubcurve_h
#define rubcurve_h

#include <InterViews/rubband.h>

class RubberEllipse : public Rubberband {
public:
    RubberEllipse(
        Painter*, Canvas*, Coord cx, Coord cy, Coord rx, Coord ry,
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetOriginal(Coord& cx, Coord& cy, Coord& rx, Coord& ry);
    virtual void GetCurrent(Coord& cx, Coord& cy, Coord& rx, Coord& ry);
    virtual void OriginalRadii(int& xr, int& yr);
    virtual void CurrentRadii(int& xr, int& yr);
    virtual void Draw();
protected:
    Coord centerx, radiusx;
    Coord centery, radiusy;
};

class SlidingEllipse : public RubberEllipse  {
public:
    SlidingEllipse(
        Painter*, Canvas*, Coord cx, Coord cy, Coord xr, Coord yr,
	Coord rfx, Coord rfy, Coord offx = 0, Coord offy = 0
    );
    virtual void GetCurrent(Coord& cx, Coord& cy, Coord& xr, Coord& yr);
    virtual void OriginalRadii(int&, int&);
    virtual void CurrentRadii(int&, int&);
protected:
    Coord refx;
    Coord refy;
};

class RubberCircle : public RubberEllipse {
public:
    RubberCircle(
        Painter*, Canvas*, Coord cx, Coord cy, Coord rx, Coord ry,
	Coord offx = 0, Coord offy = 0
    );

    virtual void OriginalRadii(int& xr, int& yr);
    virtual void CurrentRadii(int& xr, int& yr);
    virtual void Draw();
};

class RubberPointList : public Rubberband {
public:
    RubberPointList(
        Painter*, Canvas*, Coord px[], Coord py[], int n,
	Coord offx = 0, Coord offy = 0
    );
    ~RubberPointList();
protected:
    void Copy(Coord*, Coord*, int, Coord*&, Coord*&);
protected:
    Coord *x;
    Coord *y;
    int count;
};    

class RubberVertex : public RubberPointList {
public:
    RubberVertex(
        Painter*, Canvas*, Coord px[], Coord py[], int n, int pt,
	Coord offx = 0, Coord offy = 0
    );

    virtual void GetOriginal(Coord*& px, Coord*& py, int& n, int& pt);
    virtual void GetCurrent(Coord*& px, Coord*& py, int& n, int& pt);
protected:
    void DrawSplineSection (Painter*, Canvas*, Coord x[], Coord y[]);
protected:
    int rubberPt;
};

class RubberHandles : public RubberVertex {
public:
    RubberHandles(
        Painter*, Canvas*, Coord x[], Coord y[], int n, int pt, int size,
	Coord offx = 0, Coord offy = 0
    );

    virtual void Track(Coord x, Coord y);
    virtual void Draw();
protected:
    int d;	/* half of handle size */
};

class RubberSpline : public RubberVertex {
public:
    RubberSpline(
        Painter*, Canvas*, Coord px[], Coord py[], int n, int pt,
	Coord offx = 0, Coord offy = 0
    );

    virtual void Draw();
};

class RubberClosedSpline : public RubberVertex {
public:
    RubberClosedSpline(
        Painter*, Canvas*, Coord px[], Coord py[], int n, int pt,
	Coord offx = 0, Coord offy = 0
    );

    virtual void Draw();
};

class SlidingPointList : public RubberPointList {
public:
    SlidingPointList (
        Painter*, Canvas*, Coord px[], Coord py[], int n,
	Coord rfx, Coord rfy, Coord offx = 0, Coord offy = 0
    );

    virtual void GetOriginal(Coord*& px, Coord*& py, int& n);
    virtual void GetCurrent(Coord*& px, Coord*& py, int& n);
    virtual void Draw();
    virtual void Track(Coord x, Coord y);
protected:
    Coord refx;
    Coord refy;
};

class SlidingLineList : public SlidingPointList {
public:
    SlidingLineList(
        Painter*, Canvas*, Coord x[], Coord y[], int n,
	Coord rfx, Coord rfy, Coord offx = 0, Coord offy = 0
    );

    virtual void Draw();
};    

class ScalingLineList : public RubberPointList {
public:
    ScalingLineList (
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord cx, Coord cy, Coord rfx, Coord rfy,
	Coord offx = 0, Coord offy = 0
    );
    virtual ~ScalingLineList();

    virtual void GetOriginal(Coord*& px, Coord*& py, int& n);
    virtual void GetCurrent(Coord*& px, Coord*& py, int& n);
    virtual void Track(Coord, Coord);
    virtual void Draw();
    float CurrentScaling();
protected:
    virtual void Update();
protected:
    Coord* newx, *newy;
    Coord centerx, centery;
    float origVal;
};

class RotatingLineList : public ScalingLineList {
public:
    RotatingLineList (
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord cx, Coord cy, Coord rfx, Coord rfy,
	Coord offx = 0, Coord offy = 0
    );
    float OriginalAngle() { return origVal; }
    float CurrentAngle();
protected:
    virtual void Update();
};

#endif

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
 * GrowingVertices and subclasses - rubberbands defined by a set of vertices 
 * that can grow dynamically in number.
 */

#ifndef rubverts_h
#define rubverts_h

#include <InterViews/rubband.h>

class GrowingVertices : public Rubberband {
public:
    GrowingVertices(
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord offx = 0, Coord offy = 0
    );
    virtual ~GrowingVertices();

    virtual void AppendVertex(Coord, Coord);
    virtual void Draw();

    virtual void GetOriginal(Coord*& px, Coord*& py, int& n);
    virtual void GetCurrent(Coord*& px, Coord*& py, int& n);
protected:
    void CheckBufs();
    void Copy(Coord*, Coord*, int, Coord*&, Coord*&);
    virtual void DrawVertices(Coord*, Coord*, int);
protected:
    Coord* x, *y;
    int count, bufsize, origbufsize;
};

class GrowingMultiLine : public GrowingVertices {
public:
    GrowingMultiLine(
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord offx = 0, Coord offy = 0
    );
protected:
    virtual void DrawVertices(Coord*, Coord*, int);
};

class GrowingPolygon : public GrowingVertices {
public:
    GrowingPolygon(
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord offx = 0, Coord offy = 0
    );
protected:
    virtual void DrawVertices(Coord*, Coord*, int);
};

class GrowingBSpline : public GrowingVertices {
public:
    GrowingBSpline(
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord offx = 0, Coord offy = 0
    );
protected:
    virtual void DrawVertices(Coord*, Coord*, int);
};

class GrowingClosedBSpline : public GrowingVertices {
public:
    GrowingClosedBSpline(
        Painter*, Canvas*, Coord px[], Coord py[], int n,
        Coord offx = 0, Coord offy = 0
    );
protected:
    virtual void DrawVertices(Coord*, Coord*, int);
};

#endif

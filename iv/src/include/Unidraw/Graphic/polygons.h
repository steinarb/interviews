/*
 * Copyright (c) 1990, 1991 Stanford University
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
 * Interface to Rects and Polygons, objects derived from Graphic.
 */

#ifndef unidraw_graphic_polygons_h
#define unidraw_graphic_polygons_h

#include <Unidraw/Graphic/verts.h>

#include <IV-2_6/_enter.h>

class Rect : public Graphic {
public:
    void GetOriginal(Coord&, Coord&, Coord&, Coord&);
protected:
    Rect(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);

    void s_getExtent(float&, float&, float&, float&, float&, Graphic*);
    void f_getExtent(float&, float&, float&, float&, float&, Graphic*);

    boolean s_contains(PointObj&, Graphic*);
    boolean f_contains(PointObj&, Graphic*);
    boolean s_intersects(BoxObj&, Graphic*);
    boolean f_intersects(BoxObj&, Graphic*);
protected:
    Coord _x0, _y0, _x1, _y1;
};

class S_Rect : public Rect {
public:
    S_Rect(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);
    virtual ~S_Rect();

    virtual void SetBrush(PSBrush*);
    virtual PSBrush* GetBrush();

    virtual Graphic* Copy();
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
private:
    PSBrush* _br;
};

class F_Rect : public Rect {
public:
    F_Rect(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);
    virtual ~F_Rect();

    virtual void SetPattern(PSPattern*);
    virtual PSPattern* GetPattern();

    virtual Graphic* Copy();
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
private:
    PSPattern* _pat;
};

class SF_Rect : public Rect {
public:
    SF_Rect(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);
    virtual ~SF_Rect();

    virtual void SetBrush(PSBrush*);
    virtual PSBrush* GetBrush();
    virtual void SetPattern(PSPattern*);
    virtual PSPattern* GetPattern();

    virtual Graphic* Copy();
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
private:
    PSBrush* _br;
    PSPattern* _pat;
};

class Polygon : public Vertices {
protected:
    Polygon(Coord* x, Coord* y, int count, Graphic* gr = nil) ;

    boolean s_contains(PointObj&, Graphic*);
    boolean f_contains(PointObj&, Graphic*);
    boolean s_intersects(BoxObj&, Graphic*);
    boolean f_intersects(BoxObj&, Graphic*);
};

class S_Polygon : public Polygon {
public:
    S_Polygon(Coord* x, Coord* y, int count, Graphic* = nil);
    virtual ~S_Polygon();

    virtual void SetBrush(PSBrush*);
    virtual PSBrush* GetBrush();

    virtual Graphic* Copy();
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
private:
    PSBrush* _br;
};

class F_Polygon : public Polygon {
public:
    F_Polygon(Coord* x, Coord* y, int count, Graphic* = nil);
    virtual ~F_Polygon();

    virtual void SetPattern(PSPattern*);
    virtual PSPattern* GetPattern();

    virtual Graphic* Copy();
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
private:
    PSPattern* _pat;
};

class SF_Polygon : public Polygon {
public:
    SF_Polygon(Coord* x, Coord* y, int count, Graphic* = nil);
    virtual ~SF_Polygon();

    virtual void SetBrush(PSBrush*);
    virtual PSBrush* GetBrush();
    virtual void SetPattern(PSPattern*);
    virtual PSPattern* GetPattern();

    virtual Graphic* Copy();
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
private:
    PSBrush* _br;
    PSPattern* _pat;
};

#include <IV-2_6/_leave.h>

#endif

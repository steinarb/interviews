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
 * Interface to Rects and Polygons, objects derived from Graphic.
 */

#ifndef polygons_h
#define polygons_h

#include <InterViews/Graphic/lines.h>

class Rect : public Graphic {
public:
    Rect();
    Rect(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);

    void GetOriginal(Coord&, Coord&, Coord&, Coord&);
    virtual void SetBrush(PBrush*);
    virtual PBrush* GetBrush();
    virtual void SetPattern(PPattern*);
    virtual PPattern* GetPattern();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);

    virtual void pat(Ref);
    virtual Ref pat();
    virtual void br(Ref);
    virtual Ref br();

    virtual Graphic* Copy();
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
protected:
    Coord x0, y0, x1, y1;
    Ref _patbr;
};

class FillRect : public Rect {
public:
    FillRect();
    FillRect(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);

    virtual void SetBrush(PBrush*);

    virtual Graphic* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);

    virtual void pat(Ref);
    virtual Ref pat();
    virtual void br(Ref);
    virtual Ref br();
};

class Polygon : public MultiLine {
public:
    Polygon();
    Polygon(Coord* x, Coord* y, int count, Graphic* = nil);

    virtual Graphic* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);
};

class FillPolygon : public Polygon {
public:
    FillPolygon();
    FillPolygon(Coord* x, Coord* y, int count, Graphic* = nil);

    virtual void SetBrush(PBrush*);

    virtual Graphic* Copy();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual void getExtent(float&, float&, float&, float&, float&, Graphic*);
    virtual boolean contains(PointObj&, Graphic*);
    virtual boolean intersects(BoxObj&, Graphic*);
    virtual void draw(Canvas*, Graphic*);

    virtual void pat(Ref);
    virtual Ref pat();
    virtual void br(Ref);
    virtual Ref br();
};

#endif

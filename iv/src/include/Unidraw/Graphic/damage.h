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
 * Damage - maintains an array of non-overlapping rectangles representing
 * damaged areas of of a canvas, used for smart redraw.
 */

#ifndef unidraw_graphic_damage_h
#define unidraw_graphic_damage_h

#include <IV-2_6/InterViews/defs.h>
#include <Unidraw/enter-scope.h>

#include <IV-2_6/_enter.h>

class BoxObj;
class Canvas;
class Graphic;
class Iterator;
class Painter;
class UList;

class Damage {
public:
    Damage(Canvas* = nil, Painter* = nil, Graphic* = nil);
    virtual ~Damage();
    
    virtual boolean Incurred();
    virtual void Added(Graphic*);
    virtual void Incur(Graphic*);
    virtual void Incur(BoxObj&);
    virtual void Incur(Coord, Coord, Coord, Coord);
    virtual void Repair();
    virtual void Reset();

    void SetCanvas(Canvas*);
    void SetPainter(Painter*);
    void SetGraphic(Graphic*);

    Canvas* GetCanvas();
    Painter* GetPainter();
    Graphic* GetGraphic();
protected:    
    int Area(BoxObj&);

    virtual void DrawAreas();
    virtual void DrawAdditions();
    virtual void Merge(BoxObj&);

    BoxObj* GetArea(Iterator);
    Graphic* GetAddition(Iterator);

    void FirstArea(Iterator&);
    void FirstAddition(Iterator&);

    void Next(Iterator&);
    boolean Done(Iterator);

    UList* Elem(Iterator);

    void DeleteArea(BoxObj*);
    void DeleteAreas();
protected:
    UList* _additions;
    UList* _areas;
private:
    Canvas* _canvas;
    Painter* _output;
    Graphic* _graphic;
};

#include <IV-2_6/_leave.h>

#endif

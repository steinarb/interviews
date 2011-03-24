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
 * Damage - maintains an array of non-overlapping rectangles representing
 * damaged areas of of a canvas, used for smart redraw.
 */

#ifndef damage_h
#define damage_h

#include <InterViews/defs.h>

class BoxList;
class BoxObj;
class Canvas;
class Graphic;
class Painter;
class RefList;

class Damage {
public:
    Damage(Canvas* = nil, Painter* = nil, Graphic* = nil);
    ~Damage();
    
    boolean Incurred();
    void Added(Graphic*);
    void Incur(Graphic*);
    void Incur(BoxObj&);
    void Incur(Coord, Coord, Coord, Coord);
    void Repair();
    void Reset();

    void SetCanvas(Canvas*);
    void SetPainter(Painter*);
    void SetGraphic(Graphic*);

    Canvas* GetCanvas();
    Painter* GetPainter();
    Graphic* GetGraphic();
protected:    
    int Area(BoxObj&);
    void DrawAreas();
    void DrawAdditions();
    void Merge(BoxObj&);
protected:
    BoxList* areas;
    RefList* additions;
    Canvas* canvas;
    Painter* output;
    Graphic* graphic;
};

#endif

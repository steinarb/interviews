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
 * A canvas is an area of graphics output.
 */

#ifndef canvas_h
#define canvas_h

#include <InterViews/defs.h>

class Color;

enum CanvasStatus {
    CanvasMapped, CanvasUnmapped, CanvasOffscreen
};

class Canvas {
public:
    Canvas(int w, int h);
    Canvas(void*);
    ~Canvas();

    void* Id();
    int Width();
    int Height();
    CanvasStatus Status();

    void SetBackground(Color*);

    void Clip(Coord, Coord, Coord, Coord);
    void NoClip();
    void ClipOn();
    void ClipOff();
    boolean IsClipped();
    void Map(Coord&, Coord&);
private:
    friend class Interactor;
    friend class Scene;
    friend class World;
    friend class WorldView;
    friend class Painter;

    void* id;
    CanvasStatus status;
    int width, height;
    class CanvasRep* rep;

    void WaitForCopy();
};

inline void* Canvas::Id () { return id; }
inline int Canvas::Width () { return width; }
inline int Canvas::Height () { return height; }
inline CanvasStatus Canvas::Status () { return status; }

#endif

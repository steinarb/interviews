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
 * Rubber - rubberbanding graphical objects
 */

#ifndef rubband_h
#define rubband_h

#include <InterViews/defs.h>
#include <InterViews/resource.h>

enum Side { LeftSide, RightSide, BottomSide, TopSide };

class Canvas;
class Painter;

class Rubberband : public Resource {
public:
    Rubberband(Painter*, Canvas*, Coord offx, Coord offy);
    virtual ~Rubberband();

    virtual void Draw();
    virtual void Redraw();
    virtual void Erase();
    virtual void Track(Coord x, Coord y);

    virtual void SetPainter(Painter*);
    virtual void SetCanvas(Canvas*);
    Painter* GetPainter();
    Canvas* GetCanvas();
protected:
    float Angle(Coord, Coord, Coord, Coord);	// angle line makes w/horiz
    float Distance(Coord, Coord, Coord, Coord); // distance between 2 points
protected:
    Painter* output;
    Canvas* canvas;
    boolean drawn;
    Coord trackx, offx;
    Coord tracky, offy;
};

inline Painter* Rubberband::GetPainter () { return output; }
inline Canvas* Rubberband::GetCanvas () { return canvas; }

#endif

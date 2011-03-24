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
 * A brush specifies how lines should be drawn.
 */

#ifndef brush_h
#define brush_h

#include <InterViews/resource.h>

class Brush : public Resource {
public:
    Brush(int pattern, int width = 1);
    Brush(int* pattern, int count, int width = 1);
    ~Brush();

    int Width();
private:
    friend class Painter;

    class BrushRep* rep;
    int width;
};

class BrushRep {
    friend class Brush;
    friend class Painter;

    BrushRep(int* pattern, int count, int width);
    ~BrushRep();
    void* info;
    int count;
};

inline int Brush::Width () { return width; }

extern Brush* single;

#endif

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
 * Raster - rasterized image
 */

#ifndef raster_h
#define raster_h

#include <InterViews/defs.h>
#include <InterViews/resource.h>

class Color;
class Canvas;

class Raster : public Resource {
public:
    Raster(Color** data, int width, int height);
    Raster(Canvas*, int x0, int y0, int width, int height);
    Raster(Raster*);
    ~Raster();

    int Width();
    int Height();
    boolean Contains(int x, int y);
    Index(int x, int y);

    Color* Peek(int x, int y);
    void Poke(Color*, int x, int y);
protected:
    friend class Painter;

    Color** raster;
    class RasterRep* rep;
};

class RasterRep {
private:
    friend class Raster;
    friend class Painter;

    RasterRep(int w, int h);
    RasterRep(Canvas*, int x0, int y0, int w, int h);
    RasterRep(RasterRep*);
    ~RasterRep();

    int GetPixel(int x, int y);
    void PutPixel(int x, int y, int);
    void* GetData();

    int width;
    int height;
    void* data;
};

inline int Raster::Width () { return rep->width; }
inline int Raster::Height () { return rep->height; }
inline boolean Raster::Contains (int x, int y) {
    return x >= 0 && x < Width() && y >= 0 && y < Height();
}
inline int Raster::Index (int x, int y) { return y * Width() + x; }


#endif

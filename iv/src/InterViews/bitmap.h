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
 * Bitmap - a two-dimensional boolean mask
 */

#ifndef bitmap_h
#define bitmap_h

#include <InterViews/defs.h>
#include <InterViews/resource.h>

class Font;
class Transformer;

class Bitmap : public Resource {
public:
    Bitmap(const char* filename);
    Bitmap(
	void*, unsigned int width, unsigned int height, int x0 = 0, int y0 = 0
    );
    Bitmap(Font*, int);
    Bitmap(Bitmap*);
    ~Bitmap();

    void* Map();

    int Left();
    int Right();
    int Top();
    int Bottom();
    unsigned int Width();
    unsigned int Height();

    void Transform(Transformer*);
    void Scale(float sx, float sy);
    void Rotate(float angle);

    void FlipHorizontal();
    void FlipVertical();
    void Rotate90();
    void Rotate180();
    void Rotate270();
    void Invert();

    boolean Contains(int x, int y);
    boolean Peek(int x, int y);
    void Poke(boolean bit, int x, int y);
private:
    friend class Painter;

    class BitmapRep* rep;
};

enum BitTx {NoTx, FlipH, FlipV, Rot90, Rot180, Rot270, Inv};

class BitmapRep {
private:
    friend class Bitmap;
    friend class Painter;

    BitmapRep(const char* filename);
    BitmapRep(void* data, unsigned int w, unsigned int h, int x, int y);
    BitmapRep(Font*, int);
    BitmapRep(BitmapRep*, BitTx);
    BitmapRep(BitmapRep*, Transformer*);
    ~BitmapRep();

    void Touch();
    void PutBit(int x, int y, boolean bit);
    boolean GetBit(int x, int y);
    void* GetMap();
    void* GetData();

    unsigned int width;
    unsigned int height;
    int x0;
    int y0;
    void* map;
    void* data;
};

inline int Bitmap::Left () { return -rep->x0; }
inline int Bitmap::Right () { return rep->width - rep->x0 - 1; }
inline int Bitmap::Top () { return rep->height - rep->y0 - 1; }
inline int Bitmap::Bottom () { return -rep->y0; }
inline unsigned int Bitmap::Width () { return rep->width; }
inline unsigned int Bitmap::Height () { return rep->height; }

#endif

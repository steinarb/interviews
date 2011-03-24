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
 * Bitmap - Bit map for InterViews
 */

#include <InterViews/bitmap.h>
#include <InterViews/transformer.h>

Bitmap::Bitmap (const char* filename) {
    rep = new BitmapRep(filename);
}

Bitmap::Bitmap (void* d, unsigned int w, unsigned int h, int x, int y) {
    rep = new BitmapRep(d, w, h, x, y);
}

Bitmap::Bitmap (Font* f, int c) {
    rep = new BitmapRep(f, c);
}

Bitmap::Bitmap (Bitmap* b) {
    rep = new BitmapRep(b->rep, NoTx);
}

Bitmap::~Bitmap () {
    delete rep;
}

void* Bitmap::Map () {
    return rep->GetMap();
}

void Bitmap::Transform (Transformer* t) {
    BitmapRep* newrep = new BitmapRep(rep, t);
    delete rep;
    rep = newrep;
}

void Bitmap::Scale (float sx, float sy) {
    Transformer* t = new Transformer;
    t->Scale(sx,sy);
    Transform(t);
    Unref(t);
}

void Bitmap::Rotate (float angle) {
    Transformer* t = new Transformer;
    t->Rotate(angle);
    Transform(t);
    Unref(t);
}

void Bitmap::FlipHorizontal () {
    BitmapRep* newrep = new BitmapRep(rep, FlipH);
    delete rep;
    rep = newrep;
}

void Bitmap::FlipVertical () {
    BitmapRep* newrep = new BitmapRep(rep, FlipV);
    delete rep;
    rep = newrep;
}

void Bitmap::Invert () {
    BitmapRep* newrep = new BitmapRep(rep, Inv);
    delete rep;
    rep = newrep;
}

void Bitmap::Rotate90 () {
    BitmapRep* newrep = new BitmapRep(rep, Rot90);
    delete rep;
    rep = newrep;
}

void Bitmap::Rotate180 () {
    BitmapRep* newrep = new BitmapRep(rep, Rot180);
    delete rep;
    rep = newrep;
}

void Bitmap::Rotate270 () {
    BitmapRep* newrep = new BitmapRep(rep, Rot270);
    delete rep;
    rep = newrep;
}

boolean Bitmap::Contains (int x, int y) {
    return x >= Left() && x <= Right() && y >= Bottom() && y <= Top();
}

boolean Bitmap::Peek (int x, int y) {
    return Contains(x, y) ? rep->GetBit(x-Left(), y-Bottom()) : false;
}

void Bitmap::Poke (boolean bit, int x, int y) {
    if (Contains(x, y)) {
        rep->PutBit(x-Left(), y-Bottom(), bit);
        rep->Touch();
    }
}

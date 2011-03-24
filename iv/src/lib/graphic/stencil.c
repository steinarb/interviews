/*
 * Copyright (c) 1989 Stanford University
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
 * Implementation of Stencil, an object derived from Graphic.
 */

#include <InterViews/bitmap.h>
#include <InterViews/transformer.h>
#include <InterViews/Graphic/grclasses.h>
#include <InterViews/Graphic/stencil.h>

void Stencil::draw(Canvas *c, Graphic* gs) {
    update(gs);
    pStencil(c, 0, 0, image, mask);
}

ClassId Stencil::GetClassId() { return STENCIL; }
boolean Stencil::IsA(ClassId id) { return STENCIL == id || Graphic::IsA(id); }
Stencil::Stencil() { image = mask = nil; }

Stencil::Stencil(Bitmap* i, Bitmap* m, Graphic* gr) : Graphic(gr) {
    image = i;
    image->Reference();
    mask = m;
    if (mask != nil) {
	mask->Reference();
    }
}

Graphic* Stencil::Copy() {
    Bitmap* newMask = (mask == nil) ? nil : new Bitmap(*mask);

    return new Stencil(new Bitmap(*image), newMask, this);
}

Stencil::~Stencil() {
    Unref(image);
    Unref(mask);
}

void Stencil::GetOriginal(Bitmap*& i, Bitmap*& m) {
    i = image;
    m = mask;
}

boolean Stencil::readBitmap(PFile* f, Bitmap*& bitmap) {
    int w, h, bit;
    boolean ok = f->Read(w) && f->Read(h);

    if (ok) {
        bitmap = new Bitmap((void*)nil, w, h);
	bitmap->Reference();

        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h && ok; ++j) {
                ok = f->Read(bit);
                bitmap->Poke(bit, i, j);
            }
        }
    }
    return ok;
}
            
boolean Stencil::writeBitmap(PFile* f, Bitmap* bitmap) {
    int w = bitmap->Width();
    int h = bitmap->Height();
    boolean ok = f->Write(w) && f->Write(h);

    if (ok) {
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h && ok; ++j) {
                ok = f->Write(int(bitmap->Peek(i, j)));
            }
        }
    }
    return ok;
}

boolean Stencil::read(PFile* f) {
    int noMask;
    boolean ok =
        Graphic::read(f) && readBitmap(f, image) && f->Read(noMask);

    if (ok && !noMask) {
        ok = readBitmap(f, mask);
    }
    return ok;
}

boolean Stencil::write(PFile* f) {
    int noMask = mask == nil;
    boolean ok =
        Graphic::write(f) && writeBitmap(f, image) && f->Write(noMask);

    if (ok && !noMask) {
        ok = writeBitmap(f, mask);
    }
    return ok;
}

void Stencil::getExtent(
    float& x0, float& y0, float& cx, float& cy, float& tol, Graphic* gs
) {
    Bitmap* bitmap = (mask == nil) ? image : mask;

    if (gs->GetTransformer() == nil) {
	x0 = y0 = 0;
	cx = bitmap->Width() / 2;
	cy = bitmap->Height() / 2;
    } else {
	transformRect(0,0,bitmap->Width(),bitmap->Height(),x0,y0,cx,cy, gs);
	cx = (cx + x0)/2;
	cy = (cy + y0)/2;
    }
    tol = 0;
}

boolean Stencil::contains(PointObj& po, Graphic* gs) {
    Bitmap* bitmap = (mask == nil) ? image : mask;
    PointObj pt (&po);

    invTransform(pt.x, pt.y, gs);
    BoxObj b (0, 0, bitmap->Width(), bitmap->Height());
    return b.Contains(pt);
}

boolean Stencil::intersects(BoxObj& userb, Graphic* gs) {
    Transformer* t = gs->GetTransformer();
    Bitmap* bitmap = (mask == nil) ? image : mask;
    Coord xmax = bitmap->Width();
    Coord ymax = bitmap->Height();
    Coord tx0, ty0, tx1, ty1;
    
    if (t != nil && t->Rotated()) {
	Coord x[4], tx[5];
	Coord y[4], ty[5];
    
	x[0] = x[3] = y[0] = y[1] = 0;
	x[2] = x[1] = xmax;
	y[2] = y[3] = ymax;
	transformList(x, y, 4, tx, ty, gs);
	tx[4] = tx[0];
	ty[4] = ty[0];
	FillPolygonObj fp (tx, ty, 5);
	return fp.Intersects(userb);
    
    } else if (t != nil) {
	t->Transform(0, 0, tx0, ty0);
	t->Transform(xmax, ymax, tx1, ty1);
	BoxObj b1 (tx0, ty0, tx1, ty1);
	return b1.Intersects(userb);

    } else {
	BoxObj b2 (0, 0, xmax, ymax);
	return b2.Intersects(userb);
    }
}

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
 * Implementation of RasterRect, an object derived from Graphic.
 */

#include <InterViews/raster.h>
#include <InterViews/transformer.h>
#include <InterViews/Graphic/grclasses.h>
#include <InterViews/Graphic/rasterrect.h>

/*****************************************************************************/

class ColorMaker {
public:
    ColorMaker();
    ~ColorMaker();
    Color* MakeColor(ColorIntensity r, ColorIntensity g, ColorIntensity b);
private:
    int size;
    Color** colors;
    void Grow(int size);
};

ColorMaker::ColorMaker() {
    size = 0;
    colors = nil;
    Grow(256);
}

ColorMaker::~ColorMaker() {
    for (int i = 0; i < size; ++i) {
	Unref(colors[i]);
    }
    delete colors;
}

void ColorMaker::Grow(int s) {
    Color** newcolors = new Color*[s];
    for (int i = 0; i < s; ++i) {
        if (i < size) {
            newcolors[i] = colors[i];
        } else {
            newcolors[i] = nil;
        }
    }
    delete colors;
    colors = newcolors;
    size = s;
}

Color* ColorMaker::MakeColor(
    ColorIntensity r, ColorIntensity g, ColorIntensity b
) {
    int hash = int(65535*(r + g + b)) % size;
    for (int i = 0; i < 256; ++i) {
        int index = (i + hash) % size;
        if (colors[index] != nil) {
            ColorIntensity rr, gg, bb;
            colors[index]->Intensities(rr, gg, bb);
            if (rr == r && gg == g && bb == b) {
                return colors[index];
            } else {
                i = (i+1) % size;
            }
        } else {
	    Color* c = new Color(r, g, b);
	    c->Reference();
            colors[index] = c;
            return c;
        }
    }
    Grow(size*2);
    return MakeColor(r, g, b);
}

/*****************************************************************************/

ColorMaker* RasterRect::colorMaker;

void RasterRect::draw(Canvas *c, Graphic* gs) {
    update(gs);
    pRasterRect(c, 0, 0, raster);
}

ClassId RasterRect::GetClassId() { return RASTERRECT; }

boolean RasterRect::IsA(ClassId id) { 
    return RASTERRECT == id || Graphic::IsA(id); 
}

void RasterRect::Init() {
    if (colorMaker == nil) {
        colorMaker = new ColorMaker;
    }
}

RasterRect::RasterRect() { 
    Init();
    raster = nil;
}

RasterRect::RasterRect(Raster* r, Graphic* gr) : Graphic(gr) {
    Init();
    raster = r;
    raster->Reference();
}

Graphic* RasterRect::Copy() {
    return new RasterRect(new Raster(*raster), this);
}

RasterRect::~RasterRect() {
    Unref(raster);
}

Raster* RasterRect::GetOriginal() { return raster; }

boolean RasterRect::readRaster(PFile* f, Raster*& raster) {
    int w, h;
    boolean ok = f->Read(w) && f->Read(h);

    if (ok) {
        raster = new Raster(w, h);
	raster->Reference();

	ColorIntensity r, g, b;
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h && ok; ++j) {
                ok = f->Read(r) && f->Read(g) && f->Read(b);
		float fr, fg, fb;
		fr = float(r) / float(0xffff);
		fg = float(g) / float(0xffff);
		fb = float(b) / float(0xffff);
                raster->poke(i, j, fr, fg, fb, 1.0);
            }
        }
    }
    return ok;
}
            
boolean RasterRect::writeRaster(PFile* f, Raster* raster) {
    int w = (int) raster->pwidth();
    int h = (int) raster->pheight();
    ColorIntensity r, g, b;
    float alpha;
    boolean ok = f->Write(w) && f->Write(h);

    if (ok) {
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h && ok; ++j) {
		float fr, fg, fb;
                raster->peek(i, j, fr, fg, fb, alpha);
		r = round(fr * float(0xffff));
		g = round(fg * float(0xffff));
		b = round(fb * float(0xffff));
                ok = f->Write(r) && f->Write(g) && f->Write(b);
            }
        }
    }
    return ok;
}

boolean RasterRect::read(PFile* f) {
    return Graphic::read(f) && readRaster(f, raster);
}

boolean RasterRect::write(PFile* f) {
    return Graphic::write(f) && writeRaster(f, raster);
}

void RasterRect::getExtent(
    float& x0, float& y0, float& cx, float& cy, float& tol, Graphic* gs
) {
    if (gs->GetTransformer() == nil) {
	x0 = y0 = 0;
	cx = raster->pwidth() / 2;
	cy = raster->pheight() / 2;
    } else {
	transformRect(0,0,raster->pwidth(),raster->pheight(),x0,y0,cx,cy,gs);
	cx = (cx + x0)/2;
	cy = (cy + y0)/2;
    }
    tol = 0;
}

boolean RasterRect::contains(PointObj& po, Graphic* gs) {
    PointObj pt (&po);
    invTransform(pt.x, pt.y, gs);
    BoxObj b (0, 0, (int) raster->pwidth(), (int) raster->pheight());
    return b.Contains(pt);
}

boolean RasterRect::intersects(BoxObj& userb, Graphic* gs) {
    Transformer* t = gs->GetTransformer();
    Coord xmax = (Coord) raster->pwidth();
    Coord ymax = (Coord) raster->pheight();
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

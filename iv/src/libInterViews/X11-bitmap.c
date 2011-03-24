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
 * X11-dependent bitmap code
 */

#include <InterViews/bitmap.h>
#include <InterViews/font.h>
#include <InterViews/transformer.h>
#include <InterViews/X11/painterrep.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <X11/Xatom.h>
#include <bstring.h>
#include <math.h>

/*
 * class Bitmap
 */

static int BitmapImageSize (unsigned int w, unsigned int h) {
    return (w+7)/8 * h;
}

BitmapRep::BitmapRep (const char* filename) {
    XReadBitmapFile(
	_world->display(), _world->root(), filename, &width, &height,
	(Pixmap*)&map, &x0, &y0
    );
    if (x0 == -1) {
        x0 = 0;
    }
    if (y0 == -1) {
        y0 = 0;
    } else {
        y0 = height-1 - y0;
    }
    data = nil;
}

BitmapRep::BitmapRep (void* d, unsigned int w, unsigned int h, int x, int y) {
    width = w;
    height = h;
    x0 = x;
    y0 = y;
    int size = BitmapImageSize(width, height);
    char* newd = new char[size];
    if (d != nil) {
	bcopy(d, newd, size);
    } else {
	bzero(newd, size);
    }
    map = (void*)XCreateBitmapFromData(
	_world->display(), _world->root(), newd, w, h
    );
    data = nil;
}    

BitmapRep::BitmapRep (Font* f, int c) {
    int w, h;
    XFontStruct* info = (XFontStruct*)f->Info();
    if (
        c >= info->min_char_or_byte2 && c <= info->max_char_or_byte2
        && info->per_char != nil
    ) {
        int i = c - info->min_char_or_byte2;
        w = info->per_char[i].rbearing - info->per_char[i].lbearing;
        h = info->per_char[i].ascent + info->per_char[i].descent;
        x0 = - info->per_char[i].lbearing;
        y0 = info->per_char[i].descent - 1;
    } else {
        w = info->max_bounds.rbearing - info->min_bounds.lbearing;
        h = info->max_bounds.ascent + info->max_bounds.descent;
        x0 = -info->min_bounds.lbearing;
        y0 = info->max_bounds.descent - 1;
    }
    width = (w <= 0) ? 1 : w;
    height = (h <= 0) ? 1 : h;
    map = (void*)XCreatePixmap(
        _world->display(), _world->root(), width, height, 1
    );
    GC gc = XCreateGC(_world->display(), (Pixmap)map, 0, nil);
    XSetFont(_world->display(), gc, info->fid);
    XSetForeground(_world->display(), gc, 0);
    XFillRectangle(_world->display(), (Pixmap)map, gc, 0, 0, width, height);
    XSetForeground(_world->display(), gc, 1);
    char ch = c;
    XDrawString(_world->display(), (Pixmap)map, gc, x0, height-1-y0, &ch, 1);
    XFreeGC(_world->display(), gc);
    data = nil;
}    

BitmapRep::BitmapRep (BitmapRep* b, BitTx t) {
    switch (t) {
    case NoTx: case FlipH: case FlipV: case Rot180: case Inv:
        width = b->width; height = b->height; break;
    case Rot90: case Rot270:
        width = b->height; height = b->width; break;
    }
    x0 = b->x0;
    y0 = b->y0;
    map = nil;
    int size = BitmapImageSize(width, height);
    char* d = new char[size];
    data = XCreateImage(
        _world->display(), _world->visual(),
        1, ZPixmap, 0, d, width, height, 8, 0
    );
    for (int x = 0; x < width; ++x) {
	for (int y = 0; y < height; ++y) {
            boolean bit;
            switch(t) {
            case NoTx:   bit = b->GetBit(x, y); break;
            case FlipH:  bit = b->GetBit(width-x-1, y); break;
            case FlipV:  bit = b->GetBit(x, height-y-1); break;
            case Rot90:  bit = b->GetBit(height-y-1, x); break;
            case Rot180: bit = b->GetBit(width-x-1, height-y-1); break;
            case Rot270: bit = b->GetBit(y, width-x-1); break;
            case Inv: bit = !b->GetBit(x, y); break;
            }
            PutBit(x, y, bit);
	}
    }
}

static void DrawSourceTransformedImage (
    XImage* s, int sx0, int sy0,
    XImage* m, int mx0, int my0,
    Drawable d, unsigned int height, int dx0, int dy0,
    boolean stencil, unsigned long fg, unsigned long bg,
    GC gc, Transformer* matrix,
    int xmin, int ymin, int xmax, int ymax
) {
    unsigned long lastdrawnpixel = fg;
    for (int xx = xmin; xx <= xmax; ++xx) {
        float lx, ly;
        float rx, ry;
        float tx, ty;
        matrix->Transform(float(xx), float(ymin), lx, ly);
        matrix->Transform(float(xx + 1), float(ymin), rx, ry);
        matrix->Transform(float(xx), float(ymax+1), tx, ty);
        float dx = (tx - lx) / float(ymax - ymin + 1);
        float dy = (ty - ly) / float(ymax - ymin + 1);
        int ilx = 0, ily = 0;
        int irx = 0, iry = 0;
        boolean lastmask = false, mask;
        unsigned long lastpixel = fg, pixel, source;
        for (int yy = ymin; yy <= ymax+1; ++yy) {
            mask = (
                yy <= ymax
                && (m == nil || XGetPixel(m, xx-mx0, m->height-1-(yy-my0)))
            );
            if (
                yy<sy0 || yy>=sy0+s->height || xx<sx0 || xx>=sx0+s->width
            ) {
                source = bg;
            } else {
                source = XGetPixel(s, xx-sx0, s->height-1-(yy-sy0));
            }
            if (stencil) {
                pixel = (source != 0) ? fg : bg;
            } else {
                pixel = source;
            }
            if (mask != lastmask || lastmask && pixel != lastpixel) {
                int iilx = round(lx), iily = round(ly);
                int iirx = round(rx), iiry = round(ry);
                if (lastmask) {
                    if (lastpixel != lastdrawnpixel) {
                        XSetForeground(_world->display(), gc, lastpixel);
                        lastdrawnpixel = lastpixel;
                    }
                    if (
                        (ilx==iilx || ily==iily) && (irx==ilx || iry==ily)
                    ) {
                        XFillRectangle(
                            _world->display(), d, gc,
                            min(ilx, iirx) - dx0,
                            height - (max(ily, iiry) - dy0),
                            abs(ilx - iirx), abs(ily - iiry)
                        );
                    } else {
                        XPoint v[4];
                        v[0].x = ilx-dx0; v[0].y = height - (ily-dy0);
                        v[1].x = iilx-dx0; v[1].y = height - (iily-dy0);
                        v[2].x = iirx-dx0; v[2].y = height - (iiry-dy0);
                        v[3].x = irx-dx0; v[3].y = height - (iry-dy0);
                        XFillPolygon(
                            _world->display(), d, gc,
                            v, 4, Convex, CoordModeOrigin
                        );
                    }
                }
                ilx = iilx; ily = iily;
                irx = iirx; iry = iiry;
                lastpixel = pixel;
                lastmask = mask;
            }
            lx += dx; ly += dy;
            rx += dx; ry += dy;
        }
    }
    XSetForeground(_world->display(), gc, fg);
}

static void DrawDestinationTransformedImage (
    XImage* s, int sx0, int sy0,
    XImage* m, int mx0, int my0,
    Drawable d, unsigned int height, int dx0, int dy0,
    boolean stencil, unsigned long fg, unsigned long bg,
    GC gc, Transformer* matrix,
    int xmin, int ymin, int xmax, int ymax
) {
    Transformer t(matrix);
    t.Invert();

    unsigned long lastdrawnpixel = fg;
    for (Coord xx = xmin; xx <= xmax; ++xx) {
        float fx, fy;
        float tx, ty;
        t.Transform(float(xx) + 0.5, float(ymin) + 0.5, fx, fy);
        t.Transform(float(xx) + 0.5, float(ymax) + 1.5, tx, ty);
        float dx = (tx - fx) / float(ymax - ymin + 1); 
        float dy = (ty - fy) / float(ymax - ymin + 1);
        Coord lasty = ymin;
        boolean lastmask = false, mask;
        unsigned long lastpixel = fg, pixel, source;
        for (Coord yy = ymin; yy <= ymax+1; ++yy) {
            int ix = round(fx - 0.5), iy = round(fy - 0.5);
            boolean insource = (
               ix >= sx0 && ix < sx0 + s->width
               && iy >= sy0 && iy < sy0 + s->height
            );
            boolean inmask = (
                m != nil && ix >= mx0 && ix < mx0 + m->width
                && iy >= my0 && iy < my0 + m->height
            );
            if (yy <= ymax) {
                if (m == nil) {
                    mask = insource;
                } else if (inmask) {
                    mask = XGetPixel(m, ix-mx0, m->height-1-(iy-my0)) != 0;
                } else {
                    mask = false;
                }
            } else {
                mask = false;
            }
            if (insource) {
                source = XGetPixel(s, ix-sx0, s->height-1-(iy-sy0));
            } else {
                source = bg;
            }
            if (stencil) {
                pixel = (source != 0) ? fg : bg;
            } else {
                pixel = source;
            }
            if (mask != lastmask || lastmask && pixel != lastpixel) {
                if (lastmask) {
                    if (lastpixel != lastdrawnpixel) {
                        XSetForeground(_world->display(), gc, lastpixel);
                        lastdrawnpixel = lastpixel;
                    }
                    XFillRectangle(
                        _world->display(), d, gc,
                        xx - dx0, height - (yy - dy0),
                        1, yy - lasty
                    );
                }
                lastmask = mask;
                lastpixel = pixel;
                lasty = yy;
            }
            fx += dx;
            fy += dy;
        }
    }
    XSetForeground(_world->display(), gc, fg);
}

void DrawTransformedImage (
    XImage* s, int sx0, int sy0,
    XImage* m, int mx0, int my0,
    Drawable d, unsigned int height, int dx0, int dy0,
    boolean stencil, unsigned long fg, unsigned long bg,
    GC gc, Transformer* matrix
) {
    int x1 = (m != nil) ? mx0 : sx0;
    int y1 = (m != nil) ? my0 : sy0;
    int x2 = (m != nil) ? mx0 : sx0;
    int y2 = (m != nil) ? my0 + m->height : sy0 + s->height;
    int x3 = (m != nil) ? mx0 + m->width : sx0 + s->width;
    int y3 = (m != nil) ? my0 + m->height : sy0 + s->height;
    int x4 = (m != nil) ? mx0 + m->width : sx0 + s->width;
    int y4 = (m != nil) ? my0 : sy0;

    int sxmin = min(x1, min(x2, min(x3, x4)));
    int sxmax = max(x1, max(x2, max(x3, x4))) - 1;
    int symin = min(y1, min(y2, min(y3, y4)));
    int symax = max(y1, max(y2, max(y3, y4))) - 1;

    matrix->Transform(x1, y1);
    matrix->Transform(x2, y2);
    matrix->Transform(x3, y3);
    matrix->Transform(x4, y4);

    int dxmin = min(x1,min(x2,min(x3,x4)));
    int dxmax = max(x1,max(x2,max(x3,x4))) - 1;
    int dymin = min(y1,min(y2,min(y3,y4)));
    int dymax = max(y1,max(y2,max(y3,y4))) - 1;

    int swidth = sxmax - sxmin + 1;
    int sheight = symax - symin + 1;
    int dwidth = dxmax - dxmin + 1;
    int dheight = dymax - dymin + 1;

    boolean rect = (x1==x2 || y1==y2) && (x1==x4 || y1==y4);
    boolean alwaysdest = dwidth < 2 * swidth;
    boolean alwayssource = dwidth * dheight > 3 * swidth * sheight;
    boolean dest;

    switch (_world->tximages()) {
    case TxImagesDefault:
    case TxImagesAuto:
        dest = alwaysdest || (!alwayssource && !rect);
        break;
    case TxImagesDest:
        dest = true;
        break;
    case TxImagesSource:
        dest = false;
        break;
    }
    if (dest) {
        if (dheight > 0) {
            DrawDestinationTransformedImage(
                s, sx0, sy0, m, mx0, my0, d, height, dx0, dy0,
                stencil, fg, bg, gc, matrix,
                dxmin, dymin, dxmax, dymax
            );
        }
    } else {
        if (sheight > 0) {
            DrawSourceTransformedImage(
                s, sx0, sy0, m, mx0, my0, d, height, dx0, dy0,
                stencil, fg, bg, gc, matrix,
                sxmin, symin, sxmax, symax
            );
        }
    }
}

BitmapRep::BitmapRep (BitmapRep* b, Transformer* matrix) {
    Transformer t(matrix);

    Coord x1 = - b->x0;
    Coord y1 = - b->y0;
    Coord x2 = - b->x0;
    Coord y2 = b->height - b->y0;
    Coord x3 = b->width - b->x0;
    Coord y3 = b->height - b->y0;
    Coord x4 = b->width - b->x0;
    Coord y4 = - b->y0;

    t.Transform(x1, y1);
    t.Transform(x2, y2);
    t.Transform(x3, y3);
    t.Transform(x4, y4);

    Coord xmax = max(x1,max(x2,max(x3,x4))) - 1;
    Coord xmin = min(x1,min(x2,min(x3,x4)));
    Coord ymax = max(y1,max(y2,max(y3,y4))) - 1;
    Coord ymin = min(y1,min(y2,min(y3,y4)));

    int w = xmax - xmin + 1;
    int h = ymax - ymin + 1;
    width = (w <= 0) ? 1 : w;
    height = (h <= 0) ? 1 : h;
    x0 = -xmin;
    y0 = -ymin;
    map = (void*)XCreatePixmap(
        _world->display(), _world->root(), width, height, 1
    );
    GC gc = XCreateGC(_world->display(), (Pixmap)map, 0, nil);
    XSetForeground(_world->display(), gc, 0);
    XFillRectangle(_world->display(), (Pixmap)map, gc, 0, 0, width, height);
    XSetForeground(_world->display(), gc, 1);
    DrawTransformedImage(
        (XImage*)b->GetData(), -b->x0, -b->y0,
        (XImage*)b->GetData(), -b->x0, -b->y0,
        (Pixmap)map, height, -x0, -y0,
        true, 1, 0, gc, &t
    );
    XFreeGC(_world->display(), gc);
    data = nil;
}

BitmapRep::~BitmapRep () {
    Touch();
    if (data != nil) {
        XDestroyImage((XImage*)data);
        data = nil;
    }
}

void BitmapRep::Touch () {
    if (map != nil) {
	XFreePixmap(_world->display(), (Pixmap)map);
	map = nil;
    }
}

void BitmapRep::PutBit (int x, int y, boolean bit) {
    XImage* image = (XImage*)GetData();
    if (image != nil) {
        XPutPixel(image, x, height - y - 1, bit);
    }
}

boolean BitmapRep::GetBit (int x, int y) {
    XImage* image = (XImage*)GetData();
    if (image != nil) {
        return XGetPixel(image, x, height - y - 1) != 0;
    } else {
        return false;
    }
}

void* BitmapRep::GetData () {
    if (data == nil && map != nil) {
        data = XGetImage(
            _world->display(), (Pixmap)map, 0, 0, width, height, 0x01, ZPixmap
        );
    }
    return data;
}        

void* BitmapRep::GetMap () {
    if (map == nil && data != nil) {
        map = (void*)XCreatePixmap(
            _world->display(), _world->root(), width, height, 1
        );
	GC gc = XCreateGC(_world->display(), (Pixmap)map, 0, nil);
        XPutImage(
            _world->display(), (Pixmap)map, gc, (XImage*)data,
            0, 0, 0, 0, width, height
        );
	XFreeGC(_world->display(), gc);
    }
    return map;
}

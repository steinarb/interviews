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
 * X11-dependent paint code
 */

#include <InterViews/bitmap.h>
#include <InterViews/brush.h>
#include <InterViews/color.h>
#include <InterViews/font.h>
#include <InterViews/pattern.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>
#include <string.h>

/*
 * class Brush
 */

static boolean PatternBit (int i, int* pattern, int count) {
    boolean bit = true;
    int index = 0;
    while (i >= pattern[index]) {
        i -= pattern[index];
        bit = !bit;
        index = (index + 1) % count;
    }
    return bit;
}

BrushRep::BrushRep (int* pattern, int c, int width) {
    if (c != 0) {
        if (
            _world->dash() == DashAll
            || width == 0 && _world->dash() == DashThin
            || width == 0 && _world->dash() == DashDefault
        ) {
            count = c;
            info = new char[count];
            for (int i = 0; i < count; ++i) {
                *((char*)info + i) = char(pattern[i]);
            }
        } else {
            count = 0;
            const int width = 32;
            const int height = 32;
            boolean bits[width];
            for (int i = 0; i < width; ++i) {
                bits[i] = PatternBit(i, pattern, c);
            }
            info = (void*)XCreatePixmap(
                _world->display(), _world->root(), width, height, 1
            );
            GC gc = XCreateGC(_world->display(), (Pixmap)info, 0, nil);
            XSetForeground(_world->display(), gc, 0);
            XFillRectangle(
                _world->display(), (Pixmap)info, gc, 0, 0, width, height
            );
            XSetForeground(_world->display(), gc, 1);
            for (int x = 0; x < width; ++x) {
                for (int y = 0; y < height; ++y) {
                    if (bits[(x + y)%width]) {
                        XDrawPoint(
                            _world->display(), (Drawable)info, gc, x, y
                        );
                    }
                }
            }
            XFreeGC(_world->display(), gc);
        }
    } else {
        count = 0;
        info = nil;
    }
}

BrushRep::~BrushRep () {
    if (info != nil) {
        if (count != 0) {
            delete info;
        } else {
            XFreePixmap(_world->display(), (Pixmap)info);
        }
    }
}

/*
 * class Color
 */

ColorRep::ColorRep (ColorIntensity r, ColorIntensity g, ColorIntensity b) {
    XColor* c = new XColor;
    c->red = r;
    c->green = g;
    c->blue = b;
    if (XAllocColor(_world->display(), _world->cmap(), c)) {
        info = (void*)c;
    } else {
        delete c;
        info = nil;
    }
}

ColorRep::ColorRep (
    int p, ColorIntensity& r, ColorIntensity& g, ColorIntensity& b
) {
    XColor* c = new XColor;
    c->pixel = p;
    XQueryColor(_world->display(), _world->cmap(), c);
    r = c->red;
    g = c->green;
    b = c->blue;
    info = (void*)c;
}

ColorRep::ColorRep (
    const char* name, ColorIntensity& r, ColorIntensity& g, ColorIntensity& b
) {
    XColor* c = new XColor;
    if (XParseColor(_world->display(), _world->cmap(), name, c) &&
 	XAllocColor(_world->display(), _world->cmap(), c)
    ) {
        r = c->red;
        g = c->green;
        b = c->blue;
        info = (void*)c;
    } else {
        delete c;
        info = nil;
    }
}

ColorRep::~ColorRep () {
    /* don't deallocate for now - needs fixing
    unsigned long p[1];

    p[0] = ((XColor*)info)->pixel;
    XFreeColors(_world->display(), _world->cmap(), p, 1, 0);
    */
}

int ColorRep::GetPixel () {
    XColor* c = (XColor*)info;
    return c->pixel;
}

void ColorRep::GetIntensities (
    ColorIntensity& red, ColorIntensity& green, ColorIntensity& blue
) {
    XColor* c = (XColor*)info;
    red = c->red;
    green = c->green;
    blue = c->blue;
}

/*
 * class Font
 */

void Font::GetFontByName (const char* name) {
    rep->info = XLoadQueryFont(_world->display(), name);
    Init();
}

inline boolean IsFixedWidth (XFontStruct* i) {
    return i->min_bounds.width == i->max_bounds.width;
}

void Font::Init () {
    register XFontStruct* i = (XFontStruct*)rep->info;
    if (i != nil) {
	rep->id = (void*)i->fid;
	rep->height = i->ascent + i->descent;
    } else {
	rep->height = -1;
    }
}

Font::~Font () {
    Unref(rep);
}

FontRep::~FontRep () {
    if (info != nil) {
	XFreeFont(_world->display(), (XFontStruct*)info);
    }
}

int Font::Baseline () {
    XFontStruct* i = (XFontStruct*)rep->info;
    return i->descent - 1;
}

int Font::Width (const char* s) {
    return XTextWidth((XFontStruct*)rep->info, s, strlen(s));
}

int Font::Width (const char* s, int len) {
    register const char* p, * q;

    q = &s[len];
    for (p = s; *p != '\0' && p < q; p++);
    return XTextWidth((XFontStruct*)rep->info, s, p - s);
}

int Font::Index (const char* s, int len, int offset, boolean between) {
    register XFontStruct* i = (XFontStruct*)rep->info;
    register const char* p;
    register int n, w;
    int coff, cw;

    if (offset < 0 || *s == '\0' || len == 0) {
	return 0;
    }
    if (IsFixedWidth(i)) {
	cw = i->min_bounds.width;
	n = offset / cw;
	coff = offset % cw;
    } else {
	w = 0;
	for (p = s, n = 0; *p != '\0' && n < len; ++p, ++n) {
	    cw = XTextWidth(i, p, 1);
	    w += cw;
	    if (w > offset) {
		break;
	    }
	}
	coff = offset - w + cw;
    }
    if (between && coff > cw/2) {
	++n;
    }
    return min(n, len);
}

boolean Font::FixedWidth () {
    register XFontStruct* i = (XFontStruct*)rep->info;
    return IsFixedWidth(i);
}

/*
 * class Pattern
 */

/*
 * Create a Pixmap for stippling from the given array of data.
 * The assumption is that the data is 16x16 and should be expanded to 32x32.
 */

static Pixmap MakeStipple (int* p) {
    int data[32];
    register int i, j;
    register unsigned int s1, s2;
    register unsigned int src, dst;

    for (i = 0; i < patternHeight; i++) {
	dst = 0;
	src = p[i];
	s1 = 1;
	s2 = 1 << (patternWidth - 1);
	for (j = 0; j < patternWidth; j++) {
	    if ((s1 & src) != 0) {
		dst |= s2;
	    }
	    s1 <<= 1;
	    s2 >>= 1;
	}
	dst = (dst << 16) | dst;
	data[i] = dst;
	data[i+16] = dst;
    }
    return XCreateBitmapFromData(
        _world->display(), _world->root(), (char*)data, 32, 32
    );
}

Pattern::Pattern (Bitmap* b) {
    int width = b->Width();
    int height = b->Height();
    info = (void*)XCreatePixmap(
        _world->display(), _world->root(), width, height, 1
    );
    GC gc = XCreateGC(_world->display(), (Pixmap)info, 0, nil);
    XCopyArea(
        _world->display(), (Pixmap)b->Map(), (Pixmap)info, gc,
        0, 0, width, height, 0, 0
    );
    XFreeGC(_world->display(), gc);
}

Pattern::Pattern (int p[patternHeight]) {
    info = (void*)MakeStipple(p);
}

Pattern::Pattern (int dither) {
    if (dither == 0xffff) {
        info = nil;
    } else {
        register int i, seed;
        int r[16];

        seed = dither;
        for (i = 0; i < 4; i++) {
            r[i] = (seed & 0xf000) >> 12;
            r[i] |= r[i] << 4;
            r[i] |= r[i] << 8;
            seed <<= 4;
            r[i+4] = r[i];
            r[i+8] = r[i];
            r[i+12] = r[i];
        }
        info = (void*)MakeStipple(r);
    }
}

Pattern::~Pattern () {
    if (info != nil) {
	XFreePixmap(_world->display(), (Pixmap)info);
    }
}

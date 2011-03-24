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
 * X11-dependent cursors
 */

#include <InterViews/bitmap.h>
#include <InterViews/color.h>
#include <InterViews/cursor.h>
#include <InterViews/font.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>

/*
 * Create the pixmap for a cursor.  These are always 16x16, unlike
 * fill patterns, which are 32x32.
 */

static Pixmap MakeCursorPixmap (int* scanline) {
#if vax
    /*
     * Cursor bitmaps work fine on the VAX; I suspect because
     * XPutImage doesn't have to do (misbehaving) swapping on a VAX.
     * The #else code fills the cursor pixmap explicitly because
     * cursor bitmaps don't seem to work right on other machines.
     */
    char data[2*cursorHeight];
    char* p;
    register int i, j;
    register unsigned int s1, s2;
    register unsigned int src, dst;
    union {
	char c[sizeof(short)];
	short s;
    } u;

    p = data;
    for (i = 0; i < cursorHeight; i++) {
	dst = 0;
	src = scanline[i];
	s1 = 1;
	s2 = 1 << (cursorWidth - 1);
	for (j = 0; j < cursorWidth; j++) {
	    if ((s1 & src) != 0) {
		dst |= s2;
	    }
	    s1 <<= 1;
	    s2 >>= 1;
	}
	u.s = dst;
	*p++ = u.c[0];
	*p++ = u.c[1];
    }
    return XCreateBitmapFromData(
	_world->display(), _world->root(), data, cursorWidth, cursorHeight
    );
#else
    /*
     * As best as I can tell, cursors created with a bitmap
     * don't work right.  The problem must be in the X11R2 library,
     * because the cursor has a slight glitch when running either
     * local on the Sun or remote to a VAX X server.
     *
     * I don't have the stomach to try to track down the problem,
     * so the simple solution is to draw into an off-screen pixmap.
     */

    Pixmap dst;
    GC g;
    register int i, j;
    register unsigned s1, s2;

    dst = XCreatePixmap(
        _world->display(), _world->root(), cursorWidth, cursorHeight, 1
    );
    g = XCreateGC(_world->display(), dst, 0, nil);
    XSetForeground(_world->display(), g, 0);
    XSetFillStyle(_world->display(), g, FillSolid);
    XFillRectangle(
        _world->display(), dst, g, 0, 0, cursorWidth, cursorHeight
    );
    XSetForeground(_world->display(), g, 1);
    for (i = 0; i < cursorHeight; i++) {
	s1 = scanline[i];
	s2 = 1;
	for (j = 0; j < cursorWidth; j++) {
	    if ((s1 & s2) != 0) {
		XDrawPoint(_world->display(), dst, g, cursorWidth - 1 - j, i);
	    }
	    s2 <<= 1;
	}
    }
    XFreeGC(_world->display(), g);
    return dst;
#endif
}

/*
 * Convert a Color object to the X representation.
 */

static void MakeColor (Color* c, XColor& xc) {
    int r, g, b;

    xc.pixel = c->PixelValue();
    c->Intensities(r, g, b);
    xc.red = r;
    xc.green = g;
    xc.blue = b;
}

Cursor::Cursor (Bitmap* pat, Bitmap* mask, Color* fg, Color* bg) {
    XColor f, b;

    MakeColor(fg, f);
    MakeColor(bg, b);
    id = (void*)XCreatePixmapCursor(
        _world->display(), (Pixmap)pat->Map(), (Pixmap)mask->Map(),
        &f, &b, -pat->Left(), pat->Height()-1 - (-pat->Bottom())
    );
}

Cursor::Cursor (Font* font, int pat, int mask, Color* fg, Color* bg) {
    XColor f, b;

    MakeColor(fg, f);
    MakeColor(bg, b);
    XFontStruct* i = (XFontStruct*)font->Info();
    id = (void*)XCreateGlyphCursor(
        _world->display(), i->fid, i->fid, pat, mask, &f, &b
    );
}

Cursor::Cursor (int n, Color* fg, Color* bg) {
    XColor f, b;

    MakeColor(fg, f);
    MakeColor(bg, b);
    id = (void*)XCreateFontCursor(_world->display(), n);
    XRecolorCursor(_world->display(), (XCursor)id, &f, &b);
}

void* Cursor::Id () {
    if (id == nil && pat != nil && mask != nil) {
	Pixmap p, m;
	XColor f, b;

	p = MakeCursorPixmap(pat);
	m = MakeCursorPixmap(mask);
	MakeColor(foreground, f);
	MakeColor(background, b);
	id = (void*)XCreatePixmapCursor(
	    _world->display(), p, m, &f, &b, x, cursorHeight - 1 - y
	);
	XFreePixmap(_world->display(), p);
	XFreePixmap(_world->display(), m);
    }
    return id;
}

Cursor::~Cursor () {
    if (id != nil) {
	XFreeCursor(_world->display(), (XCursor)id);
    }
}

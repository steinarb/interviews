/*
 * Copyright (c) 1987, 1988, 1989, 1990, 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * Input cursors.
 */

#include <InterViews/bitmap.h>
#include <InterViews/color.h>
#include <InterViews/cursor.h>
#include <InterViews/display.h>
#include <InterViews/font.h>
#include <InterViews/style.h>
#include <InterViews/session.h>
#include <IV-X11/Xlib.h>
#include <IV-X11/xbitmap.h>
#include <IV-X11/xcolor.h>
#include <IV-X11/xcursor.h>
#include <IV-X11/xdisplay.h>
#include <IV-X11/xfont.h>
#include <OS/string.h>
#include <X11/cursorfont.h>

static const CursorPattern textPat = {
    0x0000, 0x4400, 0x2800, 0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 
    0x1000, 0x1000, 0x1000, 0x1000, 0x1000, 0x2800, 0x4400, 0x0000
};

static const CursorPattern textMask = {
    0x0000, 0xCC00, 0x7800, 0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 
    0x3000, 0x3000, 0x3000, 0x3000, 0x3000, 0x7800, 0xCC00, 0x0000, 
};

static const CursorPattern noPat = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

Cursor* defaultCursor;
Cursor* arrow;
Cursor* crosshairs;
Cursor* ltextCursor;
Cursor* rtextCursor;
Cursor* hourglass;
Cursor* upperleft;
Cursor* upperright;
Cursor* lowerleft;
Cursor* lowerright;
Cursor* noCursor;

/*
 * Define the builtin cursors.
 */

void Cursor::init() {
    arrow = new Cursor(XC_left_ptr);
    crosshairs = new Cursor(XC_crosshair);
    ltextCursor = new Cursor(4, 8, textPat, textMask);
    rtextCursor = new Cursor(0, 8, textPat, textMask);
    hourglass = new Cursor(XC_watch);
    upperleft = new Cursor(XC_ul_angle);
    upperright = new Cursor(XC_ur_angle);
    lowerleft = new Cursor(XC_ll_angle);
    lowerright = new Cursor(XC_lr_angle);
    noCursor = new Cursor(0, 0, noPat, noPat);
    defaultCursor = arrow;
}

/*
 * Create a cursor a specific pattern and mask.
 */

Cursor::Cursor(
    short xoff, short yoff, const int* p, const int* m,
    const Color* fg, const Color * bg
) {
    CursorRep* c = new CursorRep;
    rep_ = c;
    c->xcursor_ = 0;
    c->x_ = xoff;
    c->y_ = yoff;
    c->pat_ = p;
    c->mask_ = m;
    c->fg_ = fg;
    c->bg_ = bg;
    Resource::ref(fg);
    Resource::ref(bg);
}

/*
 * Create a cursor from bitmaps.
 */

Cursor::Cursor(
    const Bitmap* pat, const Bitmap* mask, const Color* fg, const Color* bg
) {
    CursorRep* c = new CursorRep;
    rep_ = c;
    BitmapRep* b = pat->rep();
    Display* d = b->display_;
    c->make_colors(d);
    c->xcursor_ = XCreatePixmapCursor(
	d->rep()->display_,
	b->pixmap_, mask->rep()->pixmap_,
	&fg->rep(d)->xcolor_, &bg->rep(d)->xcolor_,
	d->to_pixels(-b->left_), d->to_pixels(b->height_ - 1 + b->bottom_)
    );
    Resource::ref(fg);
    Resource::ref(bg);
}

/*
 * Create a cursor from a font.
 */

Cursor::Cursor(
    const Font* font, int pat, int mask, const Color* fg, const Color* bg
) {
    CursorRep* c = new CursorRep;
    rep_ = c;
    Display* d = Session::instance()->default_display();
    XFontStruct* i = font->rep(d)->font_;
    Resource::ref(fg);
    Resource::ref(bg);
    c->fg_ = fg;
    c->bg_ = bg;
    c->make_colors(d);
    c->xcursor_ = XCreateGlyphCursor(
        d->rep()->display_, i->fid, i->fid, pat, mask,
	&fg->rep(d)->xcolor_, &bg->rep(d)->xcolor_
    );
}

/*
 * Create a cursor from the predefined cursor font.
 */

Cursor::Cursor(int n, const Color* fg, const Color* bg) {
    CursorRep* c = new CursorRep;
    rep_ = c;
    c->xcursor_ = 0;
    c->x_ = (short)n;
    c->pat_ = nil;
    c->mask_ = nil;
    c->fg_ = fg;
    c->bg_ = bg;
    Resource::ref(fg);
    Resource::ref(bg);
}

Cursor::~Cursor() {
    CursorRep* c = rep_;
    XCursor xc = c->xcursor_;
    if (xc != 0) {
	XFreeCursor(c->display_->rep()->display_, xc);
    }
    Resource::unref(c->fg_);
    Resource::unref(c->bg_);
    delete c;
}

/*
 * Create the pixmap for a cursor.  These are always 16x16, unlike
 * fill patterns, which are 32x32.
 */

static Pixmap MakeCursorPixmap(
    XDisplay* dpy, XWindow root, const int* scanline
) {
    Pixmap dst = XCreatePixmap(dpy, root, cursorWidth, cursorHeight, 1);
    GC g = XCreateGC(dpy, dst, 0, nil);
    XSetForeground(dpy, g, 0);
    XSetFillStyle(dpy, g, FillSolid);
    XFillRectangle(dpy, dst, g, 0, 0, cursorWidth, cursorHeight);
    XSetForeground(dpy, g, 1);

    register int i, j;
    register unsigned s1, s2;
    for (i = 0; i < cursorHeight; i++) {
	s1 = scanline[i];
	s2 = 1;
	for (j = 0; j < cursorWidth; j++) {
	    if ((s1 & s2) != 0) {
		XDrawPoint(dpy, dst, g, cursorWidth - 1 - j, i);
	    }
	    s2 <<= 1;
	}
    }
    XFreeGC(dpy, g);
    return dst;
}

XCursor CursorRep::xid(Display* d) const {
    if (xcursor_ == 0) {
	CursorRep* c = (CursorRep*)this;
	c->make_colors(d);
	c->make_xcursor(d);
	c->display_ = d;
    }
    return xcursor_;
}

void CursorRep::make_colors(Display* d) {
    String v;
    if (fg_ == nil) {
	Style* s = d->style();
	if (s->find_attribute("pointerColor", v)) {
	    fg_ = Color::lookup(d, v);
	}
	if (fg_ == nil) {
	    fg_ = s->foreground();
	}
	Resource::ref(fg_);
    }
    if (bg_ == nil) {
	Style* s = d->style();
	if (s->find_attribute("pointerColorBackground", v)) {
	    bg_ = Color::lookup(d, v);
	}
	if (bg_ == nil) {
	    bg_ = s->background();
	}
	Resource::ref(bg_);
    }
}

void CursorRep::make_xcursor(Display* d) {
    XDisplay* dpy = d->rep()->display_;
    if (pat_ != nil && mask_ != nil) {
	XWindow root = d->rep()->root_;
	Pixmap p = MakeCursorPixmap(dpy, root, pat_);
	Pixmap m = MakeCursorPixmap(dpy, root, mask_);
	xcursor_ = XCreatePixmapCursor(
	    dpy, p, m,
	    &fg_->rep(d)->xcolor_, &bg_->rep(d)->xcolor_,
	    x_, cursorHeight - 1 - y_
	);
	XFreePixmap(dpy, p);
	XFreePixmap(dpy, m);
    } else {
	/* x contains cursorfont index */
	xcursor_ = XCreateFontCursor(dpy, x_);
	XRecolorCursor(
	    dpy, xcursor_, &fg_->rep(d)->xcolor_, &bg_->rep(d)->xcolor_
	);
    }
}

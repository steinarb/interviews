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
 * X11-dependent raster code
 */

#include <InterViews/canvas.h>
#include <InterViews/raster.h>
#include <InterViews/X11/worldrep.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>

static int RasterImageSize(int w, int h) {
    return w * h * sizeof(unsigned long);
}

RasterRep::RasterRep(int w, int h) {
    width = w;
    height = h;
    data = nil;
}

RasterRep::RasterRep(Canvas* c, int x0, int y0, int w, int h) {
    width = w;
    height = h;
    data = XGetImage(
        _world->display(), (Drawable)c->Id(),
        x0, c->Height()-1-y0-height, width, height, AllPlanes, ZPixmap
    );
}

RasterRep::RasterRep(RasterRep* r) {
    width = r->width;
    height = r->height;
    if (r->data != nil) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                PutPixel(x, y, r->GetPixel(x, y));
            }
        }
    } else {
        data = nil;
    }
}

RasterRep::~RasterRep() {
    if (data != nil) {
        XDestroyImage((XImage*)data);
    }
}

void* RasterRep::GetData() {
    if (data == nil) {
        data = XCreateImage(
            _world->display(), _world->visual(),
            DefaultDepth(_world->display(), _world->screen()),
            ZPixmap, 0, new char[RasterImageSize(width, height)],
            width, height, sizeof(unsigned long) * 8, 0
        );
    }
    return data;
}

void RasterRep::PutPixel(int x, int y, int pixel) {
    XImage* image = (XImage*)GetData();
    XPutPixel(image, x, height - y - 1, pixel);
}

int RasterRep::GetPixel(int x, int y) {
    XImage* image = (XImage*)GetData();
    return XGetPixel(image, x, height - y - 1);
}

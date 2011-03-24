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

#ifndef Xutil_h
#define Xutil_h

#include <InterViews/X11/Xlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define Colormap XColormap

#include <X11/Xutil.h>

#undef Colormap

#undef XDestroyImage
#undef XGetPixel
#undef XPutPixel
#undef XSubImage
#undef XAddPixel

inline int XDestroyImage (XImage* i) {
    typedef int (*PF)(XImage*);
    return (*((PF)i->f.destroy_image))(i);
}

inline unsigned long XGetPixel (XImage* i, int x, int y) {
    typedef unsigned long (*PF)(XImage*, int, int);
    return (*((PF)i->f.get_pixel))(i, x, y);
}

inline int XPutPixel (XImage* i, int x, int y, unsigned long pixel) {
    typedef int (*PF)(XImage*, int, int, unsigned long);
    return (*((PF)i->f.put_pixel))(i, x, y, pixel);
}

inline XImage* XSubImage (
    XImage* i, int x, int y, unsigned int width, unsigned int height
) {
    typedef XImage* (*PF)(XImage*, int, int, unsigned int, unsigned int);
    return (*((PF)i->f.sub_image))(i, x, y, width, height);
}

inline int XAddPixel (XImage* i, long value) {
    typedef int (*PF)(XImage*, long);
    return (*((PF)i->f.add_pixel))(i, value);
}

#if defined(__cplusplus)
}
#endif

#endif

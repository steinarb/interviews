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
 * X11-dependent painter interface.
 */

#ifndef painterrep_h
#define painterrep_h

#include <InterViews/X11/Xlib.h>

class PainterRep {
public:
    PainterRep();
    ~PainterRep();

    void PrepareFill(void* info);
    void PrepareDash(int width, void* info, int count);

    GC fillgc;
    GC dashgc;
    boolean fillbg;
    boolean overwrite;
    boolean xor;
};

void DrawTransformedImage(
    XImage* src, int sx0, int sy0, XImage* mask, int mx0, int my0,
    Drawable d, unsigned int height, int dx0, int dy0,
    boolean stencil, unsigned long fg, unsigned long bg,
    GC gc, class Transformer* matrix
);

#endif

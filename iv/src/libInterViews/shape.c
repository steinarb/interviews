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
 * Shape constructors
 */

#include <InterViews/shape.h>

Shape::Shape () {
    SetUndefined();
    Rigid(hfil, hfil, vfil, vfil);
    aspect = 0;
    hunits = 1;
    vunits = 1;
}

Shape::~Shape () {
    /* nothing to do for now */
}

void Shape::Square (int side) {
    width = side; height = side;
    Rigid();
    aspect = 1;
}

void Shape::Rect (int w, int h) {
    width = w; height = h;
    Rigid();
}

void Shape::Rigid (int hshr, int hstr, int vshr, int vstr) {
    hshrink = hshr;
    hstretch = hstr;
    vshrink = vshr;
    vstretch = vstr;
}

void Shape::SetUndefined () {
    width = 0;
    height = 0;
}

boolean Shape::Defined () {
    return width != 0;
}

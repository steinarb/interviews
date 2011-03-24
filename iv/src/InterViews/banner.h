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
 * A banner is a one line title bar.
 */

#ifndef banner_h
#define banner_h

#include <InterViews/interactor.h>

class Banner : public Interactor {
public:
    char* left;
    char* middle;
    char* right;
    boolean highlight;

    Banner(char* lt, char* mid, char* rt);
    Banner(const char*, char* lt, char* mid, char* rt);
    Banner(Painter* out, char* lt, char* mid = nil, char* rt = nil);
    ~Banner();

    void Update();
protected:
    int lw, mw, rw;
    Coord lx, mx, rx;
    Painter* inverse;

    void Init(char*, char*, char*);
    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();
};

#endif

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
 * A perspective describes what portion of the total view
 * an interactor shows.
 */

#ifndef perspective_h
#define perspective_h

#include <InterViews/resource.h>

class Perspective : public Resource {
public:
    Coord x0, y0;		/* origin of view */
    int width, height;		/* total size of view */
    Coord curx, cury;		/* current position */
    int curwidth, curheight;	/* current size */
    int sx, sy, lx, ly;		/* small and large scrolling increments */

    Perspective();
    Perspective(Perspective&);
    ~Perspective();

    void Init(Coord ix0, Coord iy0, int iwidth, int iheight);
    void Attach(class Interactor*);
    void Detach(Interactor*);
    void Update();

    boolean operator ==(Perspective&);
    boolean operator !=(Perspective&);
    Perspective& operator =(Perspective&);
protected:
    class ViewList* views;	/* interactors that access the perspective */
};

#endif

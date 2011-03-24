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
 * Implementation of GrowingVertices and derived classes.
 */

#include <InterViews/painter.h>
#include <InterViews/rubverts.h>

#include <bstring.h>

GrowingVertices::GrowingVertices (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n, 
    Coord offx, Coord offy
) : (p, c, offx, offy) {
    origbufsize = count = n;
    bufsize = max(2*n, 50);
    x = new Coord[bufsize];
    y = new Coord[bufsize];
    bcopy(px, x, n*sizeof(Coord));
    bcopy(py, y, n*sizeof(Coord));
}

GrowingVertices::~GrowingVertices () {
    delete x;
    delete y;
}

void GrowingVertices::GetOriginal (Coord*& px, Coord*& py, int& n) {
    Copy(x, y, origbufsize, px, py);
    n = origbufsize;
}

void GrowingVertices::GetCurrent (Coord*& px, Coord*& py, int& n) {
    Copy(x, y, count, px, py);
    n = count;
}

void GrowingVertices::CheckBufs () {
    if (count + 1 > bufsize) {
        bufsize *= 2;

        Coord* nx = new Coord[bufsize];
        Coord* ny = new Coord[bufsize];

        bcopy(x, nx, count*sizeof(Coord));
        bcopy(y, ny, count*sizeof(Coord));
        delete x;
        delete y;
        x = nx;
        y = ny;
    }
}

void GrowingVertices::Copy (Coord* x, Coord* y, int n, Coord*& nx,Coord*& ny) {
    nx = new Coord[n];
    ny = new Coord[n];
    bcopy(x, nx, n*sizeof(Coord));
    bcopy(y, ny, n*sizeof(Coord));
}

void GrowingVertices::Draw () {
    if (!drawn) {
        x[count] = trackx;
        y[count] = tracky;
        DrawVertices(x, y, count + 1);
        drawn = true;
    }
}

void GrowingVertices::DrawVertices (Coord*, Coord*, int) { }

void GrowingVertices::AppendVertex (Coord vx, Coord vy) {
    boolean wasDrawn = drawn;

    Erase();
    x[count] = vx;
    y[count] = vy;
    ++count;
    CheckBufs();
    if (wasDrawn) {
        Draw();
    }
}

/*****************************************************************************/

GrowingMultiLine::GrowingMultiLine(
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) { }

void GrowingMultiLine::DrawVertices (Coord* x, Coord* y, int n) {
    output->MultiLine(canvas, x, y, n);
}

/*****************************************************************************/

GrowingPolygon::GrowingPolygon (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) { }

void GrowingPolygon::DrawVertices (Coord* x, Coord* y, int n) {
    if (n == 2) {
        output->Line(canvas, x[0], y[0], x[1], y[1]);
    } else {
        output->Polygon(canvas, x, y, n);
    }
};

/*****************************************************************************/

GrowingBSpline::GrowingBSpline (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) { }

void GrowingBSpline::DrawVertices (Coord* x, Coord* y, int n) {
    output->BSpline(canvas, x, y, n);
};

/*****************************************************************************/

GrowingClosedBSpline::GrowingClosedBSpline (
    Painter* p, Canvas* c, Coord px[], Coord py[], int n,
    Coord offx, Coord offy
) : (p, c, px, py, n, offx, offy) { }

void GrowingClosedBSpline::DrawVertices (Coord* x, Coord* y, int n) {
    output->ClosedBSpline(canvas, x, y, n);
};

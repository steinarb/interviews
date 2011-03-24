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
 * Implementation of squares view.
 */

#include "squares.h"
#include "view.h"
#include <InterViews/painter.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>

SquaresView::SquaresView (Squares* s) {
    subject = s;
    subject->Attach(this);
    shape->width = 200;
    shape->height = 200;
    shape->hunits = 20;
    shape->vunits = 20;
}

SquaresView::~SquaresView () {
    subject->Detach(this);
}

void SquaresView::Redraw (Coord x1, Coord y1, Coord x2, Coord y2) {
    register SquareData* s;
    Coord x, y;
    int w, h;

    output->ClearRect(canvas, x1, y1, x2, y2);
    for (s = subject->Head(); s != nil; s = s->next) {
        w = round(xmax * s->size) / 2;
        h = round(ymax * s->size) / 2;
        x = round(xmax * s->cx);
        y = round(ymax * s->cy);
        if (x1 <= x + w && x2 >= x - w && y1 <= y + h && y2 >= y - h) {
            output->Rect(canvas, x - w, y - h, x + w, y + h);
        }
    }
}

void SquaresView::Update () {
    Draw();
}

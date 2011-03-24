/*
 * Copyright (c) 1991 Stanford University
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
 * PageBorder - a fancy Border
 */

#include "PageBorder.h"

#include <InterViews/canvas.h>
#include <InterViews/color.h>

PageBorder::PageBorder(Glyph* body, const Color* c) : MonoGlyph(body) {
    color_ = c;
    Resource::ref(color_);
}

PageBorder::~PageBorder() {
    Resource::unref(color_);
}

void PageBorder::draw(Canvas* c, const Allocation& a) const {
    MonoGlyph::draw(c, a);
    if (c != nil) {
        Coord left = a.left();
        Coord bottom = a.bottom();
        Coord right = a.right();
        Coord top = a.top();
        if (c->damaged(left, bottom + 5, left + 1, top)) {
            c->fill_rect(left + 1, bottom + 5, left, top, color_);
        }
        if (c->damaged(left + 1, top - 1, right - 4, top)) {
            c->fill_rect(left + 1, top - 1, right - 4, top, color_);
        }
        if (c->damaged(right - 5, bottom, right, top - 1)) {
            c->fill_rect(right - 5, top - 1, right - 4, bottom + 4, color_);
            c->fill_rect(right - 4, top - 3, right - 2, top - 2, color_);
            c->fill_rect(right - 3, top - 3, right - 2, bottom + 2, color_);
            c->fill_rect(right - 2, top - 5, right, top - 4, color_);
            c->fill_rect(right - 1, top - 5, right, bottom, color_);
        }
        if (c->damaged(left, bottom, right - 1, bottom + 5)) {
            c->fill_rect(right - 5, bottom + 5, left, bottom + 4, color_);
            c->fill_rect(right - 3, bottom + 3, left + 2, bottom + 2, color_);
            c->fill_rect(left + 3, bottom + 3, left + 2, bottom + 4, color_);
            c->fill_rect(right - 1, bottom + 1, left + 4, bottom, color_);
            c->fill_rect(left + 5, bottom + 1, left + 4, bottom + 2, color_);
        }
    }
}

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

#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/font.h>
#include <InterViews/hit.h>
#include <InterViews/space.h>

Space::Space(int count, float each, const Font* f, const Color* c) : Glyph() {
    count_ = count;
    each_ = each;
    font_ = f;
    Resource::ref(font_);
    color_ = c;
    Resource::ref(color_);
    if (font_ != nil) {
	Coord ascent = font_->ascent();
	Coord descent = font_->descent();
	width_ = font_->width(' ') * each_ * count_;
	height_ = ascent + descent;
	alignment_ = (height_ == 0) ? 0 : descent / height_;
    } else {
	width_ = 0;
	height_ = 0;
	alignment_ = 0;
    }
}

Space::~Space() {
    Resource::unref(font_);
    Resource::unref(color_);
}

void Space::request(Requisition& requisition) const {
    Requirement rx(width_, width_*4, width_/3, 0);
    Requirement ry(height_, 0, 0, alignment_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void Space::allocate(Canvas*, const Allocation& a, Extension& ext) {
    ext.xy_extents(a);
}

void Space::pick(Canvas*, const Allocation& a, int depth, Hit& h) {
    Coord x = h.left();
    Coord left = a.left();
    Coord right = a.right();
    if (x >= left && x < right) {
        h.target(depth, this, (x > (left+right)/2) ? 1 : 0);
    }
}

void Space::draw(Canvas* c, const Allocation& a) const {
    if (count_ > 0) {
        Coord x = a.x();
        Coord y = a.y();
        Coord each = (a.right() - a.left()) / count_;
        for (int i = 0; i < count_; ++i) {
            c->character(font_, ' ', each, color_, x, y);
            x += each;
        }
    }
}

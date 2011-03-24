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

/*
 * Strut - filler glyph
 */

#include <InterViews/font.h>
#include <InterViews/strut.h>

Strut::Strut(
    const Font* font, Coord natural, Coord stretch, Coord shrink
) : Glyph() {
    font_ = font;
    Resource::ref(font_);
    if (font_ != nil) {
	Coord ascent = font_->ascent();
	Coord descent = font_->descent();
	height_ = ascent + descent;
	alignment_ = (height_ == 0) ? 0 : descent / height_;
    }
    natural_ = natural;
    stretch_ = stretch;
    shrink_ = shrink;
}

Strut::~Strut() {
    Resource::unref(font_);
}

void Strut::request(Requisition& requisition) const {
    Requirement rx(natural_, stretch_, shrink_, 0);
    Requirement ry(height_, 0, 0, alignment_);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void Strut::allocate(Canvas*, const Allocation& a, Extension& ext) {
    ext.xy_extents(a);
}

HStrut::HStrut(
    Coord right_bearing, Coord left_bearing,
    Coord natural, Coord stretch, Coord shrink
) : Glyph() {
    left_bearing_ = left_bearing;
    right_bearing_ = right_bearing;
    natural_ = natural;
    stretch_ = stretch;
    shrink_ = shrink;
}

HStrut::~HStrut() { }

void HStrut::request(Requisition& requisition) const {
    Coord width = left_bearing_ + right_bearing_;
    Requirement rx(width, 0, 0, (width == 0) ? 0 : left_bearing_ / width);
    Requirement ry(natural_, stretch_, shrink_, 0);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

VStrut::VStrut(
    Coord ascent, Coord descent, Coord natural, Coord stretch, Coord shrink
) : Glyph() {
    ascent_ = ascent;
    descent_ = descent;
    natural_ = natural;
    stretch_ = stretch;
    shrink_ = shrink;
}

VStrut::~VStrut() { }

void VStrut::request(Requisition& requisition) const {
    Coord height = ascent_ + descent_;
    Requirement rx(natural_, stretch_, shrink_, 0);
    Requirement ry(height, 0, 0, (height == 0) ? 0 : descent_ / height);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

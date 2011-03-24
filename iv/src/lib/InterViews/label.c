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
 * Label - ASCII text glyph with font
 */

#include <InterViews/label.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/font.h>
#include <InterViews/hit.h>
#include <OS/string.h>

Label::Label(const String& s, const Font* f, const Color* c) : Glyph() {
    text_ = new CopyString(s);
    font_ = f;
    Resource::ref(font_);
    color_ = c;
    Resource::ref(color_);
}

Label::Label(const char* text, const Font* f, const Color* c) : Glyph() {
    text_ = new CopyString(text);
    font_ = f;
    Resource::ref(font_);
    color_ = c;
    Resource::ref(color_);
}

Label::Label(
    const char* text, int length, const Font* f, const Color* c
) : Glyph() {
    text_ = new CopyString(text, length);
    font_ = f;
    Resource::ref(font_);
    color_ = c;
    Resource::ref(color_);
}

Label::~Label() {
    delete text_;
    Resource::unref(font_);
    Resource::unref(color_);
}

void Label::request(Requisition& requisition) const {
    const Font* f = font_;
    Coord width = f->width(text_->string(), text_->length());
    Coord ascent = f->ascent();
    Coord descent = f->descent();
    Coord height = ascent + descent;
    float alignment = (height == 0) ? 0 : descent / height;
    Requirement rx(width, 0, 0, 0);
    Requirement ry(height, 0, 0, alignment);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void Label::allocate(Canvas*, const Allocation& a, Extension& ext) {
    const Font* f = font_;
    const char* str = text_->string();
    int len = text_->length();
    Coord x = a.x();
    Coord y = a.y();
    ext.xy_extents(
	x - f->left_bearing(str, len), x + f->right_bearing(str, len),
	y - f->descent(str, len), y + f->ascent(str, len)
    );
}

void Label::draw(Canvas* canvas, const Allocation& a) const {
    if (canvas != nil) {
        Coord x = a.x();
        Coord y = a.y();
        const Font* f = font_;
        const Color* c = color_;
	const char* p = text_->string();
        const char* q = &p[text_->length()];
        for (; p < q; p++) {
            Coord width = f->width(*p);
            canvas->character(f, *p, width, c, x, y);
            x += width;
        }
    }
}

void Label::pick(Canvas*, const Allocation& a, int depth, Hit& h) {
    Coord x = h.left();
    Coord y = h.bottom();
    if (x >= a.left() && x < a.right() && y >= a.bottom() && y < a.top()) {
        int index = font_->index(
	    text_->string(), text_->length(), x - a.x(), true
	);
        h.target(depth, this, index);
    }
}

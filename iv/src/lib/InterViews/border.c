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
 * Border - a glyph for framing
 */

#include <InterViews/border.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/printer.h>

Border::Border(Glyph* body, const Color* c) : MonoGlyph(body) {
    color_ = c;
    Resource::ref(color_);
    thickness_ = 1;
}

Border::Border(Glyph* body, const Color* c, Coord t) : MonoGlyph(body) {
    color_ = c;
    Resource::ref(color_);
    thickness_ = t;
}

Border::~Border() {
    Resource::unref(color_);
}

void Border::allocate(Canvas* canvas, const Allocation& a, Extension& ext) {
    MonoGlyph::allocate(canvas, a, ext);
    ext.extend(a);
}

void Border::draw(Canvas* canvas, const Allocation& a) const {
    if (canvas != nil) {
        Coord left = a.left();
        Coord bottom = a.bottom();
        Coord right = a.right();
        Coord top = a.top();
        canvas->fill_rect(
            left, bottom + thickness_, left + thickness_, top, color_
        );
        canvas->fill_rect(
            left + thickness_, top - thickness_, right, top, color_
        );
        canvas->fill_rect(
            right - thickness_, top - thickness_, right, bottom, color_
        );
        canvas->fill_rect(
            right - thickness_, bottom + thickness_, left, bottom, color_
        );
    }
    MonoGlyph::draw(canvas, a);
}

void Border::print(Printer* p, const Allocation& a) const {
    if (p != nil) {
        Coord left = a.left();
        Coord bottom = a.bottom();
        Coord right = a.right();
        Coord top = a.top();
        p->fill_rect(
            left, bottom + thickness_, left + thickness_, top, color_
        );
        p->fill_rect(
            left + thickness_, top - thickness_, right, top, color_
        );
        p->fill_rect(
            right - thickness_, top - thickness_, right, bottom, color_
        );
        p->fill_rect(
            right - thickness_, bottom + thickness_, left, bottom, color_
        );
    }
    MonoGlyph::print(p, a);
}

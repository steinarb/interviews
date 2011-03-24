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
 * Patch - for repairing glyphs
 */

#include <InterViews/canvas.h>
#include <InterViews/patch.h>

Patch::Patch(Glyph* body) : MonoGlyph(body) {
    canvas_ = nil;
}

Patch::~Patch() {
    canvas_ = nil;
}

Canvas* Patch::canvas() const {
    return canvas_;
}

const Allocation& Patch::allocation() const {
    return allocation_;
}

void Patch::redraw() const {
    if (canvas_ != nil) {
        Coord l = extension_.left();
        Coord b = extension_.bottom();
        Coord r = extension_.right();
        Coord t = extension_.top();
        if (l < r && b < t) {
            canvas_->damage(l, b, r, t);
        }
    }
}

void Patch::reallocate() {
    Requisition s;
    request(s);
    extension_.xy_extents(fil, -fil, fil, -fil);
    allocate(canvas_, allocation_, extension_);
}

void Patch::repick(int depth, Hit& h) {
    pick(canvas_, allocation_, depth, h);
}

void Patch::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    MonoGlyph::allocate(c, a, ext);
    canvas_ = c;
    allocation_ = a;
    extension_ = ext;
}

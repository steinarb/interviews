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

#include <IV-look/telltale.h>
#include <InterViews/canvas.h>
#include <InterViews/world.h>

Telltale::Telltale(Glyph* body) : MonoGlyph(body) {
    highlighted_ = false;
    chosen_ = false;
    enabled_ = true;
    canvas_ = nil;
}

Telltale::~Telltale() {
    canvas_ = nil;
}

void Telltale::highlight(boolean highlighted) {
    if (highlighted != highlighted_) {
        highlighted_ = highlighted;
        redraw();
    }
}

boolean Telltale::highlighted() const { return highlighted_; }

void Telltale::choose(boolean chosen) {
    if (chosen != chosen_) {
        chosen_ = chosen;
        redraw();
    }
}

boolean Telltale::chosen() const { return chosen_; }

void Telltale::enable(boolean enabled) {
    if (enabled != enabled_) {
        enabled_ = enabled;
        redraw();
    }
}

boolean Telltale::enabled() const { return enabled_; }

void Telltale::redraw() const {
    if (canvas_ != nil) {
        canvas_->damage(
            extension_.left(), extension_.bottom(),
            extension_.right(), extension_.top()
        );
    }
}

void Telltale::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    MonoGlyph::allocate(c, a, ext);
    ext.extend(a);
    canvas_ = c;
    extension_ = ext;
}

void Telltale::draw(Canvas* c, const Allocation& a) const {
    if (c->damaged(a.left(), a.bottom(), a.right(), a.top())) {
	MonoGlyph::draw(c, a);
    }
}

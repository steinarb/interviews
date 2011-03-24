/*
 * Copyright (c) 1991 Stanford University
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
 * OpenLook style information
 */

#include <IV-look/ol_style.h>
#include <InterViews/color.h>
#include <InterViews/font.h>
#include <InterViews/style.h>
#include <OS/string.h>

class OL_StyleImpl {
private:
    friend class OL_Style;

    const Font* glyph_font_;
    Style* style_;
    Color* shadow_;
    Color* bright_;
    Color* fill_out_;
    Color* fill_in_;
    Color* dull_;
    Color* gray_out_;
    Color* gray_in_;

    Color* shade(const Color* c, float white, float back, float alpha);
};

Color* OL_StyleImpl::shade(
    const Color* c, float white, float back, float alpha
) {
    ColorIntensity red, green, blue;
    c->intensities(red, green, blue);
    return new Color(
	white + back*red, white + back*green, white + back*blue, alpha
    );
}

OL_Style::OL_Style(Style* style) {
    OL_StyleImpl* s = new OL_StyleImpl;
    impl_ = s;
    const Font* gf = nil;
    String v;
    if (style->find_attribute("ol_font", v)) {
	gf = Font::lookup(v);
    }
    if (gf == nil) {
	gf = Font::lookup("olglyph-12");
    }
    s->glyph_font_ = gf;
    Resource::ref(s->glyph_font_);
    s->style_ = style;
    Resource::ref(s->style_);
    const Color* bg = style->background();
    s->shadow_ = new Color(0.0, 0.0, 0.0, 0.5);
    Resource::ref(s->shadow_);
    s->bright_ = s->shade(bg, 0.8, 0.2, 1.0);
    Resource::ref(s->bright_);
    s->fill_out_ = s->shade(bg, 0.0, 1.0, 1.0);
    Resource::ref(s->fill_out_);
    s->fill_in_ = s->shade(bg, 0.0, 0.9, 1.0);
    Resource::ref(s->fill_in_);
    s->dull_ = s->shade(bg, 0.0, 0.5, 1.0);
    Resource::ref(s->dull_);
    s->gray_out_ = new Color(*s->fill_out_, 0.5);
    Resource::ref(s->gray_out_);
    s->gray_in_ = new Color(*s->fill_in_, 0.5);
    Resource::ref(s->gray_in_);
}

OL_Style::~OL_Style() {
    OL_StyleImpl* s = impl_;
    Resource::unref(s->glyph_font_);
    Resource::unref(s->style_);
    Resource::unref(s->shadow_);
    Resource::unref(s->gray_out_);
    Resource::unref(s->gray_in_);
    delete impl_;
}

const Color* OL_Style::foreground() const {
    return impl_->style_->foreground();
}

const Color* OL_Style::background() const {
    return impl_->style_->background();
}

const Color* OL_Style::shadow() const {
    return impl_->shadow_;
}

const Color* OL_Style::bright() const {
    return impl_->bright_;
}

const Color* OL_Style::dull() const {
    return impl_->dull_;
}

const Color* OL_Style::fill_out() const {
    return impl_->fill_out_;
}

const Color* OL_Style::fill_in() const {
    return impl_->fill_in_;
}

const Color* OL_Style::gray_out() const {
    return impl_->gray_out_;
}

const Color* OL_Style::gray_in() const {
    return impl_->gray_in_;
}

const Font* OL_Style::glyph_font() const {
    return impl_->glyph_font_;
}

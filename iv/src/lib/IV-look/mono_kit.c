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
 * MonoKit -- object for creating common UI components for monochrome display
 */

#include <IV-look/button.h>
#include <IV-look/choice.h>
#include <IV-look/menu.h>
#include <IV-look/mono_kit.h>
#include <IV-look/kitimpl.h>
#include <IV-look/panner.h>
#include <IV-look/scroller.h>
#include <InterViews/action.h>
#include <InterViews/background.h>
#include <InterViews/bitmap.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/canvas.h>
#include <InterViews/center.h>
#include <InterViews/color.h>
#include <InterViews/fixedspan.h>
#include <InterViews/label.h>
#include <InterViews/glue.h>
#include <InterViews/margin.h>
#include <InterViews/monoglyph.h>
#include <InterViews/rule.h>
#include <InterViews/session.h>
#include <InterViews/shadow.h>
#include <InterViews/stencil.h>
#include <InterViews/style.h>

#include <InterViews/Bitmaps/enlargeHit.bm>
#include <InterViews/Bitmaps/enlarge.bm>
#include <InterViews/Bitmaps/reducerHit.bm>
#include <InterViews/Bitmaps/reducer.bm>
#include <InterViews/Bitmaps/lmoverHit.bm>
#include <InterViews/Bitmaps/lmover.bm>
#include <InterViews/Bitmaps/rmoverHit.bm>
#include <InterViews/Bitmaps/rmover.bm>
#include <InterViews/Bitmaps/umoverHit.bm>
#include <InterViews/Bitmaps/umover.bm>
#include <InterViews/Bitmaps/dmoverHit.bm>
#include <InterViews/Bitmaps/dmover.bm>

#include <OS/string.h>

class MonoKitImpl {
private:
    friend class MonoKit;

    Color* highlight_;
    Color* shadow_;
    Bitmap* enlarger_hit_, * enlarger_plain_;
    Bitmap* reducer_hit_, * reducer_plain_;
    Bitmap* lmover_hit_, * lmover_plain_;
    Bitmap* rmover_hit_, * rmover_plain_;
    Bitmap* umover_hit_, * umover_plain_;
    Bitmap* dmover_hit_, * dmover_plain_;

    MonoKitImpl();
    ~MonoKitImpl();

    Bitmap* create_bitmap(void*, int width, int height);
};

class MonoKitInverter : public MonoGlyph {
public:
    MonoKitInverter(Glyph*, const Color*);
    virtual ~MonoKitInverter();

    virtual void draw(Canvas*, const Allocation&) const;
private:
    const Color* wash_;
};

MonoKit::MonoKit() {
    impl_ = new MonoKitImpl;
}

MonoKit::~MonoKit() {
    delete impl_;
}

Glyph* MonoKit::inset_frame(Glyph* g, Style* s) const {
    return new Background(
	new MonoKitInverter(g, impl_->highlight_),
	s->background()
    );
}

Glyph* MonoKit::outset_frame(Glyph* g, Style* s) const {
    return new Background(
	new MonoKitInverter(g, impl_->highlight_),
	s->background()
    );
}

Glyph* MonoKit::flat_frame(Glyph* g, Style* s) const {
    return new Background(g, s->background());
}

Glyph* MonoKit::label(const char* str, Style* s) const {
    return new Label(str, s->font(), s->foreground());
}

Glyph* MonoKit::label(const String& str, Style* s) const {
    return new Label(str, s->font(), s->foreground());
}

Menu* MonoKit::menubar(Style* s) const {
    return new Menu(
	new Background(
	    new RMargin(new LRBox, 0.0, fil, 0.0),
	    s->background()
	),
	0.0, 0.0, 0.0, 1.0
    );
}

Menu* MonoKit::pulldown(Style* s) const {
    return new Menu(
	new Margin(
	    new Shadow(
		new Background(new TBBox, s->background()),
		4, -4, impl_->shadow_
	    ),
	    0, 4, 4, 0
	),
	1.0, 1.0, 0.0, 1.0
    );
}

Menu* MonoKit::pullright(Style* s) const {
    return new Menu(
	new Background(
	    new Border(new Margin(new TBBox, 1.0), s->foreground(), 1.0),
	    s->background()
	),
	1.0, 1.0, 0.0, 1.0
    );
}

Telltale* MonoKit::menubar_item(Glyph* g, Style*) const {
    Glyph* label = new Margin(g, 3.0);
    return new ChoiceItem(
	label, new MonoKitInverter(label, impl_->highlight_)
    );
}

Telltale* MonoKit::menu_item(Glyph* g, Style*) const {
    Glyph* label = g;
    return new ChoiceItem(
	label, new MonoKitInverter(label, impl_->highlight_)
    );
}

Telltale* MonoKit::menu_item_separator(Style* s) const {
    return new Telltale(
	new HMargin(new HRule(s->foreground(), 2.0), 3.0)
    );
}

void MonoKit::popup(Menu*) const {
    /* unimplemented */
}

Button* MonoKit::push_button(Glyph* g, Style* s, Action* a) const {
    Glyph* label = new Border(
	new Margin(g, 4.0), s->foreground(), 1.0
    );
    return new Button(
	a,
	new ChoiceItem(label, new MonoKitInverter(label, impl_->highlight_))
    );
}

Button* MonoKit::toggle_button(Glyph* g, Style*, Action* a) const {
    Glyph* label = new Margin(g, 3.0);
    return new Button(
	a,
	new ChoiceItem(label, new MonoKitInverter(label, impl_->highlight_))
    );
}

Button* MonoKit::radio_button(Glyph* g, Style*, Action* a) const {
    Glyph* label = new Margin(g, 3.0);
    return new Button(
	a,
	new ChoiceItem(label, new MonoKitInverter(label, impl_->highlight_))
    );
}

Action* MonoKit::quit() const {
    return new ActionCallback(Session)(Session::instance(), &Session::quit);
}

Glyph* MonoKit::hscroll_bar(Adjustable* a, Style* style) const {
    Style* s = new Style(style);
    s->prefix("HScrollBar");
    s->prefix("ScrollBar");
    Coord mover_size = 15.0;
    s->find_attribute("mover_size", mover_size);
    Glyph* sep = new HGlue(1.0, 0.0, 0.0);
    return inset_frame(
	new LRBox(
	    new HCenter(
		new FixedSpan(left_mover(a, s), Dimension_X, mover_size),
		0.0
	    ),
	    sep,
	    new HScroller(a, s),
	    sep,
	    new FixedSpan(right_mover(a, s), Dimension_X, mover_size)
	),
	s
    );
}

Glyph* MonoKit::vscroll_bar(Adjustable* a, Style* style) const {
    Style* s = new Style(style);
    s->prefix("VScrollBar");
    s->prefix("ScrollBar");
    Coord mover_size = 15.0;
    s->find_attribute("mover_size", mover_size);
    Glyph* sep = new VGlue(1.0, 0.0, 0.0);
    return inset_frame(
	new TBBox(
	    new VCenter(
		new FixedSpan(up_mover(a, s), Dimension_Y, mover_size),
		1.0
	    ),
	    sep,
	    new VScroller(a, s),
	    sep,
	    new FixedSpan(down_mover(a, s), Dimension_Y, mover_size)
	),
	s
    );
}

Glyph* MonoKit::panner(Adjustable* x, Adjustable* y, Style* s) const {
    return new Panner(x, y, s);
}

Button* MonoKit::enlarger(Adjustable*, Style*) const {
    return nil;
}

Button* MonoKit::reducer(Adjustable*, Style*) const {
    return nil;
}

Button* MonoKit::up_mover(Adjustable* a, Style* s) const {
    const Color* c = s->foreground();
    return new Button(
	new MoverActionForward(a, Dimension_Y),
	new ChoiceItem(
	    new Stencil(impl_->umover_plain_, c),
	    new Stencil(impl_->umover_hit_, c)
	)
    );
}

Button* MonoKit::down_mover(Adjustable* a, Style* s) const {
    const Color* c = s->foreground();
    return new Button(
	new MoverActionBackward(a, Dimension_Y),
	new ChoiceItem(
	    new Stencil(impl_->dmover_plain_, c),
	    new Stencil(impl_->dmover_hit_, c)
	)
    );
}

Button* MonoKit::left_mover(Adjustable* a, Style* s) const {
    const Color* c = s->foreground();
    return new Button(
	new MoverActionForward(a, Dimension_X),
	new ChoiceItem(
	    new Stencil(impl_->lmover_plain_, c),
	    new Stencil(impl_->lmover_hit_, c)
	)
    );
}

Button* MonoKit::right_mover(Adjustable* a, Style* s) const {
    const Color* c = s->foreground();
    return new Button(
	new MoverActionBackward(a, Dimension_X),
	new ChoiceItem(
	    new Stencil(impl_->rmover_plain_, c),
	    new Stencil(impl_->rmover_hit_, c)
	)
    );
}

/** class MonoKitImpl **/

MonoKitImpl::MonoKitImpl() {
    highlight_ = new Color(0.0, 0.0, 0.0, 1.0, Color::Xor);
    Resource::ref(highlight_);
    shadow_ = new Color(0.0, 0.0, 0.0, 0.5);
    Resource::ref(shadow_);
    enlarger_hit_ = create_bitmap(
	enlarger_hit_bits, enlarger_hit_width, enlarger_hit_height
    );
    enlarger_plain_ = create_bitmap(
	enlarger_plain_bits, enlarger_plain_width, enlarger_plain_height
    );
    reducer_hit_ = create_bitmap(
	reducer_hit_bits, reducer_hit_width, reducer_hit_height
    );
    reducer_plain_ = create_bitmap(
	reducer_plain_bits, reducer_plain_width, reducer_plain_height
    );
    lmover_hit_ = create_bitmap(
	lmover_hit_bits, lmover_hit_width, lmover_hit_height
    );
    lmover_plain_ = create_bitmap(
	lmover_plain_bits, lmover_plain_width, lmover_plain_height
    );
    rmover_hit_ = create_bitmap(
	rmover_hit_bits, rmover_hit_width, rmover_hit_height
    );
    rmover_plain_ = create_bitmap(
	rmover_plain_bits, rmover_plain_width, rmover_plain_height
    );
    umover_hit_ = create_bitmap(
	umover_hit_bits, umover_hit_width, umover_hit_height
    );
    umover_plain_ = create_bitmap(
	umover_plain_bits, umover_plain_width, umover_plain_height
    );
    dmover_hit_ = create_bitmap(
	dmover_hit_bits, dmover_hit_width, dmover_hit_height
    );
    dmover_plain_ = create_bitmap(
	dmover_plain_bits, dmover_plain_width, dmover_plain_height
    );
}

MonoKitImpl::~MonoKitImpl() {
    Resource::unref(highlight_);
    Resource::unref(shadow_);
    Resource::unref(enlarger_hit_);
    Resource::unref(enlarger_plain_);
    Resource::unref(reducer_hit_);
    Resource::unref(reducer_plain_);
    Resource::unref(lmover_hit_);
    Resource::unref(lmover_plain_);
    Resource::unref(rmover_hit_);
    Resource::unref(rmover_plain_);
    Resource::unref(umover_hit_);
    Resource::unref(umover_plain_);
    Resource::unref(dmover_hit_);
    Resource::unref(dmover_plain_);
}

Bitmap* MonoKitImpl::create_bitmap(void* data, int width, int height) {
    Bitmap* b = new Bitmap(data, width, height);
    Resource::ref(b);
    return b;
}

/** class MonoKitInverter **/

MonoKitInverter::MonoKitInverter(Glyph* g, const Color* c) : MonoGlyph(g) {
    wash_ = c;
    Resource::ref(wash_);
}

MonoKitInverter::~MonoKitInverter() {
    Resource::unref(wash_);
}

void MonoKitInverter::draw(Canvas* c, const Allocation& a) const {
    MonoGlyph::draw(c, a);
    c->fill_rect(a.left(), a.bottom(), a.right(), a.top(), wash_);
}

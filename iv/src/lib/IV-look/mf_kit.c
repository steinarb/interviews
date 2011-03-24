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
 * MFKit -- object for creating common UI Motif-ish objects
 */

#include <IV-look/adjustable.h>
#include <IV-look/adjuster.h>
#include <IV-look/bevel.h>
#include <IV-look/button.h>
#include <IV-look/choice.h>
#include <IV-look/kitimpl.h>
#include <IV-look/menu.h>
#include <IV-look/mf_kit.h>
#include <IV-look/panner.h>
#include <IV-look/scroller.h>
#include <InterViews/action.h>
#include <InterViews/background.h>
#include <InterViews/bitmap.h>
#include <InterViews/box.h>
#include <InterViews/center.h>
#include <InterViews/color.h>
#include <InterViews/glue.h>
#include <InterViews/fixedspan.h>
#include <InterViews/label.h>
#include <InterViews/margin.h>
#include <InterViews/session.h>
#include <InterViews/stencil.h>
#include <InterViews/style.h>
#include <InterViews/Bitmaps/checkmark.bm>
#include <OS/string.h>

class MFKitImpl {
private:
    friend class MFKit;

    Bitmap* checkmark_;

    MFKitImpl();
    ~MFKitImpl();

    Coord frame_thickness(Style*);
    Glyph* make_inset(Glyph*, Beveler, Style*, Coord thickness);
    Glyph* make_inset(Glyph*, Beveler, Style*);
    Glyph* make_outset(Glyph*, Beveler, Style*, Coord thickness);
    Glyph* make_outset(Glyph*, Beveler, Style*);
    Glyph* make_flat(Glyph*, Beveler, Style*, Coord thickness);
    Glyph* make_flat(Glyph*, Beveler, Style*);
    Glyph* make_label(const char*, Style*);
    Glyph* make_label(const String&, Style*);
    Telltale* make_mover(Beveler, Style*);
};

MFKit::MFKit() {
    impl_ = new MFKitImpl;
}

MFKit::~MFKit() {
    delete impl_;
}

MFKitImpl::MFKitImpl() {
    checkmark_ = new Bitmap(
	checkmark_bits, checkmark_width, checkmark_height,
	checkmark_x_hot, checkmark_y_hot
    );
    checkmark_->ref();
}

MFKitImpl::~MFKitImpl() {
    Resource::unref(checkmark_);
}

Glyph* MFKit::inset_frame(Glyph* g, Style* s) const {
    return impl_->make_inset(g, &Bevel::rect, s);
}

Glyph* MFKit::outset_frame(Glyph* g, Style* s) const {
    return impl_->make_outset(g, &Bevel::rect, s);
}

Glyph* MFKit::flat_frame(Glyph* g, Style* s) const {
    return impl_->make_flat(g, &Bevel::rect, s);
}

Glyph* MFKit::label(const char* str, Style* s) const {
    return impl_->make_label(str, s);
}

Glyph* MFKit::label(const String& str, Style* s) const {
    return impl_->make_label(str, s);
}

Menu* MFKit::menubar(Style* s) const {
    return new Menu(
	outset_frame(
	    new RMargin(new LRBox, 0.0, fil, 0.0),
	    s
	),
	0.0, 0.0, 0.0, 1.0
    );
}

Telltale* MFKit::menubar_item(Glyph* g, Style* s) const {
    Glyph* i = new HMargin(g, 2.0);
    return new ChoiceItem(flat_frame(i, s), outset_frame(i, s));
}

Menu* MFKit::pulldown(Style* s) const {
    return new Menu(outset_frame(new TBBox, s), 1.0, 1.0, 0.0, 1.0);
}

Menu* MFKit::pullright(Style* s) const {
    return new Menu(outset_frame(new TBBox, s), 1.0, 1.0, 0.0, 1.0);
}

void MFKit::popup(Menu*) const {
    /* unimplemented */
}

Telltale* MFKit::menu_item(Glyph* g, Style* s) const {
    Bitmap* b = impl_->checkmark_;
    const Color* c = nil;
    String checkmark_color_name;
    if (s->find_attribute("checkmark_color", checkmark_color_name)) {
	c = Color::lookup(
	    Session::instance()->default_display(), checkmark_color_name
	);
    }
    if (c == nil) {
	c = new Color(0.8, 0.1, 0.1, 1.0);
    }
    Glyph* check = new Stencil(b, c);

    Coord margin = b->left_bearing() + b->right_bearing();
    Glyph* item = new HMargin(g, margin);
    Glyph* with_check = new Overlay(
	item,
	new RMargin(check, 0.0, fil, 0.0)
    );

    Requirement r(0.0, fil, 0.0, 0.0);
    Requisition req;
    req.require(Dimension_X, r);
    req.require(Dimension_Y, r);
    Glyph* grayout = new Background(
	new Glue(req), new Color(*s->flat(), 0.40)
    );
    Glyph* disabled = new Overlay(item, grayout);
    Glyph* disabled_with_check = new Overlay(with_check, grayout);

    Glyph* flat = flat_frame(g, s);
    Glyph* flat_with_check = flat_frame(with_check, s);
    Glyph* outset = outset_frame(g, s);
    Glyph* outset_with_check = outset_frame(with_check, s);

    ChoiceItem* choice = new ChoiceItem;
    for (unsigned int i = 0; i < choice->numlooks(); i++) {
	Glyph* i_look;
	if (choice->contains_looks(i, ChoiceItem::is_enabled)) {
	    if (choice->contains_looks(
		i, ChoiceItem::is_pressed + ChoiceItem::is_chosen)
	    ) {
		i_look = outset_with_check;
	    } else if (choice->contains_looks(i, ChoiceItem::is_pressed)) {
		i_look = outset;
	    } else if (choice->contains_looks(i, ChoiceItem::is_chosen)) {
		i_look = flat_with_check;
	    } else {
		i_look = flat;
	    }
	} else if (choice->contains_looks(i, ChoiceItem::is_chosen)) {
	    i_look = disabled_with_check;
	} else {
	    i_look = disabled;
	}
	choice->look(i, i_look);
    }
    return choice;
}

Telltale* MFKit::menu_item_separator(Style* s) const {
    return new Telltale(
	new VMargin(
	    new FixedSpan(
		new HMargin(inset_frame(nil, s), 3.0), Dimension_Y, 3.0
	    ),
	    2.0
	)
    );
}

Button* MFKit::push_button(Glyph* g, Style* s, Action* a) const {
    Coord t = impl_->frame_thickness(s);
    Coord margin = 3.0;
    Coord in_margin = margin + t;
    return new Button(
	a,
	new ChoiceItem(
	    impl_->make_outset(new Margin(g, margin), &Bevel::rect, s, t),
	    impl_->make_inset(new Margin(g, in_margin), &Bevel::rect, s, t)
	)
    );
}

Button* MFKit::toggle_button(Glyph* g, Style* s, Action* a) const {
    /* unimplemented, return menu_item */
    return new Button(a, menu_item(g, s));
}

Button* MFKit::radio_button(Glyph* g, Style* s, Action* a) const {
    /* unimplemented, return menu_item */
    return new Button(a, menu_item(g, s));
}

Action* MFKit::quit() const {
    return new ActionCallback(Session)(Session::instance(), &Session::quit);
}

Glyph* MFKit::hscroll_bar(Adjustable* a, Style* style) const {
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

Glyph* MFKit::vscroll_bar(Adjustable* a, Style* style) const {
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

Glyph* MFKit::panner(Adjustable* x, Adjustable* y, Style* s) const {
    return new Panner(x, y, s);
}

Button* MFKit::enlarger(Adjustable*, Style*) const {
    return nil;
}

Button* MFKit::reducer(Adjustable*, Style*) const {
    return nil;
}

Button* MFKit::up_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionForward(a, Dimension_Y),
	impl_->make_mover(&Bevel::up_arrow, s)
    );
}

Button* MFKit::down_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionBackward(a, Dimension_Y),
	impl_->make_mover(&Bevel::down_arrow, s)
    );
}

Button* MFKit::left_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionForward(a, Dimension_X),
	impl_->make_mover(&Bevel::left_arrow, s)
    );
}

Button* MFKit::right_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionBackward(a, Dimension_X),
	impl_->make_mover(&Bevel::right_arrow, s)
    );
}

/** class MFKitImpl **/

Coord MFKitImpl::frame_thickness(Style* s) {
    Coord c;
    if (!s->find_attribute("frameThickness", c)) {
	c = 2.0;
    }
    return c;
}

Glyph* MFKitImpl::make_inset(Glyph* g, Beveler b, Style* s, Coord t) {
    return new Bevel(g, b, s->dark(), s->dull(), s->light(), t);
}

Glyph* MFKitImpl::make_inset(Glyph* g, Beveler b, Style* s) {
    return make_inset(g, b, s, frame_thickness(s));
}

Glyph* MFKitImpl::make_outset(Glyph* g, Beveler b, Style* s, Coord t) {
    return new Bevel(g, b, s->light(), s->flat(), s->dark(), t);
}

Glyph* MFKitImpl::make_outset(Glyph* g, Beveler b, Style* s) {
    return make_outset(g, b, s, frame_thickness(s));
}

Glyph* MFKitImpl::make_flat(Glyph* g, Beveler b, Style* s, Coord t) {
    return new Bevel(g, b, s->flat(), s->flat(), s->flat(), t);
}

Glyph* MFKitImpl::make_flat(Glyph* g, Beveler b, Style* s) {
    return make_flat(g, b, s, frame_thickness(s));
}

Glyph* MFKitImpl::make_label(const char* label, Style* s) {
    return new Label(label, s->font(), s->foreground());
}

Glyph* MFKitImpl::make_label(const String& label, Style* s) {
    return new Label(label, s->font(), s->foreground());
}

Telltale* MFKitImpl::make_mover(Beveler b, Style* s) {
    return new ChoiceItem(
	make_outset(nil, b, s), make_inset(nil, b, s)
    );
}

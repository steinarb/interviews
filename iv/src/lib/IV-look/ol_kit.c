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
 * OLKit -- object for creating common UI OpenLook-ish objects
 */

#include <IV-look/adjustable.h>
#include <IV-look/adjuster.h>
#include <IV-look/bevel.h>
#include <IV-look/button.h>
#include <IV-look/choice.h>
#include <IV-look/menu.h>
#include <IV-look/kitimpl.h>
#include <IV-look/ol_glyph.h>
#include <IV-look/ol_kit.h>
#include <IV-look/ol_style.h>
#include <IV-look/panner.h>
#include <IV-look/scroller.h>
#include <InterViews/action.h>
#include <InterViews/background.h>
#include <InterViews/bitmap.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/center.h>
#include <InterViews/color.h>
#include <InterViews/glue.h>
#include <InterViews/fixedspan.h>
#include <InterViews/label.h>
#include <InterViews/margin.h>
#include <InterViews/session.h>
#include <InterViews/shapeof.h>
#include <InterViews/stencil.h>
#include <InterViews/style.h>
#include <InterViews/Bitmaps/checkmark.bm>
#include <OS/string.h>

class OLKitImpl {
private:
    friend class OLKit;

    Style* cur_style_;
    OL_Style* cur_ol_style_;
    Bitmap* checkmark_;

    OLKitImpl();
    ~OLKitImpl();

    OL_Style* ol_style(Style*);

    Coord frame_thickness(Style*);
    Glyph* make_inset(Glyph*, Beveler, Style*);
    Glyph* make_outset(Glyph*, Beveler, Style*);
    Glyph* make_flat(Glyph*, Beveler, Style*);
    Glyph* make_label(const char*, Style*);
    Glyph* make_label(const String&, Style*);
    Telltale* make_mover(Beveler, Style*);
    Telltale* make_item(
	Glyph* disabled, Glyph* plain, Glyph* hit,
	Glyph* chosen, Glyph* chosen_hit
    );
};

OLKit::OLKit() {
    impl_ = new OLKitImpl;
}

OLKit::~OLKit() {
    delete impl_;
}

OLKitImpl::OLKitImpl() {
    checkmark_ = new Bitmap(
	checkmark_bits, checkmark_width, checkmark_height,
	checkmark_x_hot, checkmark_y_hot
    );
    checkmark_->ref();
    cur_style_ = nil;
    cur_ol_style_ = nil;
}

OLKitImpl::~OLKitImpl() {
    Resource::unref(checkmark_);
}

Glyph* OLKit::inset_frame(Glyph* g, Style* s) const {
    return impl_->make_inset(g, &Bevel::rect, s);
}

Glyph* OLKit::outset_frame(Glyph* g, Style* s) const {
    return impl_->make_outset(g, &Bevel::rect, s);
}

Glyph* OLKit::flat_frame(Glyph* g, Style* s) const {
    return impl_->make_flat(g, &Bevel::rect, s);
}

Glyph* OLKit::label(const char* str, Style* s) const {
    return impl_->make_label(str, s);
}

Glyph* OLKit::label(const String& str, Style* s) const {
    return impl_->make_label(str, s);
}

Menu* OLKit::menubar(Style* s) const {
    return new Menu(
	new Background(
	    new RMargin(new LRBox, 0.0, fil, 0.0),
	    s->background()
	),
	0.5, 0.0, 0.5, 1.0
    );
}

Telltale* OLKit::menubar_item(Glyph* g, Style* s) const {
    OL_Style* ol = impl_->ol_style(s);
    Glyph* label = new LRBox(
	g,
	new HGlue(5.0, 0.0, 0.0),
	new OL_VerticalMenuMarkGlyph(ol)
    );
    Glyph* disabled = new OL_ButtonGlyph(label, ol, false, true, false, false);
    Glyph* plain = new OL_ButtonGlyph(label, ol, false, false, false, false);
    Glyph* hit = new OL_ButtonGlyph(label, ol, true, false, false, false);
    return impl_->make_item(disabled, plain, hit, plain, hit);
}

Menu* OLKit::pulldown(Style* s) const {
    OL_Style* ol = impl_->ol_style(s);
    Coord frame_size = 1.0;
    return new Menu(
	new Background(
	    new Border(
		new Margin(
		    new OL_ChoiceItemGlyph(
			new TBBox, ol, false, false, false
		    ),
		    frame_size
		),
		s->flat(),
		frame_size
	    ),
	    ol->background()
	),
	1.0, 1.0, 0.0, 1.0
    );
}

Menu* OLKit::pullright(Style* s) const {
    return new Menu(outset_frame(new TBBox, s), 1.0, 1.0, 0.0, 1.0);
}

void OLKit::popup(Menu*) const {
    /* unimplemented */
}

Telltale* OLKit::menu_item(Glyph* g, Style* s) const {
    OL_Style* ol = impl_->ol_style(s);
    Glyph* dimple = new Margin(new OL_DimpleGlyph(ol, true), 0, 3, 1, 0);
    Glyph* spacer = new ShapeOf(dimple);

    Glyph* disabled = new OL_ButtonGlyph(
	new LRBox(spacer, g, spacer), ol, false, true, true, false
    );
    Glyph* plain = new OL_ButtonGlyph(
	new LRBox(spacer, g, spacer), ol, false, false, true, false
    );
    Glyph* hit = new OL_ButtonGlyph(
	new LRBox(spacer, g, spacer), ol, true, false, true, false
    );
    Glyph* chosen = new OL_ButtonGlyph(
	new LRBox(dimple, g, spacer), ol, false, false, true, false
    );
    Glyph* chosen_hit = new OL_ButtonGlyph(
	new LRBox(dimple, g, spacer), ol, true, false, true, false
    );

    return impl_->make_item(disabled, plain, hit, chosen, chosen_hit);
}

Telltale* OLKit::menu_item_separator(Style* s) const {
    return new Telltale(
	new VMargin(
	    new FixedSpan(
		new HMargin(inset_frame(nil, s), 3.0), Dimension_Y, 3.0
	    ),
	    2.0
	)
    );
}

Button* OLKit::push_button(Glyph* g, Style* s, Action* a) const {
    Glyph* m = new Margin(g, 3.0);
    return new Button(
	a,
	new ChoiceItem(outset_frame(m, s), inset_frame(m, s))
    );
}

Button* OLKit::toggle_button(Glyph* g, Style* s, Action* a) const {
    /* unimplemented, return menu_item */
    return new Button(a, menu_item(g, s));
}

Button* OLKit::radio_button(Glyph* g, Style* s, Action* a) const {
    /* unimplemented, return menu_item */
    return new Button(a, menu_item(g, s));
}

Action* OLKit::quit() const {
    return new ActionCallback(Session)(Session::instance(), &Session::quit);
}

Glyph* OLKit::hscroll_bar(Adjustable* a, Style* style) const {
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

Glyph* OLKit::vscroll_bar(Adjustable* a, Style* style) const {
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

Glyph* OLKit::panner(Adjustable* x, Adjustable* y, Style* s) const {
    return new Panner(x, y, s);
}

Button* OLKit::enlarger(Adjustable*, Style*) const {
    return nil;
}

Button* OLKit::reducer(Adjustable*, Style*) const {
    return nil;
}

Button* OLKit::up_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionForward(a, Dimension_Y),
	impl_->make_mover(&Bevel::up_arrow, s)
    );
}

Button* OLKit::down_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionBackward(a, Dimension_Y),
	impl_->make_mover(&Bevel::down_arrow, s)
    );
}

Button* OLKit::left_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionForward(a, Dimension_X),
	impl_->make_mover(&Bevel::left_arrow, s)
    );
}

Button* OLKit::right_mover(Adjustable* a, Style* s) const {
    return new Button(
	new MoverActionBackward(a, Dimension_X),
	impl_->make_mover(&Bevel::right_arrow, s)
    );
}

/** class OLKitImpl **/

OL_Style* OLKitImpl::ol_style(Style* s) {
    if (cur_style_ != s) {
	Resource::unref(cur_ol_style_);
	cur_style_ = s;
	cur_ol_style_ = new OL_Style(s);
	Resource::ref(cur_ol_style_);
    }
    return cur_ol_style_;
}

Coord OLKitImpl::frame_thickness(Style* s) {
    Coord c;
    if (!s->find_attribute("frameThickness", c)) {
	c = 2.0;
    }
    return c;
}

Glyph* OLKitImpl::make_inset(Glyph* g, Beveler b, Style* s) {
    return new Bevel(
	g, b, s->dark(), s->dull(), s->light(), frame_thickness(s)
    );
}

Glyph* OLKitImpl::make_outset(Glyph* g, Beveler b, Style* s) {
    return new Bevel(
	g, b, s->light(), s->flat(), s->dark(), frame_thickness(s)
    );
}

Glyph* OLKitImpl::make_flat(Glyph* g, Beveler b, Style* s) {
    return new Bevel(
	g, b, s->flat(), s->flat(), s->flat(), frame_thickness(s)
    );
}

Glyph* OLKitImpl::make_label(const char* label, Style* s) {
    return new Label(label, s->font(), s->foreground());
}

Glyph* OLKitImpl::make_label(const String& label, Style* s) {
    return new Label(label, s->font(), s->foreground());
}

Telltale* OLKitImpl::make_mover(Beveler b, Style* s) {
    return new ChoiceItem(
	make_outset(nil, b, s), make_inset(nil, b, s)
    );
}

Telltale* OLKitImpl::make_item(
    Glyph* disabled, Glyph* plain, Glyph* hit,
    Glyph* chosen, Glyph* chosen_hit
) {
    ChoiceItem* choice = new ChoiceItem;
    for (unsigned int i = 0; i < choice->numlooks(); i++) {
	Glyph* i_look;
	if (choice->contains_looks(i, ChoiceItem::is_enabled)) {
	    if (choice->contains_looks(i, ChoiceItem::is_pressed)) {
		if (choice->contains_looks(i, ChoiceItem::is_chosen)) {
		    i_look = chosen_hit;
		} else {
		    i_look = hit;
		}
	    } else if (choice->contains_looks(i, ChoiceItem::is_chosen)) {
		i_look = chosen;
	    } else {
		i_look = plain;
	    }
	} else {
	    i_look = disabled;
	}
	choice->look(i, i_look);
    }
    return choice;
}

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
 * Menu - a visible list of Actions
 */

#include <IV-look/menu.h>
#include <IV-look/telltale.h>
#include <InterViews/action.h>
#include <InterViews/hit.h>
#include <InterViews/listener.h>
#include <InterViews/margin.h>
#include <InterViews/patch.h>
#include <InterViews/window.h>
#include <OS/list.h>

class MenuItem {
public:
    MenuItem(Telltale*, Action*, Menu*, Window*);
    ~MenuItem();

    boolean enabled_;
    Telltale* telltale_;
    Action* action_;
    Menu* menu_;
    Window* popup_;
    Patch* patch_;
};

MenuItem::MenuItem(
    Telltale* telltale, Action* action, Menu* menu, Window* window
) {
    enabled_ = telltale != nil && (menu != nil || action != nil);
    telltale_ = telltale;
    if (telltale_ != nil) {
        telltale_->enable(enabled_);
    }
    action_ = action;
    Resource::ref(action_);
    menu_ = menu;
    if (menu_ != nil) {
	if (window == nil) {
	    popup_ = new PopupWindow(menu_);
	} else {
	    popup_ = window;
	}
    } else {
        popup_ = nil;
    }
    patch_ = new Patch(telltale_);
    patch_->ref();
}

MenuItem::~MenuItem() {
    Resource::unref(action_);
    Resource::unref(patch_);
    delete popup_;
}

declareList(MenuItemList,MenuItem*);
implementList(MenuItemList,MenuItem*);

Menu::Menu(
    Glyph* contents, float x1, float y1, float x2, float y2
) : MonoGlyph(nil), Handler() {
    x1_ = x1;
    y1_ = y1;
    x2_ = x2;
    y2_ = y2;
    itemlist_ = new MenuItemList;
    item_ = -1;
    if (contents != nil) {
	body(contents);
    }
}

void Menu::body(Glyph* g) {
    contents_ = g;
    patch_ = new Patch(g);
    listener_ = new Listener(patch_, this);
    listener_->button(true, Event::any);
    MonoGlyph::body(listener_);
}

Glyph* Menu::body() const {
    return MonoGlyph::body();
}

Menu::~Menu() {
    for (GlyphIndex i = 0; i < itemlist_->count(); i++) {
	MenuItem* m = itemlist_->item(i);
	delete m;
    }
    delete itemlist_;
}

void Menu::item(
    GlyphIndex i,
    Telltale* telltale, Action* action, Menu* menu, Window* window
) {
    if (i >= 0) {
	MenuItem* m;
	if (i < itemlist_->count()) {
	    m = itemlist_->item(i);
	    if (m != nil) {
		delete m;
	    }
	}
	m = new MenuItem(telltale, action, menu, window);
	itemlist_->insert(i, m);

	/*
	 * We have to put something in the body before we replace it
	 * because some glyphs (notably Box) assume that replace
	 * is called on an element that has been previously inserted.
	 * This might be considered a bug in Box.
	 */
	Glyph* g = MonoGlyph::body();
	g->insert(i, nil);
	g->replace(i, m->patch_);
    }
}

void Menu::add_item(
    Telltale* telltale, Action* action, Menu* menu, Window* window
) {
    item(itemlist_->count(), telltale, action, menu, window);
}

void Menu::add_item(Telltale* telltale, Action* action) {
    item(itemlist_->count(), telltale, action, nil, nil);
}

void Menu::add_item(Telltale* telltale, Menu* menu, Window* window) {
    item(itemlist_->count(), telltale, nil, menu, window);
}

void Menu::add_item(Telltale* telltale) {
    item(itemlist_->count(), telltale, nil, nil, nil);
}

Telltale* Menu::telltale(GlyphIndex i) const {
    MenuItem* m = itemlist_->item(i);
    return m != nil ? m->telltale_ : nil;
}

Action* Menu::action(GlyphIndex i) const {
    MenuItem* m = itemlist_->item(i);
    return m != nil ? m->action_ : nil;
}

Menu* Menu::menu(GlyphIndex i) const {
    MenuItem* m = itemlist_->item(i);
    return m != nil ? m->menu_ : nil;
}

Window* Menu::window(GlyphIndex i) const {
    MenuItem* m = itemlist_->item(i);
    return m != nil ? m->popup_ : nil;
}

void Menu::select(GlyphIndex index) {
    if (item_ != index) {
	register MenuItem* i = item_ >= 0 ? itemlist_->item(item_) : nil;
        if (i != nil && i->enabled_) {
            if (i->telltale_ != nil) {
                i->telltale_->highlight(false);
            }
            if (i->popup_ != nil) {
                i->popup_->unmap();
            }
        }
        item_ = index;
        i = item_ >= 0 ? itemlist_->item(item_) : nil;
        if (i != nil && i->enabled_) {
            if (i->telltale_ != nil) {
                i->telltale_->highlight(true);
            }
            if (i->popup_ != nil) {
		Window* rel = i->patch_->canvas()->window();
                const Allocation& a = i->patch_->allocation();
                Window* w = i->popup_;
		w->place(
		    rel->left() + (1 - x1_) * a.left() + x1_ * a.right(),
		    rel->bottom() + (1 - y1_) * a.bottom() + y1_ * a.top()
		);
		w->align(x2_, y2_);
		w->map();
            }
        }
    }
}

void Menu::event(Event& e) {
    e.grab(this);
    listener_->motion(true);
    boolean done = false;
    while (!done) {
	if (item_ >= 0) {
	    Menu* m = itemlist_->item(item_)->menu_;
	    if (m != nil) {
		m->ref();
		m->event(e);
		m->unref();
	    }
	}
	switch (e.type()) {
	case Event::up:
	    if (item_ >= 0) {
		Action* a = itemlist_->item(item_)->action_;
		if (a != nil) {
		    a->execute();
		}
	    }
            select(-1);
            done = true;
	    break;
	case Event::motion:
	case Event::down:
	    Handler* target = e.handler();
	    if (target == nil) {
		select(-1);
	    } else if (target == this) {
		Hit hit(&e);
		patch_->repick(0, hit);
		if (hit.any()) {
		    select(hit.index(0));
		}
	    } else {
		select(-1);
		done = true;
	    }
	    break;
        default:
	    break;
	}
        if (!done) {
            e.read();
        }
    }
    listener_->motion(false);
    e.ungrab(this);
}

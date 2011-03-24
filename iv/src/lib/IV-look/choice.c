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
 * ChoiceItem - telltale with different look for each state
 */

#include <IV-look/choice.h>
#include <InterViews/deck.h>

ChoiceItem::ChoiceItem() : Telltale(nil) {
    init();
}

ChoiceItem::ChoiceItem(Glyph* normal, Glyph* pressed) : Telltale(nil) {
    init();
    for (unsigned int i = 0; i < numlooks(); i++) {
	Glyph* item_look;
	if (contains_looks(i, is_pressed)) {
	    item_look = pressed;
	} else {
	    item_look = normal;
	}
	look(i, item_look);
    }
}

ChoiceItem::~ChoiceItem() { }

void ChoiceItem::init() {
    deck_ = new Deck;
    for (unsigned int i = 0; i < numlooks(); i++) {
	deck_->insert(i, nil);
    }
    index_ = is_enabled;
    deck_->flip_to(index_);
    body(deck_);
}

boolean ChoiceItem::contains_looks(
    unsigned int set, unsigned int subset
) const {
    return (set & subset) == subset;
}

void ChoiceItem::look(unsigned int bitset, Glyph* g) {
    if (bitset < numlooks()) {
	deck_->replace(bitset, g);
    }
}

Glyph* ChoiceItem::look(unsigned int bitset) const {
    return bitset < numlooks() ? deck_->component(bitset) : nil;
}

void ChoiceItem::update(unsigned int flag, boolean b) {
    if (b) {
	index_ |= flag;
    } else {
	index_ &= ~flag;
    }
    deck_->flip_to(index_);
}

void ChoiceItem::highlight(boolean b) {
    update(is_pressed, b);
    Telltale::highlight(b);
}

void ChoiceItem::choose(boolean b) {
    update(is_chosen, b);
    Telltale::choose(b);
}

void ChoiceItem::enable(boolean b) {
    update(is_enabled, b);
    Telltale::enable(b);
}

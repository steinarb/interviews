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

#include <InterViews/align.h>
#include <InterViews/deck.h>
#include <InterViews/hit.h>
#include <OS/list.h>

class DeckInfo {
public:
    Glyph* glyph_;
};

declareList(DeckInfo_List,DeckInfo);
implementList(DeckInfo_List,DeckInfo);

Deck::Deck() : Glyph() {
    info_ = new DeckInfo_List();
    card_ = -1;
}

Deck::~Deck() {
    GlyphIndex count = info_->count();
    for (GlyphIndex i = 0; i < count; ++i) {
        DeckInfo& info = info_->item(i);
	Resource::unref(info.glyph_);
    }
    delete info_;
    info_ = nil;
}

GlyphIndex Deck::card() const {
    return card_;
}

void Deck::flip_to(GlyphIndex index) {
    card_ = index;
}

void Deck::append(Glyph* glyph) {
    DeckInfo info;
    info.glyph_ = glyph;
    info_->append(info);
    Resource::ref(glyph);
}

void Deck::prepend(Glyph* glyph) {
    DeckInfo info;
    info.glyph_ = glyph;
    info_->prepend(info);
    Resource::ref(glyph);
}

void Deck::insert(GlyphIndex index, Glyph* glyph) {
    DeckInfo info;
    info.glyph_ = glyph;
    info_->insert(index, info);
    Resource::ref(glyph);
}

void Deck::remove(GlyphIndex index) {
    DeckInfo& info = info_->item(index);
    Resource::unref(info.glyph_);
    info_->remove(index);
}

void Deck::replace(GlyphIndex index, Glyph* glyph) {
    DeckInfo& info = info_->item(index);
    Resource::ref(glyph);
    Resource::unref(info.glyph_);
    info.glyph_ = glyph;
}

GlyphIndex Deck::count() const {
    return info_->count();
}

Glyph* Deck::component(GlyphIndex index) const {
    return info_->item(index).glyph_;
}

void Deck::request(Requisition& requisition) const {
    GlyphIndex count = info_->count();
    Requisition* r = new Requisition[count];
    for (int index = 0; index < count; ++index) {
        DeckInfo& info = info_->item(index);
        if (info.glyph_ != nil) {
            info.glyph_->request(r[index]);
        }
    }
    Align x(Dimension_X);
    x.request(count, r, requisition);
    Align y(Dimension_Y);
    y.request(count, r, requisition);
    delete r;
}

void Deck::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    allocation_ = a;
    GlyphIndex count = info_->count();
    for (GlyphIndex i = 0; i < count; ++i) {
        DeckInfo& info = info_->item(i);
        if (info.glyph_ != nil) {
            info.glyph_->allocate((i == card_) ? c : nil, a, ext);
        }
    }
}

void Deck::draw(Canvas* c, const Allocation& a) const {
    GlyphIndex count = info_->count();
    if (card_ >= 0 && card_ < count) {
        DeckInfo& info = info_->item(card_);
        if (info.glyph_ != nil) {
            info.glyph_->draw(c, a);
        }
    }
}

void Deck::print(Printer* p, const Allocation& a) const {
    GlyphIndex count = info_->count();
    if (card_ >= 0 && card_ < count) {
        DeckInfo& info = info_->item(card_);
        if (info.glyph_ != nil) {
            info.glyph_->print(p, a);
        }
    }
}

void Deck::pick(Canvas* c, const Allocation& a, int depth, Hit& h) {
    GlyphIndex count = info_->count();
    if (card_ >= 0 && card_ < count) {
        DeckInfo& info = info_->item(card_);
        if (info.glyph_ != nil) {
	    h.begin(depth, this, card_);
	    info.glyph_->pick(c, a, depth + 1, h);
	    h.end();
        }
    }
}

void Deck::allotment(GlyphIndex, DimensionName res, Allotment& a) const {
    a = allocation_.allotment(res);
}

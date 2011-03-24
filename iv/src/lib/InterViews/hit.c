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
 * Hit detection
 */

#include <InterViews/event.h>
#include <InterViews/hit.h>
#include <InterViews/transformer.h>
#include <OS/list.h>
#include <OS/math.h>
#include <OS/memory.h>

class HitTarget {
private:
    friend class Hit;
    friend class HitImpl;

    Glyph* glyph_;
    GlyphIndex index_;
    Handler* handler_;
};

class HitTargetList {
private:
    friend class Hit;
    friend class HitImpl;

    enum { fixed = 20, incr = 25 };

    int count_;
    int used_;
    HitTarget fixed_targets_[fixed];
    HitTarget* targets_;
};

declareList(HitList,HitTargetList*);
implementList(HitList,HitTargetList*);

class PossibleHitTarget {
private:
    friend class Hit;
    friend class HitImpl;

    boolean picked_;
    GlyphIndex count_;
    int depth_;
    HitTarget item_;
};

declareList(HitStack,PossibleHitTarget);
implementList(HitStack,PossibleHitTarget);

struct HitTargetArea {
    Coord left;
    Coord bottom;
    Coord right;
    Coord top;
};

declareList(HitTargetAreaList,HitTargetArea);
implementList(HitTargetAreaList,HitTargetArea);

class HitImpl {
private:
    friend class Hit;

    HitImpl(Coord x1, Coord y1, Coord x2, Coord y2);
    ~HitImpl();

    void add_item(int depth, Glyph*, GlyphIndex, Handler*, GlyphIndex target);
    HitTarget& item(int depth, GlyphIndex) const;

    const Event* event_;
    Handler* default_handler_;
    int default_handler_depth_;
    HitList items_;
    HitStack picks_;
    HitTargetAreaList areas_;
};

HitImpl::HitImpl(
    Coord x1, Coord y1, Coord x2, Coord y2
) : items_(20), picks_(20), areas_(5) {
    event_ = nil;
    default_handler_ = nil;
    HitTargetArea a;
    a.left = x1;
    a.bottom = y1;
    a.right = x2;
    a.top = y2;
    areas_.append(a);
}

HitImpl::~HitImpl() {
    for (ListItr(HitList) i(items_); i.more(); i.next()) {
	HitTargetList* t = i.cur();
	if (t->targets_ != t->fixed_targets_) {
	    delete t->targets_;
	}
	delete t;
    }
}

Hit::Hit(HitImpl* h) { impl_ = h; }

Hit::Hit(const Event* e) {
    Coord x = e->pointer_x();
    Coord y = e->pointer_y();
    impl_ = new HitImpl(x, y, x, y);
    impl_->event_ = e;
}

Hit::Hit(Coord x, Coord y) {
    impl_ = new HitImpl(x, y, x, y);
}

Hit::Hit(Coord left, Coord bottom, Coord right, Coord top) {
    impl_ = new HitImpl(left, bottom, right, top);
}

Hit::~Hit() {
    delete impl_;
}

const Event* Hit::event() const { return impl_->event_; }
Coord Hit::left() const { return impl_->areas_.item(0).left; }
Coord Hit::bottom() const { return impl_->areas_.item(0).bottom; }
Coord Hit::right() const { return impl_->areas_.item(0).right; }
Coord Hit::top() const { return impl_->areas_.item(0).top; }

void Hit::push_transform() {
    HitTargetAreaList& list = impl_->areas_;
    HitTargetArea& a = list.item(0);
    list.prepend(a);
}

void Hit::transform(const Transformer& t) {
    HitTargetAreaList& list = impl_->areas_;
    HitTargetArea& a = list.item(0);
    t.inverse_transform(a.left, a.bottom);
    t.inverse_transform(a.right, a.top);
}

void Hit::pop_transform() {
    HitTargetAreaList& list = impl_->areas_;
    if (list.count() != 0) {
	list.remove(0);
    }
}

void Hit::begin(int depth, Glyph* target, GlyphIndex index, Handler* h) {
    PossibleHitTarget p;
    p.picked_ = false;
    p.count_ = impl_->items_.count();
    p.depth_ = depth;
    p.item_.glyph_ = target;
    p.item_.index_ = index;
    p.item_.handler_ = h;
    impl_->picks_.append(p);
}

void Hit::target(int depth, Glyph* target, GlyphIndex index, Handler* h) {
    HitStack& stk = impl_->picks_;
    long top = stk.count() - 1;
    if (top >= 0) {
	stk.item(top).picked_ = true;
    }
    HitTargetList* t = new HitTargetList;
    t->count_ = HitTargetList::fixed;
    t->used_ = -1;
    t->targets_ = t->fixed_targets_;
    impl_->items_.prepend(t);
    impl_->add_item(depth, target, index, h, 0);
}

void Hit::end() {
    HitStack& stk = impl_->picks_;
    long top = stk.count() - 1;
    if (top >= 0) {
	PossibleHitTarget& p = stk.item(top);
	if (p.picked_) {
	    GlyphIndex new_targets = impl_->items_.count() - p.count_;
	    for (int i = 0; i < new_targets; i++) {
		impl_->add_item(
		    p.depth_, p.item_.glyph_, p.item_.index_,
		    p.item_.handler_, i
		);
	    }
	    if (top > 0) {
		stk.item(top - 1).picked_ = true;
	    }
	}
	stk.remove(top);
    }
}

void Hit::remove(int depth, GlyphIndex target) {
    HitTargetList& list = *impl_->items_.item(target);
    for (int i = depth + 1; i <= list.used_; i++) {
	list.targets_[i - 1] = list.targets_[i];
    }
    list.used_ -= 1;
    if (list.targets_[depth].handler_ == impl_->default_handler_) {
	impl_->default_handler_depth_ = depth;
    }
}

void Hit::retarget(
    int depth, Glyph* g, GlyphIndex i, Handler* h, GlyphIndex target
) {
    impl_->add_item(depth, g, i, h, target);
}

boolean Hit::any() const { return impl_->items_.count() != 0; }
int Hit::count() const { return impl_->items_.count(); }
int Hit::depth(GlyphIndex target) const {
    return impl_->items_.item(target)->used_;
}

Glyph* Hit::target(int depth, GlyphIndex target) const {
    return impl_->item(depth, target).glyph_;
}

GlyphIndex Hit::index(int depth, GlyphIndex target) const {
    return impl_->item(depth, target).index_;
}

Handler* Hit::handler() const {
    return impl_->default_handler_;
}

/*
 * Add an item to the current hit list.  Use the depth
 * to place it appropriately.
 */

void HitImpl::add_item(
    int depth, Glyph* g, GlyphIndex i, Handler* h, GlyphIndex target
) {
    HitTarget& t = item(depth, target);
    t.glyph_ = g;
    t.index_ = i;
    t.handler_ = h;

    if (h != nil &&
	(default_handler_ == nil || depth >= default_handler_depth_)
    ) {
	default_handler_ = h;
	default_handler_depth_ = depth;
    }
}

HitTarget& HitImpl::item(int depth, GlyphIndex target) const {
    HitTargetList& list = *items_.item(target);
    if (depth >= list.count_) {
	int new_count = depth + HitTargetList::incr;
	HitTarget* new_targets = new HitTarget[new_count];
	Memory::copy(
	    list.targets_, new_targets, (list.used_ + 1) * sizeof(HitTarget)
	);
	if (list.targets_ != list.fixed_targets_) {
	    delete list.targets_;
	}
	list.count_ = new_count;
	list.targets_ = new_targets;
    }
    if (depth > list.used_) {
	list.used_ = depth;
    }
    return list.targets_[depth];
}

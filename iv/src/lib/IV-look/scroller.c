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
 * Scroller - adjust in one dimension
 */

#include <IV-look/adjustable.h>
#include <IV-look/bevel.h>
#include <IV-look/scroller.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/style.h>
#include <OS/math.h>

Scroller::Scroller(
    DimensionName d, Adjustable* a, Style* s
) : Listener(nil, this), Adjuster(a) {
    dimension_ = d;
    thumb_ = new Bevel(
	nil, &Bevel::rect, s->light(), s->flat(), s->dark(), 2.0
    );
    thumb_->ref();
    canvas_ = nil;
    scale_ = 1.0;
    button(true);
    motion(true);
}

Scroller::~Scroller() {
    Resource::unref(thumb_);
}

/*
 * Simple implementation of short-hand for accessing the adjustable's
 * current settings.  A more sophisticated approach might be to make
 * these inline and/or cache the values (invalidating in update_all).
 */

Coord Scroller::lower() const { return subject()->lower(dimension_); }
Coord Scroller::upper() const { return subject()->upper(dimension_); }
Coord Scroller::length() const { return subject()->length(dimension_); }
Coord Scroller::cur_lower() const { return subject()->cur_lower(dimension_); }
Coord Scroller::cur_upper() const { return subject()->cur_upper(dimension_); }
Coord Scroller::cur_length() const {
    return subject()->cur_length(dimension_);
}
Coord Scroller::offset() const { return cur_lower() - lower(); }

void Scroller::request(Requisition& req) const {
    Requirement r_length(108.0, fil, 108.0, 0.0);
    Requirement r_thickness(15.0, 0.0, 15.0, 0.0);
    req.require(Dimension_X, r_thickness);
    req.require(Dimension_Y, r_thickness);
    req.require(dimension_, r_length);
}

void Scroller::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    canvas_ = c;
    allocation_ = a;
    ext.xy_extents(a);
    update_all();
}

/*
 * We assume that we can draw the thumb without necessarily
 * having allocated it.  This works because thumb is a bevel
 * with nothing inside it.  If it is changed to something that
 * needs to be allocated, then it should be allocated in Scroller::update_all.
 */

void Scroller::draw(Canvas* c, const Allocation&) const {
    thumb_->draw(c, thumb_allocation_);
}

void Scroller::update_all() {
    if (canvas_ != nil) {
	canvas_->damage(
	    thumb_allocation_.left(), thumb_allocation_.bottom(),
	    thumb_allocation_.right(), thumb_allocation_.top()
	);

	Allotment& a = allocation_.allotment(dimension_);
	Coord len = length();
	if (Math::equal(len, float(0.0), float(1e-10))) {
	    scale_ = 1.0;
	} else {
	    scale_ = a.span() / len;
	}
	allocate_thumb(
	    allocation_, thumb_allocation_,
	    scale_ * offset(), scale_ * cur_length()
	);

	/*
	 * Clip the thumb's allotment in the dimension to the scroller's.
	 * We assume the alignment is zero.
	 */
	Allotment& thumb = thumb_allocation_.allotment(dimension_);
	Coord c1 = Math::min(Math::max(thumb.begin(), a.begin()), a.end());
	Coord c2 = Math::max(Math::min(thumb.end(), a.end()), a.begin());
	thumb.origin(c1);
	thumb.span(c2 - c1);

	canvas_->damage(
	    thumb_allocation_.left(), thumb_allocation_.bottom(),
	    thumb_allocation_.right(), thumb_allocation_.top()
	);
    }
}

void Scroller::update(DimensionName d) {
    if (d == dimension_) {
	update_all();
    }
}

void Scroller::press(Event& e) {
    Adjustable* a = subject();
    save_pos_ = a->cur_lower(dimension_) + a->cur_length(dimension_) / 2;
    PointerHandler::press(e);
}

void Scroller::drag(Event& e) {
    subject()->scroll_to(
	dimension_, lower() + pointer(e, allocation_) / scale_
    );
}

void Scroller::abort(Event& e) {
    subject()->scroll_to(dimension_, save_pos_);
    PointerHandler::abort(e);
}

HScroller::HScroller(Adjustable* a, Style* s) : Scroller(Dimension_X, a, s) { }
HScroller::~HScroller() { }

void HScroller::allocate_thumb(
    const Allocation& a, Allocation& thumb, Coord position, Coord length
) {
    Allotment ax(a.left() + position, length, 0.0);
    Allotment ay(a.bottom(), a.allotment(Dimension_Y).span(), 0.0);
    thumb.allot(Dimension_X, ax);
    thumb.allot(Dimension_Y, ay);
}

Coord HScroller::pointer(const Event& e, const Allocation& a) const {
    return e.pointer_x() - a.left();
}

VScroller::VScroller(Adjustable* a, Style* s) : Scroller(Dimension_Y, a, s) { }
VScroller::~VScroller() { }

void VScroller::allocate_thumb(
    const Allocation& a, Allocation& thumb, Coord position, Coord length
) {
    Allotment ax(a.left(), a.allotment(Dimension_X).span(), 0.0);
    Allotment ay(a.bottom() + position, length, 0.0);
    thumb.allot(Dimension_X, ax);
    thumb.allot(Dimension_Y, ay);
}

Coord VScroller::pointer(const Event& e, const Allocation& a) const {
    return e.pointer_y() - a.bottom();
}

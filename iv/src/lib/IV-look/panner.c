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
 * Panner - adjust in xy dimensions
 */

#include <IV-look/adjustable.h>
#include <IV-look/bevel.h>
#include <IV-look/panner.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/handler.h>
#include <InterViews/style.h>

Panner::Panner(
    Adjustable* x, Adjustable* y, Style* s
) : Listener(nil, this), Adjuster(x) {
    y_adjust_ = y;
    thumb_ = new Bevel(
	nil, &Bevel::rect, s->light(), s->flat(), s->dark(), 2.0
    );
    thumb_->ref();
    canvas_ = nil;
    xscale_ = 1.0;
    yscale_ = 1.0;
    button(true);
}

Panner::~Panner() {
    Resource::unref(thumb_);
}

void Panner::request(Requisition& req) const {
    Requirement r(72.0, fil, 72.0, 0.0);
    req.require(Dimension_X, r);
    req.require(Dimension_Y, r);
}

void Panner::allocate(Canvas* c, const Allocation& a, Extension& ext) {
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

void Panner::draw(Canvas* c, const Allocation&) const {
    thumb_->draw(c, thumb_allocation_);
}

void Panner::update(DimensionName) {
    update_all();
}

void Panner::update_all() {
    if (canvas_ != nil) {
	/* first damage the old thumb area */
	canvas_->damage(
	    thumb_allocation_.left(), thumb_allocation_.bottom(),
	    thumb_allocation_.right(), thumb_allocation_.top()
	);

	/* now compute the new thumb location */
	Adjustable* x = subject();
	Adjustable* y = y_adjust_;
	const DimensionName dx = Dimension_X, dy = Dimension_Y;
	Allocation& a = allocation_;
	Coord a_xlen = x->length(dx);
	if (Math::equal(a_xlen, float(0.0), float(1e-6))) {
	    xscale_ = 1.0;
	} else {
	    xscale_ = a.allotment(dx).span() / x->length(dx);
	}
	Coord a_ylen = y->length(dy);
	if (Math::equal(a_ylen, float(0.0), float(1e-6))) {
	    yscale_ = 1.0;
	} else {
	    yscale_ = a.allotment(dy).span() / y->length(dy);
	}
	Coord xoff = xscale_ * (x->cur_lower(dx) - x->lower(dx));
	Coord yoff = yscale_ * (y->cur_lower(dy) - y->lower(dy));
	Coord xlen = xscale_ * x->cur_length(dx);
	Coord ylen = yscale_ * y->cur_length(dy);

	Allotment ax(a.left() + xoff, xlen, 0.0);
	Allotment ay(a.top() - yoff - ylen, ylen, 0.0);
	thumb_allocation_.allot(Dimension_X, ax);
	thumb_allocation_.allot(Dimension_Y, ay);

	/* now damage the new thumb area */
	canvas_->damage(
	    thumb_allocation_.left(), thumb_allocation_.bottom(),
	    thumb_allocation_.right(), thumb_allocation_.top()
	);
    }
}

void Panner::event(Event& e) {
    if (e.type() != Event::down) {
	return;
    }
    e.grab(this);
    motion(true);
    boolean done = false;
    Adjustable* x = subject();
    Adjustable* y = y_adjust_;
    Coord xbase = x->lower(Dimension_X);
    Coord ybase = y->lower(Dimension_Y);
    Allocation& a = allocation_;
    for (;;) {
	switch (e.type()) {
	case Event::up:
	    done = true;
	    /* fall through */
	case Event::motion:
	case Event::down:
	    x->scroll_to(
		Dimension_X, xbase + (e.pointer_x() - a.left()) / xscale_
	    );
	    y->scroll_to(
		Dimension_Y, ybase + (a.top() - e.pointer_y()) / yscale_
	    );
	    break;
	default:
	    break;
	}
	if (done) {
	    break;
	}
	e.read();
    }
    motion(false);
    e.ungrab(this);
}

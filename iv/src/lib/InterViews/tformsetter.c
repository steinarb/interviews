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
 * TransformSetter
 */

#include <InterViews/boolean.h>
#include <InterViews/canvas.h>
#include <InterViews/hit.h>
#include <InterViews/printer.h>
#include <InterViews/tformsetter.h>

TransformSetter::TransformSetter(
    Glyph* body, const Transformer& tx
) : MonoGlyph(body) {
    transformer_ = tx;
}

TransformSetter::~TransformSetter() { }

static void tx_box(
    const Transformer& tx, Coord& l, Coord& b, Coord& r, Coord& t
) {
    Coord x[4], y[4];
    tx.transform(l, b, x[0], y[0]);
    tx.transform(l, t, x[1], y[1]);
    tx.transform(r, t, x[2], y[2]);
    tx.transform(r, b, x[3], y[3]);
    l = x[0]; r = x[0];
    b = y[0]; t = y[0];
    for (int i = 1; i < 4; i++) {
	if (x[i] < l) {
	    l = x[i];
	} else if (x[i] > r) {
	    r = x[i];
	}
	if (y[i] < b) {
	    b = y[i];
	} else if (y[i] > t) {
	    t = y[i];
	}
    }
}

static void inv_tx_box(
    const Transformer& tx, Coord& l, Coord& b, Coord& r, Coord& t
) {
    Coord x[4], y[4];
    tx.inverse_transform(l, b, x[0], y[0]);
    tx.inverse_transform(l, t, x[1], y[1]);
    tx.inverse_transform(r, t, x[2], y[2]);
    tx.inverse_transform(r, b, x[3], y[3]);
    l = x[0]; r = x[0];
    b = y[0]; t = y[0];
    for (int i = 1; i < 4; i++) {
	if (x[i] < l) {
	    l = x[i];
	} else if (x[i] > r) {
	    r = x[i];
	}
	if (y[i] < b) {
	    b = y[i];
	} else if (y[i] > t) {
	    t = y[i];
	}
    }
}

void TransformSetter::request(Requisition& requisition) const {
    Requisition req;
    MonoGlyph::request(req);
    Requirement& x = req.requirement(Dimension_X);
    Requirement& y = req.requirement(Dimension_Y);
    Coord l = -(x.natural() * x.alignment());
    Coord b = -(y.natural() * y.alignment());
    Coord r = x.natural() * (1 - x.alignment());
    Coord t = y.natural() * (1 - y.alignment());
    tx_box(transformer_, l, b, r, t);
    Requirement rx(-l, -l, -l, r, r, r);
    Requirement ry(-b, -b, -b, t, t, t);
    requisition.require(Dimension_X, rx);
    requisition.require(Dimension_Y, ry);
}

void TransformSetter::allocate(
    Canvas* c, const Allocation& a, Extension& ext
) {
    Coord left = a.left();
    Coord right = a.right();
    Coord top = a.top();
    Coord bottom = a.bottom();
    inv_tx_box(transformer_, left, bottom, right, top);

    Coord x = 0;
    Coord y = 0;
    transformer_.transform(x, y);
    x += a.x();
    y += a.y();
    transformer_.inverse_transform(x, y);
    Coord xspan = right - left;
    Coord yspan = top - bottom;
    float xalign = (xspan == 0) ? 0 : float(x - left)/float(xspan);
    float yalign = (yspan == 0) ? 0 : float(y - bottom)/float(yspan);

    Allotment ax(x, xspan, xalign);
    Allotment ay(y, yspan, yalign);
    allocation_.allot(Dimension_X, ax);
    allocation_.allot(Dimension_Y, ay);

    Extension e;
    e.xy_extents(fil, -fil, fil, -fil);

    if (c != nil) {
	c->push_transform();
	c->transform(transformer_);
    }
    MonoGlyph::allocate(c, allocation_, e);
    if (c != nil) {
	c->pop_transform();
    }

    Coord ll = e.left() - x;
    Coord bb = e.bottom() - y;
    Coord rr = e.right() - x;
    Coord tt = e.top() - y;
    tx_box(transformer_, ll, bb, rr, tt);
    ext.xy_extents(a.x() + ll, a.x() + rr, a.y() + bb, a.y() + tt);
}

void TransformSetter::draw(Canvas* c, const Allocation&) const {
    c->push_transform();
    c->transform(transformer_);
    MonoGlyph::draw(c, allocation_);
    c->pop_transform();
}

void TransformSetter::print(Printer* p, const Allocation&) const {
    p->push_transform();
    p->transform(transformer_);
    MonoGlyph::print(p, allocation_);
    p->pop_transform();
}

void TransformSetter::pick(Canvas* c, const Allocation&, int depth, Hit& h) {
    h.push_transform();
    h.transform(transformer_);
    c->push_transform();
    c->transform(transformer_);
    MonoGlyph::pick(c, allocation_, depth, h);
    c->pop_transform();
    h.pop_transform();
}

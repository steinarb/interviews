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
 * Margin - space around a Glyph
 */

#include <InterViews/margin.h>

Margin::Margin(Glyph* body, Coord margin) : MonoGlyph(body) {
    lnatural_ = margin; lstretch_ = 0; lshrink_ = 0;
    rnatural_ = margin; rstretch_ = 0; rshrink_ = 0;
    bnatural_ = margin; bstretch_ = 0; bshrink_ = 0;
    tnatural_ = margin; tstretch_ = 0; tshrink_ = 0;
}

Margin::Margin(Glyph* body, Coord hmargin, Coord vmargin) : MonoGlyph(body) {
    lnatural_ = hmargin; lstretch_ = 0; lshrink_ = 0;
    rnatural_ = hmargin; rstretch_ = 0; rshrink_ = 0;
    bnatural_ = vmargin; bstretch_ = 0; bshrink_ = 0;
    tnatural_ = vmargin; tstretch_ = 0; tshrink_ = 0;
}

Margin::Margin(
    Glyph* body,
    Coord lmargin, Coord rmargin, Coord bmargin, Coord tmargin
) : MonoGlyph(body) {
    lnatural_ = lmargin; lstretch_ = 0; lshrink_ = 0;
    rnatural_ = rmargin; rstretch_ = 0; rshrink_ = 0;
    bnatural_ = bmargin; bstretch_ = 0; bshrink_ = 0;
    tnatural_ = tmargin; tstretch_ = 0; tshrink_ = 0;
}

Margin::Margin(
    Glyph* body,
    Coord lmargin, Coord lstretch, Coord lshrink,
    Coord rmargin, Coord rstretch, Coord rshrink,
    Coord bmargin, Coord bstretch, Coord bshrink,
    Coord tmargin, Coord tstretch, Coord tshrink
) : MonoGlyph(body) {
    lnatural_ = lmargin; lstretch_ = lstretch; lshrink_ = lshrink;
    rnatural_ = rmargin; rstretch_ = rstretch; rshrink_ = rshrink;
    bnatural_ = bmargin; bstretch_ = bstretch; bshrink_ = bshrink;
    tnatural_ = tmargin; tstretch_ = tstretch; tshrink_ = tshrink;
}

Margin::~Margin() { }

void Margin::request(Requisition& requisition) const {
    Margin* m = (Margin*)this;
    MonoGlyph::request(m->requisition_);
    Requirement x = m->requisition_.requirement(Dimension_X);
    if (x.defined()) {
	x.natural(x.natural() + lnatural_ + rnatural_);
	x.stretch(x.stretch() + lstretch_ + rstretch_);
	x.shrink(x.shrink() + lshrink_ + rshrink_);
    }
    m->requisition_.require(Dimension_X, x);
    Requirement y = m->requisition_.requirement(Dimension_Y);
    if (y.defined()) {
	y.natural(y.natural() + bnatural_ + tnatural_);
	y.stretch(y.stretch() + bstretch_ + tstretch_);
	y.shrink(y.shrink() + bshrink_ + tshrink_);
    }
    m->requisition_.require(Dimension_Y, y);
    requisition = requisition_;
}

static Coord span(
    Coord span, Requirement& total,
    Coord natural, Coord stretch, Coord shrink
) {
    Coord extra = span - total.natural();
    Coord result = natural;
    float ss = 0.0;
    if (extra > 0 && total.stretch() > 0) {
        ss = float(stretch)/float(total.stretch());
    } else if (extra < 0 && total.shrink() > 0) {
        ss = float(shrink)/float(total.shrink());
    }
    result += Coord(ss * float(extra));
    return result;
}

void Margin::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    Allotment x = a.allotment(Dimension_X);
    Requirement rx = requisition_.requirement(Dimension_X);
    Coord lmargin = span(x.span(), rx, lnatural_, lstretch_, lshrink_);
    Coord rmargin = span(x.span(), rx, rnatural_, rstretch_, rshrink_);
    x.span(x.span() - (lmargin + rmargin));
    x.offset(Coord((1-rx.alignment()) * lmargin - rx.alignment() * rmargin));
    allocation_.allot(Dimension_X, x);
    Allotment y = a.allotment(Dimension_Y);
    Requirement ry = requisition_.requirement(Dimension_Y);
    Coord bmargin = span(y.span(), ry, bnatural_, bstretch_, bshrink_);
    Coord tmargin = span(y.span(), ry, tnatural_, tstretch_, tshrink_);
    y.span(y.span() - (bmargin + tmargin));
    y.offset(Coord((1-ry.alignment()) * bmargin - ry.alignment() * tmargin));
    allocation_.allot(Dimension_Y, y);
    MonoGlyph::allocate(c, allocation_, ext);
}    

void Margin::draw(Canvas* c, const Allocation&) const {
    MonoGlyph::draw(c, allocation_);
}

void Margin::print(Printer* p, const Allocation&) const {
    MonoGlyph::print(p, allocation_);
}

HMargin::HMargin(
    Glyph* body, Coord margin
) : Margin(body, margin, 0, 0, margin, 0, 0, 0, 0, 0, 0, 0, 0) { }

HMargin::HMargin(
    Glyph* body, Coord lmargin, Coord rmargin
) : Margin(body, lmargin, 0, 0, rmargin, 0, 0, 0, 0, 0, 0, 0, 0) { }

HMargin::HMargin(
    Glyph* body,
    Coord lmargin, Coord lstretch, Coord lshrink,
    Coord rmargin, Coord rstretch, Coord rshrink
) : Margin(
    body,
    lmargin, lstretch, lshrink, rmargin, rstretch, rshrink,
    0, 0, 0, 0, 0, 0
) { }

HMargin::~HMargin() { }

VMargin::VMargin(
    Glyph* body, Coord margin
) : Margin(body, 0, 0, 0, 0, 0, 0, margin, 0, 0, margin, 0, 0) { }

VMargin::VMargin(
    Glyph* body, Coord bmargin, Coord tmargin
) : Margin(body, 0, 0, 0, 0, 0, 0, bmargin, 0, 0, tmargin, 0, 0) { }

VMargin::VMargin(
    Glyph* body,
    Coord bmargin, Coord bstretch, Coord bshrink,
    Coord tmargin, Coord tstretch, Coord tshrink
) : Margin(
    body,
    0, 0, 0, 0, 0, 0,
    bmargin, bstretch, bshrink, tmargin, tstretch, tshrink
) { }

VMargin::~VMargin() { }

LMargin::LMargin(
    Glyph* body, Coord margin
) : Margin(body, margin, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) { }

LMargin::LMargin(
    Glyph* body, Coord lmargin, Coord lstretch, Coord lshrink
) : Margin(body, lmargin, lstretch, lshrink, 0, 0, 0, 0, 0, 0, 0, 0, 0) { }

LMargin::~LMargin() { }

RMargin::RMargin(
    Glyph* body, Coord margin
) : Margin(body, 0, 0, 0, margin, 0, 0, 0, 0, 0, 0, 0, 0) { }

RMargin::RMargin(
    Glyph* body, Coord rmargin, Coord rstretch, Coord rshrink
) : Margin(body, 0, 0, 0, rmargin, rstretch, rshrink, 0, 0, 0, 0, 0, 0) { }

RMargin::~RMargin() { }

TMargin::TMargin(
    Glyph* body, Coord margin
) : Margin(body, 0, 0, 0, 0, 0, 0, 0, 0, 0, margin, 0, 0) { }

TMargin::TMargin(
    Glyph* body, Coord tmargin, Coord tstretch, Coord tshrink
) : Margin(body, 0, 0, 0, 0, 0, 0, 0, 0, 0, tmargin, tstretch, tshrink) { }

TMargin::~TMargin() { }

BMargin::BMargin(
    Glyph* body, Coord margin
) : Margin(body, 0, 0, 0, 0, 0, 0, margin, 0, 0, 0, 0, 0) { }

BMargin::BMargin(
    Glyph* body, Coord bmargin, Coord bstretch, Coord bshrink
) : Margin(body, 0, 0, 0, 0, 0, 0, bmargin, bstretch, bshrink, 0, 0, 0) { }

BMargin::~BMargin() { }

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
 * Center - override Glyph alignment
 */

#include <InterViews/center.h>

Center::Center(
    Glyph* body, float xalignment, float yalignment
) : MonoGlyph(body) {
    xaligned_ = true;
    xalignment_ = xalignment;
    yaligned_ = true;
    yalignment_ = yalignment;
}

Center::Center(
    Glyph* body, DimensionName dimension, float alignment
) : MonoGlyph(body) {
    xaligned_ = false;
    yaligned_ = false;
    if (dimension == Dimension_X) {
        xaligned_ = true;
        xalignment_ = alignment;
    } else if (dimension == Dimension_Y) {
        yaligned_ = true;
        yalignment_ = alignment;
    }
}

Center::~Center() { }

void Center::request(Requisition& requisition) const {
    MonoGlyph::request(requisition);
    Center* c = (Center*)this;
    if (xaligned_) {
        Requirement x = requisition.requirement(Dimension_X);
        c->cxalignment_ = x.alignment();
        x.alignment(xalignment_);
        requisition.require(Dimension_X, x);
    }
    if (yaligned_) {
        Requirement y = requisition.requirement(Dimension_Y);
        c->cyalignment_ = y.alignment();
        y.alignment(yalignment_);
        requisition.require(Dimension_Y, y);
    }
}

void Center::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    allocation_ = a;
    if (xaligned_) {
        Allotment x = a.allotment(Dimension_X);
        x.offset(Coord(float(x.span()) * (cxalignment_ - x.alignment())));
        x.alignment(cxalignment_);
        allocation_.allot(Dimension_X, x);
    }
    if (yaligned_) {
        Allotment y = a.allotment(Dimension_Y);
        y.offset(Coord(float(y.span()) * (cyalignment_ - y.alignment())));
        y.alignment(cyalignment_);
        allocation_.allot(Dimension_Y, y);
    }
    MonoGlyph::allocate(c, allocation_, ext);
}    

void Center::draw(Canvas* c, const Allocation&) const {
    MonoGlyph::draw(c, allocation_);
}

void Center::print(Printer* p, const Allocation&) const {
    MonoGlyph::print(p, allocation_);
}

HCenter::HCenter(
    Glyph* body, float alignment
) : Center(body, Dimension_X, alignment) { }

HCenter::~HCenter() { }

VCenter::VCenter(
    Glyph* body, float alignment
) : Center(body, Dimension_Y, alignment) { }

VCenter::~VCenter() { }

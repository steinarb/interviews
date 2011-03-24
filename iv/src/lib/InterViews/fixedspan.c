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
 * FixedSpan - allocate fixed size to component
 */

#include <InterViews/fixedspan.h>

FixedSpan::FixedSpan(
    Glyph* body, DimensionName dimension, Coord span
) : MonoGlyph(body) {
    dimension_ = dimension;
    span_ = span;
}

FixedSpan::~FixedSpan() { }

void FixedSpan::request(Requisition& requisition) const {
    MonoGlyph::request(requisition);
    Requirement& r = requisition.requirement(dimension_);
    Requirement nr(span_, 0, 0, r.alignment());
    requisition.require(dimension_, nr);
}

void FixedSpan::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    allocation_ = a;
    const Allotment& al = a.allotment(dimension_);
    Allotment na(al.origin(), span_, al.alignment());
    allocation_.allot(dimension_, na);
    MonoGlyph::allocate(c, allocation_, ext);
}

void FixedSpan::draw(Canvas* c, const Allocation&) const {
    MonoGlyph::draw(c, allocation_);
}

void FixedSpan::print(Printer* p, const Allocation&) const {
    MonoGlyph::print(p, allocation_);
}

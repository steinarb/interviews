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
 * Glue - filler glyph
 */

#include <InterViews/glue.h>

Glue::Glue(
    DimensionName d,
    Coord natural, Coord stretch, Coord shrink, float alignment
) : Glyph() {
    Requirement r(natural, stretch, shrink, alignment);
    requisition_.require(d, r);
}

Glue::Glue(const Requisition& r) : Glyph() {
    requisition_ = r;
}

Glue::~Glue() { }

void Glue::request(Requisition& r) const {
    r = requisition_;
}

void Glue::allocate(Canvas*, const Allocation& a, Extension& ext) {
    ext.xy_extents(a);
}

HGlue::HGlue() : Glue(Dimension_X, 0, fil, 0, 0.0) { }

HGlue::HGlue(Coord natural) : Glue(Dimension_X, natural, fil, 0, 0.0) { }

HGlue::HGlue(
    Coord natural, Coord stretch, Coord shrink
) : Glue(Dimension_X, natural, stretch, shrink, 0.0) { }

HGlue::HGlue(
    Coord natural, Coord stretch, Coord shrink, float alignment
) : Glue(Dimension_X, natural, stretch, shrink, alignment) { }

HGlue::~HGlue() { }

VGlue::VGlue() : Glue(Dimension_Y, 0, fil, 0, 0.0) { }

VGlue::VGlue(Coord natural) : Glue(Dimension_Y, natural, fil, 0, 0.0) { }

VGlue::VGlue(
    Coord natural, Coord stretch, Coord shrink
) : Glue(Dimension_Y, natural, stretch, shrink, 0.0) { }

VGlue::VGlue(
    Coord natural, Coord stretch, Coord shrink, float alignment
) : Glue(Dimension_Y, natural, stretch, shrink, alignment) { }

VGlue::~VGlue() { }

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
 * Box - tiling glyph
 */

#ifndef iv_box_h
#define iv_box_h

#include <InterViews/polyglyph.h>

#include <InterViews/_enter.h>

class BoxImpl;
class Layout;

class Box : public PolyGlyph {
public:
    Box(Layout* layout, GlyphIndex size);
    Box(
        Layout* layout,
        Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil,
        Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil, Glyph* =nil
    );
    virtual ~Box();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void print(Printer*, const Allocation&) const;
    virtual void pick(Canvas*, const Allocation&, int depth, Hit&);
    virtual void undraw();

    virtual void modified(GlyphIndex);

    virtual void allotment(GlyphIndex, DimensionName, Allotment&) const;
private:
    BoxImpl* impl_;
};

#include <InterViews/_leave.h>

#endif

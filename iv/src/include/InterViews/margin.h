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

#ifndef iv_margin_h
#define iv_margin_h

#include <InterViews/monoglyph.h>

class Margin : public MonoGlyph {
public:
    Margin(
        Glyph* body, Coord margin
    );
    Margin(
        Glyph* body, Coord hmargin, Coord vmargin
    );
    Margin(
        Glyph* body,
        Coord lmargin, Coord rmargin, Coord bmargin, Coord tmargin
    );
    Margin(
        Glyph* body,
        Coord lmargin, Coord lstretch, Coord lshrink,
        Coord rmargin, Coord rstretch, Coord rshrink,
        Coord bmargin, Coord bstretch, Coord bshrink,
        Coord tmargin, Coord tstretch, Coord tshrink
    );
    virtual ~Margin();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void print(Printer*, const Allocation&) const;
private:
    Coord lnatural_, lstretch_, lshrink_;
    Coord rnatural_, rstretch_, rshrink_;
    Coord bnatural_, bstretch_, bshrink_;
    Coord tnatural_, tstretch_, tshrink_;
    Requisition requisition_;
    Allocation allocation_;
};

class HMargin : public Margin {
public:
    HMargin(Glyph* body, Coord margin);
    HMargin(Glyph* body, Coord lmargin, Coord rmargin);
    HMargin(
        Glyph* body,
        Coord lmargin, Coord lstretch, Coord lshrink,
        Coord rmargin, Coord rstretch, Coord rshrink
    );
    virtual ~HMargin();
};

class VMargin : public Margin {
public:
    VMargin(Glyph* body, Coord margin);
    VMargin(Glyph* body, Coord bmargin, Coord tmargin);
    VMargin(
        Glyph* body,
        Coord bmargin, Coord bstretch, Coord bshrink,
        Coord tmargin, Coord tstretch, Coord tshrink
    );
    virtual ~VMargin();
};

class LMargin : public Margin {
public:
    LMargin(Glyph* body, Coord margin);
    LMargin(Glyph* body, Coord lmargin, Coord lstretch, Coord lshrink);
    virtual ~LMargin();
};

class RMargin : public Margin {
public:
    RMargin(Glyph* body, Coord margin);
    RMargin(Glyph* body, Coord rmargin, Coord rstretch, Coord rshrink);
    virtual ~RMargin();
};

class TMargin : public Margin {
public:
    TMargin(Glyph* body, Coord margin);
    TMargin(Glyph* body, Coord tmargin, Coord tstretch, Coord tshrink);
    virtual ~TMargin();
};

class BMargin : public Margin {
public:
    BMargin(Glyph* body, Coord margin);
    BMargin(Glyph* body, Coord bmargin, Coord bstretch, Coord bshrink);
    virtual ~BMargin();
};

#endif

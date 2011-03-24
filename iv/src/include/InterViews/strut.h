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
 * Strut - filler glyph
 */

#ifndef iv_strut_h
#define iv_strut_h

#include <InterViews/glyph.h>

class Font;

class Strut : public Glyph {
public:
    Strut(
        const Font*,
        Coord natural = 0, Coord stretch = 0, Coord shrink = 0
    );
    virtual ~Strut();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
private:
    const Font* font_;
    Coord natural_;
    Coord stretch_;
    Coord shrink_;
    Coord height_;
    float alignment_;
};

class HStrut : public Glyph {
public:
    HStrut(
        Coord right_bearing, Coord left_bearing = 0,
        Coord natural = 0, Coord stretch = 0, Coord shrink = 0
    );
    virtual ~HStrut();

    virtual void request(Requisition&) const;
private:
    Coord left_bearing_;
    Coord right_bearing_;
    Coord natural_;
    Coord stretch_;
    Coord shrink_;
};

class VStrut : public Glyph {
public:
    VStrut(
        Coord ascent, Coord descent = 0,
        Coord natural = 0, Coord stretch = 0, Coord shrink = 0
    );
    virtual ~VStrut();

    virtual void request(Requisition&) const;
private:
    Coord ascent_;
    Coord descent_;
    Coord natural_;
    Coord stretch_;
    Coord shrink_;
};

#endif

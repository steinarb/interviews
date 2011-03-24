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
 * Adjustable - object to scroll and/or zoom
 */

#ifndef ivlook_adjustable_h
#define ivlook_adjustable_h

#include <InterViews/geometry.h>

#include <InterViews/_enter.h>

class Adjuster;

class Adjustable {
public:
    virtual ~Adjustable();

    virtual void attach(Adjuster*);
    virtual void detach(Adjuster*);

    virtual Coord lower(DimensionName) const = 0;
    virtual Coord upper(DimensionName) const = 0;
    virtual Coord length(DimensionName) const = 0;
    virtual Coord cur_lower(DimensionName) const = 0;
    virtual Coord cur_upper(DimensionName) const = 0;
    virtual Coord cur_length(DimensionName) const = 0;

    virtual void scroll_forward(DimensionName);
    virtual void scroll_backward(DimensionName);
    virtual void page_forward(DimensionName);
    virtual void page_backward(DimensionName);

    virtual void scroll_to(DimensionName, Coord position);
    virtual void scale_to(DimensionName, float fraction_visible);
    virtual void zoom_to(float magnification);
protected:
    Adjustable();

    virtual void constrain(DimensionName, Coord& position) const;
    virtual void notify() const;
    virtual void notify(DimensionName) const;
private:
    class AdjusterList* views_;
};

#include <InterViews/_leave.h>

#endif

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
 * Scroller - adjust one dimension
 */

#ifndef ivlook_scroller_h
#define ivlook_scroller_h

#include <IV-look/adjuster.h>
#include <InterViews/ptrhandler.h>
#include <InterViews/listener.h>

class Style;

class Scroller : public Listener, public Adjuster, public PointerHandler {
protected:
    Scroller(DimensionName, Adjustable*, Style*);
public:
    virtual ~Scroller();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;

    virtual void update(DimensionName);
    virtual void update_all();

    virtual void press(Event&);
    virtual void drag(Event&);
    virtual void abort(Event&);
protected:
    Coord lower() const;
    Coord upper() const;
    Coord length() const;
    Coord cur_lower() const;
    Coord cur_upper() const;
    Coord cur_length() const;
    Coord offset() const;

    virtual void allocate_thumb(
	const Allocation& scroller, Allocation& thumb,
	Coord position, Coord length
    ) = 0;
    virtual Coord pointer(const Event&, const Allocation&) const = 0;
private:
    DimensionName dimension_;
    Glyph* thumb_;
    Canvas* canvas_;
    Allocation allocation_;
    Allocation thumb_allocation_;
    Coord save_pos_;
    float scale_;
};

class HScroller : public Scroller {
public:
    HScroller(Adjustable*, Style*);
    virtual ~HScroller();
protected:
    virtual void allocate_thumb(
	const Allocation& scroller, Allocation& thumb,
	Coord position, Coord length
    );
    virtual Coord pointer(const Event&, const Allocation&) const;
};

class VScroller : public Scroller {
public:
    VScroller(Adjustable*, Style*);
    virtual ~VScroller();
protected:
    virtual void allocate_thumb(
	const Allocation& scroller, Allocation& thumb,
	Coord position, Coord length
    );
    virtual Coord pointer(const Event&, const Allocation&) const;
};

#endif

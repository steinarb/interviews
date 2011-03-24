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

#ifndef ivlook_ol_glyph_h
#define ivlook_ol_glyph_h

#include <InterViews/monoglyph.h>

class OL_Style;

class OL_ButtonGlyph : public MonoGlyph {
public:
    OL_ButtonGlyph(
        Glyph* label, OL_Style*,
        boolean invoked, boolean inactive, boolean menu, boolean def
    );
    virtual ~OL_ButtonGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
protected:
    void path(Canvas*, int, Coord, Coord, Coord, Coord) const;
    void top_path(Canvas*, int, Coord, Coord, Coord, Coord) const;
    void bottom_path(Canvas*, int, Coord, Coord, Coord, Coord) const;

    boolean invoked_;
    boolean inactive_;
    boolean menu_;
    boolean default_;
    OL_Style* style_;
};

class OL_AbbrevButtonGlyph : public MonoGlyph {
public:
    OL_AbbrevButtonGlyph(
        Glyph* label, OL_Style*, boolean invoked, boolean inactive
    );
    virtual ~OL_AbbrevButtonGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
protected:
    boolean invoked_;
    boolean inactive_;
    OL_Style* style_;
    Coord width_;
    Coord height_;
};

class OL_ChoiceItemGlyph : public MonoGlyph {
public:
    OL_ChoiceItemGlyph(
        Glyph* label, OL_Style*,
        boolean invoked, boolean inactive, boolean def
    );
    virtual ~OL_ChoiceItemGlyph();

    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
private:
    boolean invoked_;
    boolean inactive_;
    boolean default_;
    OL_Style* style_;
};

class OL_ChiseledBorder : public MonoGlyph {
public:
    OL_ChiseledBorder(Glyph* contents, OL_Style*);
    virtual ~OL_ChiseledBorder();

    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
private:
    OL_Style* style_;
};

class OL_PushPinGlyph : public Glyph {
public:
    OL_PushPinGlyph(OL_Style*, boolean invoked, boolean inactive);
    virtual ~OL_PushPinGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
private:
    boolean pushed_;
    boolean inactive_;
    OL_Style* style_;
    Coord width_;
    Coord height_;
};


class OL_ElevatorGlyph : public Glyph {
public:
    enum { normal, abbreviated };

    virtual void set_dimple(boolean);
    virtual void set_forward(boolean) = 0;
    virtual void set_backward(boolean) = 0;
    virtual void set_abbreviated(boolean) = 0;
    virtual Coord length() const;
    virtual boolean get_abbreviated() const;
protected:
    OL_ElevatorGlyph();
    virtual ~OL_ElevatorGlyph();

    boolean dimpled_;
    boolean abbreviated_;
    OL_Style* style_;
    Coord length_;
    Coord width_;
    Coord height_;
};

class OL_HorizontalElevatorGlyph : public OL_ElevatorGlyph {
public:
    OL_HorizontalElevatorGlyph(
        OL_Style*, boolean abbreviated = false, boolean left = false,
        boolean right = false, boolean dimpled = false
    );
    virtual ~OL_HorizontalElevatorGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void set_forward(boolean);
    virtual void set_backward(boolean);
    virtual void set_abbreviated(boolean);
private:
    boolean left_;
    boolean right_;
};

class OL_VerticalElevatorGlyph : public OL_ElevatorGlyph {
public:
    OL_VerticalElevatorGlyph(
        OL_Style*, boolean abbreviated = false, boolean up = false,
        boolean down = false, boolean dimpled = false
    );
    virtual ~OL_VerticalElevatorGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;

    virtual void set_forward(boolean);
    virtual void set_backward(boolean);
    virtual void set_abbreviated(boolean);
private:
    boolean up_;
    boolean down_;
};

class OL_CheckBoxGlyph : public Glyph {
public:
    OL_CheckBoxGlyph(
        OL_Style*, boolean checked, boolean invoked, boolean inactive
    );
    virtual ~OL_CheckBoxGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
private:
    boolean checked_;
    boolean invoked_;
    boolean inactive_;
    OL_Style* style_;
    Coord width_;
    Coord height_;
};

class OL_VerticalMenuMarkGlyph : public Glyph {
public:
    OL_VerticalMenuMarkGlyph(OL_Style*);
    virtual ~OL_VerticalMenuMarkGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
private:
    OL_Style* style_;
    Coord width_;
    Coord height_;
};

class OL_HorizontalMenuMarkGlyph : public Glyph {
public:
    OL_HorizontalMenuMarkGlyph(OL_Style*);
    virtual ~OL_HorizontalMenuMarkGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
private:
    OL_Style* style_;
    Coord width_;
    Coord height_;
};

class OL_DimpleGlyph : public Glyph {
public:
    OL_DimpleGlyph(OL_Style*, boolean in);
    virtual ~OL_DimpleGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void set_in(boolean);
private:
    OL_Style* style_;
    boolean in_;
    Coord width_;
    Coord height_;
};

class OL_AnchorGlyph : public Glyph {
public:
    virtual ~OL_AnchorGlyph();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
//    virtual void draw(Canvas*, const Allocation&) const = 0;
    virtual void set_in(boolean);
protected:
    OL_AnchorGlyph(OL_Style*, boolean in = false);

    OL_Style* style_;
    boolean in_;
    Coord width_;
    Coord height_;
};

class OL_LRAnchorGlyph : public OL_AnchorGlyph {
public:
    OL_LRAnchorGlyph(OL_Style*, boolean in = false);
    virtual ~OL_LRAnchorGlyph();

    virtual void draw(Canvas*, const Allocation&) const;
};

class OL_TBAnchorGlyph : public OL_AnchorGlyph {
public:
    OL_TBAnchorGlyph(OL_Style*, boolean in = false);
    virtual ~OL_TBAnchorGlyph();

    virtual void draw(Canvas*, const Allocation&) const;
};

class OL_TitleBarGlyph : public Glyph {
public:
    OL_TitleBarGlyph(OL_Style*, Coord margin);
    ~OL_TitleBarGlyph();

    virtual void request(Requisition&) const;
    virtual void draw(Canvas*, const Allocation&) const;
private:
    OL_Style* style_;
    Coord margin_;
};

#endif

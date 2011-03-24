/*
 * planar figures
 */

#ifndef figure_h
#define figure_h

#include <InterViews/glyph.h>
#include <OS/list.h>
#include "globals.h"

class Brush;
class Color;
class Font;
class Graphic;
class PolyGlyph;
class String;
class Transformer;

declarePtrList(GraphicList, Graphic);

class Graphic : public Glyph {
public:
    Graphic(Graphic* gr = nil);
    virtual ~Graphic ();

    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void drawit(Canvas*);
    virtual boolean drawclipped(Canvas*, Coord, Coord, Coord, Coord);

    virtual Glyph* clone() const;
    virtual void flush();

    virtual Transformer* transformer();
    virtual void transformer(Transformer*);

    void eqv_transformer(Transformer&);
    void get_max_min(Coord&, Coord&, Coord&, Coord&);

    virtual void brush(const Brush*);
    virtual const Brush* brush();

    virtual void stroke(const Color*);
    virtual const Color* stroke();
    virtual void fill(const Color*);
    virtual const Color* fill();
    virtual void font(const Font*);
    virtual const Font* font();
    virtual void closed(boolean);
    virtual boolean closed();
    virtual void curved(boolean);
    virtual boolean curved();
    virtual int ctrlpts(Coord*&, Coord*&) const;
    virtual void ctrlpts(Coord*, Coord*, int);
    virtual Graphic* parent();
    virtual void parent(Graphic*);
    virtual Graphic* root();

    virtual void translate(Coord dx, Coord dy);
    virtual void scale(Coord sx, Coord sy, Coord ctrx = 0.0, Coord ctry = 0.0);
    virtual void rotate(float angle, Coord ctrx = 0.0, Coord ctry = 0.0);
    virtual void align(Alignment, Graphic*, Alignment);

    virtual void recompute_shape();
    virtual void getbounds(Coord&, Coord&, Coord&, Coord&);
    virtual void getcenter(Coord&, Coord&);    
    virtual boolean contains(PointObj&);
    virtual boolean intersects(BoxObj&);

    virtual void uncacheParents();
    virtual void uncacheChildren();
    virtual void uncacheExtent();
    virtual void invalidateCaches();

    virtual void undraw();
    virtual void append_(Graphic*);
    virtual void prepend_(Graphic*);
    virtual void insert_(GlyphIndex, Graphic*);
    virtual void remove_(GlyphIndex);
    virtual void remove_(Graphic*);
    virtual void remove_all_();
    virtual void replace_(GlyphIndex, Graphic*);
    virtual void change_(GlyphIndex);

    virtual GlyphIndex count_() const;
    virtual Graphic* child_(GlyphIndex) const;
    virtual void modified_(GlyphIndex);

    virtual long first_containing(PointObj&, Graphic*&);
    virtual long last_containing(PointObj&, Graphic*&);

    virtual long first_intersecting(BoxObj&, Graphic*&);
    virtual long last_intersecting(BoxObj&, Graphic*&);

    virtual long first_within(BoxObj&, Graphic*&);
    virtual long last_within(BoxObj&, Graphic*&);

    virtual Graphic& operator = (Graphic&);

    void add_point (Coord x, Coord y);
    void add_curve (Coord x, Coord y, Coord x1, Coord y1, Coord x2, Coord y2);
    void Bspline_move_to (
        Coord x, Coord y, Coord x1, Coord y1, Coord x2, Coord y2
    );
    void Bspline_curve_to (
        Coord x, Coord y, Coord x1, Coord y1, Coord x2, Coord y2
    );

    virtual void total_gs (Graphic& gs);
    virtual void draw_gs(Canvas*, Graphic*);
    virtual boolean drawclipped_gs(
        Canvas*, Coord, Coord, Coord, Coord, Graphic*
    );
    virtual void getextent_gs(
        Coord&, Coord&, Coord&, Coord& ,float& ,Graphic* gs
    );
    virtual boolean contains_gs(PointObj&, Graphic* gs);
    virtual boolean intersects_gs(BoxObj&, Graphic* gs);
    virtual void getbounds_gs(Coord&, Coord&, Coord&, Coord&, Graphic* gs);

    void alpha_stroke(boolean);
    boolean alpha_stroke();

    void alpha_fill(boolean);
    boolean alpha_fill();
protected:
    Graphic (
        const Brush* brush, const Color* stroke, const Color* fill,
        const Font* font, boolean closed, boolean curved, int coords, 
        Transformer*
    );

    void parentXform(Transformer& t);
    
    virtual void concat_gs(Graphic* a, Graphic* b, Graphic* dest);
    virtual void concatXform(
        Transformer* a, Transformer* b, Transformer* dest
    );
    virtual void concat(Graphic* a, Graphic* b, Graphic* dest);

/*   Helpers   */

    virtual boolean contains_(Graphic*, PointObj&, Graphic* gs);
    virtual boolean intersects_(Graphic*, BoxObj&, Graphic* gs);
    virtual void getbounds_(
        Graphic*, Coord&, Coord&, Coord&, Coord&, Graphic* gs
    );
    void total_gs_(Graphic*, Graphic& gs);
    void concatgs_(Graphic*, Graphic*, Graphic*, Graphic*);
    void concatXform_(Graphic*, Transformer*, Transformer*, Transformer*);
    void concat_(Graphic*, Graphic*, Graphic*, Graphic*);
    void getextent_(Graphic*, Coord&, Coord&, Coord&, Coord&, float&,Graphic*);

    void draw_(Graphic*, Canvas*, Graphic*);
    boolean drawclipped_(
        Graphic*, Canvas*, Coord, Coord, Coord, Coord, Graphic*
    );
    void transform_(Coord, Coord, Coord&, Coord&, Graphic*);
protected:
    const Brush* _brush;

    const Color* _stroke;
    const Color* _fill;

    const Color* _alpha_stroke;
    const Color* _alpha_fill;

    const Font* _font;
    Transformer*  _t;

    boolean _a_stroke;
    boolean _a_fill;

    boolean _closed;
    boolean _curved;
    int _ctrlpts;
    int _buf_size;
    Coord* _x;
    Coord* _y;

    Coord _xmin;
    Coord _xmax;
    Coord _ymin;
    Coord _ymax;

    Graphic* _parent;
    Extent* _cache;
};

class PolyGraphic : public Graphic {
public:
    PolyGraphic(Graphic* = nil) ;
    virtual ~PolyGraphic();
    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);

    virtual void undraw();

    virtual void append_(Graphic*);
    virtual void prepend_(Graphic*);
    virtual void insert_(GlyphIndex, Graphic*);
    virtual void remove_(GlyphIndex);
    virtual void remove_(Graphic*);
    virtual void remove_all_();
    virtual void replace_(GlyphIndex, Graphic*);
    virtual void change_(GlyphIndex);

    virtual GlyphIndex count_() const;
    virtual Graphic* child_(GlyphIndex) const;

    virtual void modified_(GlyphIndex);
    virtual void flush();
    virtual Glyph* clone() const;

    virtual long first_containing(PointObj&, Graphic*&);
    virtual long last_containing(PointObj&, Graphic*&);

    virtual long first_intersecting(BoxObj&, Graphic*&);
    virtual long last_intersecting(BoxObj&, Graphic*&);

    virtual long first_within(BoxObj&, Graphic*&);
    virtual long last_within(BoxObj&, Graphic*&);
protected:
    virtual void draw_gs(Canvas*, Graphic*);
    virtual boolean drawclipped_gs(
        Canvas*, Coord, Coord, Coord, Coord, Graphic*
    );
    virtual boolean contains_gs(PointObj&, Graphic*);
    virtual boolean intersects_gs(BoxObj&, Graphic*);
    virtual void getextent_gs(
        Coord&, Coord&, Coord&, Coord&, float&, Graphic* gs
    );

protected:
    PolyGlyph* _body;
};

class RootGraphic : public PolyGraphic {
public:
    RootGraphic(Graphic* = nil, const Color* bg = nil);
    virtual ~RootGraphic();

    void background(const Color*);
    const Color* background();

    virtual void undraw();
    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual void translate(Coord dx, Coord dy);
    virtual Glyph* clone() const;
    
    virtual void append_(Graphic*);
    virtual void prepend_(Graphic*);
    virtual void insert_(GlyphIndex, Graphic*);
    virtual void remove_(GlyphIndex);
    virtual void remove_(Graphic*);
    virtual void replace_(GlyphIndex, Graphic*);
protected:
    virtual boolean drawclipped_gs(
        Canvas*, Coord, Coord, Coord, Coord, Graphic*
    );
protected:
    const Color* _bg;
    Allocation _a;
};
inline const Color* RootGraphic::background () { return _bg; }

class Line : public Graphic {
public:
    Line (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord x1, Coord y1, Coord x2, Coord y2, Transformer*
    );
    virtual Glyph* clone() const;
    
protected:
    virtual ~Line ();
};

class Rectangle : public Graphic {
public:
    Rectangle (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord l, Coord b, Coord r, Coord t, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    virtual ~Rectangle ();
};

class Circle : public Graphic {
public:
    Circle (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord x, Coord y, Coord r, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    virtual ~Circle ();
};

class Ellipse : public Graphic {
public:
    Ellipse (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord x, Coord y, Coord rx, Coord ry, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    virtual ~Ellipse ();
};

class Open_BSpline : public Graphic {
public:
    Open_BSpline (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord* x, Coord* y, int ctrlpts, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    Open_BSpline(Open_BSpline*);
    virtual ~Open_BSpline ();
};

class Closed_BSpline : public Graphic {
public:
    Closed_BSpline (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord* x, Coord* y, int ctrlpts, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    Closed_BSpline(Closed_BSpline*);
    virtual ~Closed_BSpline ();
};

class Polyline : public Graphic {
public:
    Polyline (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord* x, Coord* y, int ctrlpts, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    virtual ~Polyline ();
};

class Polygon : public Graphic {
public:
    Polygon (
        const Brush* brush, const Color* stroke, const Color* fill,
        Coord* x, Coord* y, int ctrlpts, Transformer*
    );
    virtual Glyph* clone() const;

protected:
    virtual ~Polygon ();
};

class Text : public Graphic {
public:
    Text (
        const Font* font, const Color* stroke, const char*, Transformer*
    );
    virtual void text(const char*);
    virtual const char* text();
    virtual void draw(Canvas*, const Allocation&) const;
    virtual void request(Requisition&) const;
    virtual void allocate(Canvas*, const Allocation&, Extension&);
    virtual Glyph* clone() const;

protected:
    virtual ~Text();
    
    virtual void getextent_gs (Coord&, Coord&, Coord&, Coord&,float&,Graphic*);
    virtual void draw_gs(Canvas*, Graphic*);

    void init();
protected:
    String* _text;
    Allocation _a;
    PolyGlyph* _body;
};

class MpegVideo : public Graphic {
public:
    MpegVideo (const char* file);
};

#endif

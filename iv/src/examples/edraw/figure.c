/*
 * planar figures
 */

#include "figure.h"

#include <InterViews/brush.h>
#include <InterViews/canvas.h>
#include <InterViews/character.h>
#include <InterViews/color.h>
#include <InterViews/cursor.h>
#include <InterViews/font.h>
#include <InterViews/geometry.h>
#include <InterViews/layout.h>
#include <InterViews/transformer.h>

#include <IV-X11/xcanvas.h>

#include <OS/math.h>
#include <OS/string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static const int buf_size = 10;

implementPtrList(GraphicList, Graphic);

/*
static int count = 0;

static boolean no_events(const Event& last_e) {
    Event e;
    e.window(last_e.window());
    Session* session = Session::instance();
    session->poll(e);
    if (
        Math::abs(e.pointer_x()-last_e.pointer_x()) > 0.5 ||
        Math::abs(e.pointer_x()-last_e.pointer_x()) > 0.5
    ) {
        return false;
    } else {
        return true;
    }
}
*/

static void corners(
    Coord& left, Coord& bottom, Coord& right, Coord& top, const Transformer& t
) {
    Coord x1, y1, x2, y2, x3, y3, x4, y4;
    
    t.transform(left, bottom, x1, y1);
    t.transform(left, top, x2, y2);
    t.transform(right, top, x3, y3);
    t.transform(right, bottom, x4, y4);
    left = Math::min(x1, x2, x3, x4);
    bottom = Math::min(y1, y2, y3, y4);
    right = Math::max(x1, x2, x3, x4);
    top = Math::max(y1, y2, y3, y4);
}    

Graphic::Graphic (Graphic* gr) {
    if (gr != nil) {
        _brush = gr->brush();
        Resource::ref(_brush);
        _stroke = gr->stroke();
        Resource::ref(_stroke);
        _fill = gr->fill();
        Resource::ref(_fill);
        _font = gr->font();
        Resource::ref(_font);
        _closed = gr->closed();
        _curved = gr->curved();
        
        _ctrlpts = 0;
        _x = new Coord[buf_size];
        _y = new Coord[buf_size];
        _buf_size = buf_size;
        if (gr->transformer() != nil) {
            _t = new Transformer;
            *_t = *gr->transformer();
        } else {
            _t = nil;
        }
    } else {
        _brush = nil;
        _stroke = nil;
        _fill = nil;
        _font = nil;
        _closed = false;
        _curved = false;
        _ctrlpts = 0;
        _x = new Coord[buf_size];
        _y = new Coord[buf_size];
        _buf_size = buf_size;
        _t = nil;
    } 
    _xmin = _xmax = _ymin = _ymax = 0.0;
    _parent = nil;

    _alpha_stroke = nil;
    _alpha_fill = nil;

    _a_stroke = false;
    _a_fill = false;

    _cache = nil;
}

Graphic::Graphic (
    const Brush* brush, const Color* stroke, const Color* fill,
    const Font* font, boolean closed, boolean curved, int coords, 
    Transformer* t
) {
    _brush = brush;
    Resource::ref(_brush);
    _stroke = stroke;
    Resource::ref(_stroke);
    _fill = fill;
    Resource::ref(_fill);
    _font = font;
    Resource::ref(_font);
    _closed = closed;
    _curved = curved;
    _ctrlpts = 0;
    if (coords > 0) {
        _x = new Coord[coords];
        _y = new Coord[coords];
    } else {
        _x = nil;
        _y = nil;
    }
    _buf_size = coords;
    _t = nil;
    if (t != nil) {
        _t = new Transformer;
        *_t = *t;
    }
    _parent = nil;

    _alpha_stroke = nil;
    _alpha_fill = nil;

    _a_stroke = false;
    _a_fill = false;

    _xmin = _xmax = _ymin = _ymax = 0.0;
    _cache = nil;
}

Graphic::~Graphic () {
    Resource::unref(_brush);
    Resource::unref(_stroke);
    Resource::unref(_fill);
    Resource::unref(_alpha_stroke);
    Resource::unref(_alpha_fill);
    Resource::unref(_font);
    Resource::unref(_t);
    delete _x;
    delete _y;
    delete _cache;
}

void Graphic::get_max_min (Coord& l, Coord& b, Coord& r, Coord& t) {
    l = _xmin;
    b = _ymin;
    r = _xmax;
    t = _ymax;
}

int Graphic::ctrlpts (Coord*& x, Coord*& y) const{
    x = _x;
    y = _y;
    return _ctrlpts;
}

void Graphic::ctrlpts (Coord* x, Coord* y, int count) {
    delete _x;
    delete _y;
    int size = max(count, buf_size);
    _x = new Coord[size];
    _y = new Coord[size];
    _ctrlpts = count;
    for (int i = 0; i < count; i++) {
        _x[i] = x[i];
        _y[i] = y[i];
    }
    recompute_shape();
}

void Graphic::flush () {}

Glyph* Graphic::clone () const { return nil; }

Transformer* Graphic::transformer() { return _t; }

/*   To be replaced by templates  */

void Graphic::transformer(Transformer* t) {
    Resource::ref(t);
    Resource::unref(_t);
    _t = t;
    uncacheParents();
}

const Brush* Graphic::brush() { return _brush; }

void Graphic::brush(const Brush* b) { 
    Resource::ref(b);
    Resource::unref(_brush);
    _brush = b;
    invalidateCaches();
}

boolean Graphic::alpha_stroke () { return _a_stroke; }

void Graphic::alpha_stroke (boolean a_stroke) { 
    _a_stroke = a_stroke; 
}

boolean Graphic::alpha_fill () { return _a_fill; }

void Graphic::alpha_fill (boolean a_fill) { 
    _a_fill = a_fill; 
}

const Color* Graphic::stroke() { return _stroke; }

void Graphic::stroke(const Color* s) {
    Resource::ref(s);
    Resource::unref(_stroke);
    _stroke = s;
}

const Color* Graphic::fill() { return _fill; }

void Graphic::fill(const Color* f) {
    Resource::ref(f);
    Resource::unref(_fill);
    _fill = f;
}

const Font* Graphic::font() { return _font; }

void Graphic::font(const Font* f) {
    Resource::ref(f);
    Resource::unref(_font);
    _font = f;
    invalidateCaches();
}

void Graphic::closed (boolean c) { _closed = c; }

boolean Graphic::closed () { return _closed; }

void Graphic::curved (boolean c) { _curved = c; }

boolean Graphic::curved () { return _curved; }

void Graphic::parent (Graphic* p) { _parent = p; }

Graphic* Graphic::parent () { return _parent; }

Graphic* Graphic::root () {
    Graphic* cur, *parent = this;

    do {
        cur = parent;
        parent = cur->parent();
    } while (parent != nil);

    return cur;
}

void Graphic::total_gs (Graphic& gs) {
    Graphic* p = parent();

    if (p == nil) {
        concat(nil, this, &gs);

    } else {
        p->total_gs(gs);
        concat(this, &gs, &gs);
    }
}

void Graphic::translate (float dx, float dy) { 
    if (dx != 0 || dy != 0) {
	if (_t == nil) {
	    _t = new Transformer;
	}
	_t->translate(dx, dy);
        uncacheParents();
    }
}

void Graphic::scale (float sx, float sy, float cx, float cy) {
    float ncx, ncy;

    if (sx != 1.0 || sy != 1.0) {
	if (_t == nil) {
	    _t = new Transformer;
	}
	Transformer parents;
	parentXform(parents);
	parents.InvTransform(cx, cy, ncx, ncy);
	
	if (ncx != 0 || ncy != 0) {
	    _t->translate(-ncx, -ncy);
	    _t->scale(sx, sy);
	    _t->translate(ncx, ncy);
	} else {
	    _t->scale(sx, sy);
	}
        uncacheParents();
    }
}

void Graphic::rotate (float angle, float cx, float cy) {
    float mag = (angle < 0) ? -angle : angle;
    float ncx, ncy;

    if ((mag - int(mag)) != 0 || int(mag)%360 != 0) {
	if (_t == nil) {
	    _t = new Transformer;
	}
	Transformer parents;
	parentXform(parents);
	parents.InvTransform(cx, cy, ncx, ncy);
	
	if (ncx != 0 || ncy != 0) {
	    _t->translate(-ncx, -ncy);
	    _t->rotate(angle);
	    _t->translate(ncx, ncy);
	} else {
	    _t->rotate(angle);
	}
        uncacheParents();
    }
}

void Graphic::align (Alignment falign, Graphic* moved, Alignment malign) {
    float fx0, fy0, fx1, fy1, mx0, my0, mx1, my1, dx = 0, dy = 0;

    getbounds(fx0, fy0, fx1, fy1);
    moved->getbounds(mx0, my0, mx1, my1);
    
    switch (falign) {
	case BottomLeft:
	case CenterLeft:
	case TopLeft:
	case Left:
	    dx = fx0;
	    break;
	case BottomCenter:
	case Center:
	case TopCenter:
	case HorizCenter:
	    dx = (fx0 + fx1 + 1)/2;
	    break;
	case BottomRight:
	case CenterRight:
	case TopRight:
	case Right:
	    dx = fx1 + 1;
	    break;
    }
    switch (falign) {
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
	case Bottom:
	    dy = fy0;
	    break;
	case CenterLeft:
	case Center:
	case CenterRight:
	case VertCenter:
	    dy = (fy0 + fy1 + 1)/2;
	    break;
	case TopLeft:
	case TopCenter:
	case TopRight:
	case Top:
	    dy = fy1 + 1;
	    break;
    }
    
    switch (malign) {
	case BottomLeft:
	case CenterLeft:
	case TopLeft:
	case Left:
	    dx -= mx0;
	    break;	
	case BottomCenter:
	case Center:
	case TopCenter:
	case HorizCenter:
	    dx -= (mx0 + mx1 + 1)/2;
	    break;
	case BottomRight:
	case CenterRight:
	case TopRight:
	case Right:
	    dx -= (mx1 + 1);
	    break;
    }
    switch (malign) {
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
	case Bottom:
	    dy -= my0;
	    break;
	case CenterLeft:
	case Center:
	case CenterRight:
	case VertCenter:
	    dy -= (my0 + my1 + 1)/2;
	    break;
	case TopLeft:
	case TopCenter:
	case TopRight:
	case Top:
	    dy -= (my1 + 1);
	    break;
    }
    if (dx != 0 || dy != 0) {
        Transformer parents;
	moved->parentXform(parents);

        parents.Invert();
	parents.Transform(0.0, 0.0, fx0, fy0);
	parents.Transform(dx, dy, mx0, my0);

	moved->translate(mx0-fx0, my0-fy0);
    }
}

void Graphic::recompute_shape () {
    if (_ctrlpts == 0) {
        return;
    } else {
        _xmin = _xmax = _x[0];
        _ymin = _ymax = _y[0];
    }
    for (int i = 1; i < _ctrlpts; i++) {
        _xmin = Math::min(_xmin, _x[i]);
        _xmax = Math::max(_xmax, _x[i]);
        _ymin = Math::min(_ymin, _y[i]);
        _ymax = Math::max(_ymax, _y[i]);
    }
}

void Graphic::getbounds (float& x0, float& y0, float& x1, float& y1) {
    Graphic gs;
        
    total_gs(gs);
    getbounds_gs(x0, y0, x1, y1, &gs);
}

void Graphic::getcenter (float& x, float& y) {
    Graphic gs;
    float l, b, tol;
    
    total_gs(gs);
    getextent_gs(l, b, x, y, tol, &gs);
}    

boolean Graphic::contains (PointObj& p) {
    if (parent() == nil) {
        return contains_gs(p, this);
        
    } else {
        Graphic gs;
        total_gs(gs);
        return contains_gs(p, &gs);
    }
}

boolean Graphic::intersects (BoxObj& b) {
    if (parent() == nil) {
        return intersects_gs(b, this);

    } else {
	Graphic gs;
	total_gs(gs);
	return intersects_gs(b, &gs);
    }
}

void Graphic::uncacheExtent () { 
    delete _cache;
    _cache = nil;
}

void Graphic::uncacheChildren () { 
    for (long i = 0; i < count(); i++) {
        child_(i)->uncacheExtent();
        child_(i)->uncacheChildren();
    }
}

void Graphic::uncacheParents () {
    Graphic* p = parent();

    if (p != nil) {
        p->uncacheExtent();
        p->uncacheParents();
    }
}

void Graphic::invalidateCaches() {
    uncacheParents();
    uncacheExtent();
    uncacheChildren();
}

void Graphic::undraw () {}
void Graphic::append_(Graphic*) {}
void Graphic::prepend_(Graphic*) {}
void Graphic::insert_(GlyphIndex, Graphic*) {}
void Graphic::remove_(GlyphIndex) {}
void Graphic::remove_(Graphic*) {}
void Graphic::remove_all_() {}
void Graphic::replace_(GlyphIndex, Graphic*) {}
void Graphic::change_(GlyphIndex) {}
GlyphIndex Graphic::count_ () const { return 0; }
Graphic* Graphic::child_ (GlyphIndex) const { return nil; }
void Graphic::modified_ (GlyphIndex) {}

long Graphic::first_containing (PointObj&, Graphic*&) { return -1; }
long Graphic::last_containing (PointObj&, Graphic*&) { return -1; }
long Graphic::first_intersecting (BoxObj&, Graphic*&) { return -1; }
long Graphic::last_intersecting (BoxObj&, Graphic*&) { return -1; }
long Graphic::first_within (BoxObj&, Graphic*&) { return -1; }
long Graphic::last_within (BoxObj&, Graphic*&) { return -1; }

Graphic& Graphic::operator = (Graphic& g) {
    brush(g.brush());
    stroke(g.stroke());
    fill(g.fill());
    font(g.font());
    alpha_stroke(g.alpha_stroke() || alpha_stroke());
    alpha_fill(g.alpha_fill() || alpha_fill());

    if (g._t == nil) {
        Resource::unref(_t);
        _t = nil;

    } else {
	if (_t == nil) {
	    _t = new Transformer(g._t);
	} else {
	    *_t = *g._t;
	}
    }
    invalidateCaches();
    return *this;
}

void Graphic::add_point(Coord x, Coord y) {
    if (_ctrlpts == 0) {
        _xmin = x - 1;
        _xmax = x + 1;
        _ymin = y - 1;
        _ymax = y + 1;
    } else {
        _xmin = Math::min(_xmin, x);
        _xmax = Math::max(_xmax, x);
        _ymin = Math::min(_ymin, y);
        _ymax = Math::max(_ymax, y);
    }
    _x[_ctrlpts] = x;
    _y[_ctrlpts] = y;
    _ctrlpts += 1;
    if (_ctrlpts >= _buf_size) {
        _buf_size = buf_size + _buf_size;
        Coord* x = new Coord[_buf_size];
        Coord* y = new Coord[_buf_size];
        Memory::copy(_x, x, _ctrlpts*sizeof(int));
        Memory::copy(_y, y, _ctrlpts*sizeof(int));
        delete _x;
        delete _y;
        _x = x;
        _y = y;
    }
}

void Graphic::add_curve(
    Coord x, Coord y, Coord x1, Coord y1, Coord x2, Coord y2
) {
    add_point(x1, y1);
    add_point(x2, y2);
    add_point(x, y);
}

void Graphic::Bspline_move_to (
    Coord x, Coord y, Coord x1, Coord y1, Coord x2, Coord y2
) {
    Coord p1x = (x + x + x1) / 3;
    Coord p1y = (y + y + y1) / 3;
    Coord p2x = (x + x + x2) / 3;
    Coord p2y = (y + y + y2) / 3;
    add_point((p1x + p2x) / 2, (p1y + p2y) / 2);
}

void Graphic::Bspline_curve_to (
    Coord x, Coord y, Coord x1, Coord y1, Coord x2, Coord y2
) {
    Coord p1x = (x + x + x1) / 3;
    Coord p1y = (y + y + y1) / 3;
    Coord p2x = (x + x + x2) / 3;
    Coord p2y = (y + y + y2) / 3;
    Coord p3x = (x1 + x1 + x) / 3;
    Coord p3y = (y1 + y1 + y) / 3;
    add_curve((p1x + p2x) / 2, (p1y + p2y) / 2, p3x, p3y, p1x, p1y);
}

void Graphic::request(Requisition& req) const {
    if (_ctrlpts > 0) {
        Coord left = _xmin, bottom = _ymin;
        Coord right = _xmax, top = _ymax;
        Requirement& rx = req.x_requirement();
        Requirement& ry = req.y_requirement();

        if (_t != nil) {
            corners(left, bottom, right, top, *_t);
        } 
        
        rx.natural(right - left);
        rx.stretch(0.0);
        rx.shrink(0.0);
        rx.alignment(-left / rx.natural());
        
        ry.natural(top - bottom);
        ry.stretch(0.0);
        ry.shrink(0.0);
        ry.alignment(-bottom / ry.natural());
    }
}

void Graphic::allocate(Canvas* c, const Allocation&, Extension& ext) {
    if (_ctrlpts > 0) {
        Coord l, b, r, t;
        getbounds(l, b, r, t);
        ext.set_xy(c, l, b, r, t);
    }
}

void Graphic::draw(Canvas* c, const Allocation&) const {
    if (c != nil) {
        Graphic* gr = (Graphic*) this;
        CanvasDamage& cd = c->rep()->damage_;
        gr->drawclipped(
            c, cd.left, cd.bottom, cd.right, cd.top
        );
    }
}

void Graphic::drawit (Canvas* c) {
    if (parent() == nil) {
	draw_gs(c, this);

    } else {
	Graphic gs;
	total_gs(gs);
	draw_gs(c, &gs);
    }
}    

boolean Graphic::drawclipped (Canvas* c, Coord l, Coord b, Coord r, Coord t) {
    if (parent() == nil) {
        return drawclipped_gs(c, l, b, r, t, this);
    } else {
        Graphic gs;
        total_gs(gs);
        return drawclipped_gs(c, l, b, r, t, &gs);
    }
}

void Graphic::draw_gs (Canvas* c, Graphic* gs) {
    const Brush* brush = gs->brush();
    const Color* stroke = gs->stroke();
    const Color* fill = gs->fill();

    if (stroke != nil && _alpha_stroke == nil) {
        _alpha_stroke = new Color(*stroke, 0.7);
        _alpha_stroke->ref();
    }
    
    if (fill != nil && _alpha_fill == nil) {
        _alpha_fill = new Color(*fill, 0.7);
        _alpha_fill->ref();
    }

    if (gs->alpha_stroke()) {
        stroke = _alpha_stroke;
    }
    if (gs->alpha_fill()) {
        fill = _alpha_fill;
    }

    Transformer* tx = gs->transformer();
    if (tx != nil) {
        c->push_transform();
        c->transform(*tx);
    }
    c->new_path();
    c->move_to(_x[0], _y[0]);
    if (_curved) {
        for (int i = 1; i < _ctrlpts; i += 3) {
            c->curve_to(
                _x[i + 2], _y[i + 2],
                _x[i], _y[i],
                _x[i + 1], _y[i + 1]
            );
        }
    } else {
        for (int i = 1; i < _ctrlpts; ++i) {
            c->line_to(_x[i], _y[i]);
        }
    }
    if (_closed) {
        c->close_path();
    }
    if (fill != nil) {
        c->fill(fill);
    }
    if (brush != nil && stroke != nil) {
        c->stroke(stroke, brush);
    }
    if (tx != nil) {
        c->pop_transform();
    }
}

boolean Graphic::drawclipped_gs (
    Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    Coord ll, bb, rr, tt;
    getbounds_gs(ll, bb, rr, tt, gs);

    BoxObj thisBox(ll, bb, rr, tt);
    BoxObj clipBox(l, b, r, t);
    if (clipBox.Intersects(thisBox)) {
	draw_gs(c, gs);
        return true;
    } else {
        return false;
    }
}

void Graphic::eqv_transformer (Transformer& total) {
    Graphic* p = parent();
    
    if (p == nil) {
        concatXform(nil, _t, &total);

    } else {
        p->eqv_transformer(total);
        concatXform(_t, &total, &total);
    }
}

void Graphic::getextent_gs (
    Coord& l, Coord& b, Coord& cx, Coord& cy, float& tol, Graphic* gs
) {
    tol = 1.0;
    const Brush* br = gs->brush();
    if (br != nil) {
        float width = float(br->Width());
        tol = (width > 1) ? width : tol;
    }
    if (_ctrlpts > 0) {
        Coord left = _xmin, bottom = _ymin;
        Coord right = _xmax, top = _ymax;
        Transformer* t = gs->transformer();

        if (t != nil) {
            corners(left, bottom, right, top, *t);
        } 
        l = left;
        b = bottom;
        cx = (left + right)/2.0;
        cy = (top + bottom)/2.0;
    }
}

void Graphic::getbounds_gs (
    Coord& l, Coord& b, Coord& r, Coord& t, Graphic* gs
) {
    float tol;
    
    getextent_gs(l, b, r, t, tol, gs);
    r += r - l;
    t += t - b;
    l -= tol;
    b -= tol;
    r += tol;
    t += tol;
}

static void invXform_gs (Coord& tx, Coord& ty, Graphic* g) {
    Transformer* t = g->transformer();
    if (t != nil) {
        t->inverse_transform(tx, ty);
    }
}

static void Xform_gs(
    Coord x[], Coord y[], int n, Coord tx[], Coord ty[], Graphic* g
) {
    Transformer* t = g->transformer();
    if (t != nil) {
        register Coord* ox, * oy, *nx, *ny;
        Coord* lim;
        
        lim = &x[n];
        for (
            ox = x, oy = y, nx = tx, ny = ty; ox < lim; ox++, oy++, nx++, ny++
        ) {
            t->transform(*ox, *oy, *nx, *ny);
        }
    } else {
        Memory::copy(x, tx, n*sizeof(Coord));
        Memory::copy(y, ty, n*sizeof(Coord));
    }
}

boolean Graphic::contains_gs (PointObj& po, Graphic* gs) { 
    PointObj pt (&po);
    Coord ll, bb, rr, tt;
    getbounds_gs(ll, bb, rr, tt, gs);
    BoxObj b(ll, bb, rr, tt);

    if (!_curved && !_fill) {
        if (b.Contains(po)) {
            MultiLineObj ml (_x, _y, _ctrlpts);
            invXform_gs(pt._x, pt._y, gs);
            if (_closed) {
                LineObj l (_x[_ctrlpts - 1], _y[_ctrlpts - 1], *_x, *_y);
                return ml.Contains(pt) || l.Contains(pt);
            } else {
                return ml.Contains(pt);
            }
        }
        return false;

    } else if (!_curved && _fill) {
        if (b.Contains(pt)) {
            FillPolygonObj fp (_x, _y, _ctrlpts);
            invXform_gs(pt._x, pt._y, gs);
            return fp.Contains(pt);
        }
        return false;

    } else if (_curved && !_fill) {
        if (b.Contains(pt)) {
            MultiLineObj ml;
            if (_closed) {
                ml.ClosedSplineToPolygon(_x, _y, _ctrlpts);
            } else {
                ml.SplineToMultiLine(_x, _y, _ctrlpts);
            }
            invXform_gs(pt._x, pt._y, gs);
            return ml.Contains(pt);
        }
        return false;

    } else {
        if (b.Contains(pt)) {
            FillPolygonObj fp;
            fp.ClosedSplineToPolygon(_x, _y, _ctrlpts);
            invXform_gs(pt._x, pt._y, gs);
            return fp.Contains(pt);
        }
        return false;
    }
}

boolean Graphic::intersects_gs (BoxObj& userb, Graphic* gs) { 
    Coord* convx, *convy;
    Coord ll, bb, rr, tt;
    getbounds_gs(ll, bb, rr, tt, gs);
    BoxObj b(ll, bb, rr, tt);;
    boolean result = false;

    if (!_curved && !_fill) {
        if (b.Intersects(userb)) {
            convx = new Coord[_ctrlpts+1];
            convy = new Coord[_ctrlpts+1];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            if (_closed) {
                convx[_ctrlpts] = *convx;
                convy[_ctrlpts] = *convy;
                MultiLineObj ml(convx, convy, _ctrlpts+1);
                result = ml.Intersects(userb);
            } else {
                MultiLineObj ml(convx, convy, _ctrlpts);
                result = ml.Intersects(userb);
            }
            delete convx;
            delete convy;
        }
        return result;

    } else if (!_curved && _fill) {
        if (b.Intersects(userb)) {
            convx = new Coord[_ctrlpts];
            convy = new Coord[_ctrlpts];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            FillPolygonObj fp (convx, convy, _ctrlpts);
            result = fp.Intersects(userb);
            delete convx;
            delete convy;
        }
        return result;    

    } else if (_curved && !_fill) {
        if (b.Intersects(userb)) {
            convx = new Coord[_ctrlpts];
            convy = new Coord[_ctrlpts];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            MultiLineObj ml;
            if (_closed) {
                ml.ClosedSplineToPolygon(convx, convy, _ctrlpts);
            } else {
                ml.SplineToMultiLine(convx, convy, _ctrlpts);
            }
            result = ml.Intersects(userb);
            delete convx;
            delete convy;
        }
        return result;

    } else {
        if (b.Intersects(userb)) {
            convx = new Coord[_ctrlpts];
            convy = new Coord[_ctrlpts];
            Xform_gs(_x, _y, _ctrlpts, convx, convy, gs);
            FillPolygonObj fp;
            fp.ClosedSplineToPolygon(convx, convy, _ctrlpts);
            result = fp.Intersects(userb);
            delete convx;
            delete convy;
        }
        return result;
    }
}

void Graphic::parentXform (Transformer& t) {
    Transformer identity;
    Graphic* p = parent();

    if (p == nil) {
        *(&t) = *(&identity);
    } else {
        p->eqv_transformer(t);
    }
}

void Graphic::concat_gs (Graphic* a, Graphic* b, Graphic* dest) {
    const Color* stroke, *fill;
    const Font* font;
    const Brush* br;
    
    if (a == nil) {
        *dest = *b;
        return;
    } else if (b == nil) {
        *dest = *a;
        return;
    }

    dest->alpha_stroke(a->alpha_stroke() || b->alpha_stroke());
    dest->alpha_fill(a->alpha_fill() || b->alpha_fill());

    if ((fill = b->fill()) == nil) {
	fill = a->fill();
    }
    dest->fill(fill);

    if ((stroke = b->stroke()) == nil) {
	stroke = a->stroke();
    }
    dest->stroke(stroke);

    if ((font = b->font()) == nil) {
	font = a->font();
    }
    dest->font(font);

    if ((br = b->brush()) == nil) {
	br = a->brush();
    }
    dest->brush(br);
}

void Graphic::concatXform (
    Transformer* a, Transformer* b, Transformer* dest
) {
    Transformer identity;
    if (a == nil) {
        *dest = (b == nil) ? *(&identity) : *b;

    } else if (b == nil) {
        *dest = *a;
        
    } else {
        Transformer tmp(a);
        tmp.Postmultiply(b);
        *dest = tmp;
    }
}

void Graphic::concat (Graphic* a, Graphic* b, Graphic* dest) {
    Transformer* ta = (a == nil) ? nil : a->transformer();
    Transformer* tb = (b == nil) ? nil : b->transformer();
    Transformer* td = dest->transformer();
    if (td == nil) {
        td = new Transformer;
    } else {
        Resource::ref(td);
    }
    concatXform(ta, tb, td);
    dest->transformer(td);
    concat_gs(a, b, dest);
    Resource::unref(td);
}

boolean Graphic::contains_ (Graphic* g, PointObj& p, Graphic* gs) {
    return g->contains_gs(p, gs);
}
boolean Graphic::intersects_ (Graphic* g, BoxObj& b, Graphic* gs) {
    return g->intersects_gs(b, gs);
}
void Graphic::getbounds_ (
    Graphic* g, Coord& l, Coord& b, Coord& r, Coord& t, Graphic* gs
) {
    g->getbounds_gs(l, b, r, t, gs);
}
void Graphic::total_gs_(Graphic* gr, Graphic& gs) {
    gr->total_gs(gs);
}
void Graphic::concatgs_ (Graphic* gr, Graphic* a, Graphic* b, Graphic* dest) {
    gr->concat_gs(a, b, dest);
}
void Graphic::concatXform_ (
    Graphic* gr, Transformer* a, Transformer* b, Transformer* dest
) {
    gr->concatXform(a, b, dest);
}
void Graphic::concat_ (Graphic* g, Graphic* a, Graphic* b, Graphic* d) {
    g->concat(a, b, d);
}
void Graphic::getextent_ (
    Graphic* gr,Coord& l, Coord& b, Coord& r, Coord& t, float& tol, Graphic* gs
) {
    gr->getextent_gs(l, b, r, t, tol, gs);
}
void Graphic::draw_ (Graphic* gr, Canvas* c, Graphic* gs) {
    gr->draw_gs(c, gs);
}
boolean Graphic::drawclipped_ (
    Graphic* gr, Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    return gr->drawclipped_gs(c, l, b, r, t, gs);
}
void Graphic::transform_ (Coord x, Coord y, Coord& tx, Coord& ty, Graphic* g) {
    Transformer* t = (g == nil) ? transformer() : g->transformer();

    if (t != nil) {
        t->Transform(x, y, tx, ty);
    } else {
        tx = x;
        ty = y;
    }
}

/************************************************************************/

PolyGraphic::PolyGraphic (Graphic* gr) : Graphic (gr) {
    LayoutKit* layout = LayoutKit::instance();
    _body = layout->overlay();
}

PolyGraphic::~PolyGraphic () {
    delete _body;
}

void PolyGraphic::request (Requisition& req) const { 
   _body->request(req);
    Requirement& rx = req.x_requirement();
    Requirement& ry = req.y_requirement();

    Coord left, bottom, right, top;
    left = -rx.natural()*rx.alignment();
    right = left + rx.natural();
    bottom = -ry.natural()*ry.alignment();
    top = bottom + ry.natural();

    if (_t != nil) {
        corners(left, bottom, right, top, *_t);
    }
    rx.natural(right - left);
    rx.stretch(0.0);
    rx.shrink(0.0);
    rx.alignment(-left / rx.natural());

    ry.natural(top - bottom);
    ry.stretch(0.0);
    ry.shrink(0.0);
    ry.alignment(-bottom / ry.natural());
}

void PolyGraphic::allocate (Canvas* c, const Allocation& a, Extension& ext) {
    if (_t != nil) {
        c->push_transform();
        c->transform(*_t);
    } 
    _body->allocate(c, a, ext);
    if (_t != nil) {
        c->pop_transform();
    }
}

void PolyGraphic::undraw () { _body->undraw(); }

void PolyGraphic::append_ (Graphic* g) { 
    _body->append(g); 
    g->parent(this);
    uncacheParents();
    uncacheExtent();
}

void PolyGraphic::prepend_ (Graphic* g) { 
    _body->prepend(g); 
    g->parent(this);
    uncacheParents();
    uncacheExtent();
}

void PolyGraphic::insert_ (GlyphIndex i, Graphic* g) { 
    _body->insert(i, g); 
    g->parent(this);
    uncacheParents();
    uncacheExtent();
}

void PolyGraphic::remove_ (GlyphIndex i) { 
    Graphic* g = (Graphic*)_body->component(i);
    if (g != nil) {
        g->parent(nil);
    }
    _body->remove(i); 
    uncacheParents();
    uncacheExtent();
}

void PolyGraphic::remove_ (Graphic* g) {
    for (int i = 0; i < count_(); i++) {
        if (child_(i) == g) {
            remove_(i);
            uncacheParents();
            uncacheExtent();
            break;
        }
    }
}

void PolyGraphic::remove_all_ () {
    uncacheParents();
    uncacheExtent();
    while (true) {
        long i = count_()-1;
        if (i >= 0) {
            _body->remove(i);
        } else {
            break;
        }
    }
}

void PolyGraphic::replace_ (GlyphIndex i, Graphic* g) { 
    Graphic* gr = (Graphic*) _body->component(i);
    gr->parent(nil);
    _body->replace(i, g); 
    g->parent(this);
    uncacheParents();
    uncacheExtent();
}

void PolyGraphic::change_ (GlyphIndex i) { _body->change(i); }

void PolyGraphic::modified_ (GlyphIndex i) { _body->modified(i); }

void PolyGraphic::flush () {
    GlyphIndex count = count_();
    
    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = child_(i);
        concat_(gr, gr, this, gr);
        gr->flush();
    }
    Graphic n;

    *((Graphic*) this) = *(&n);
    Resource::unref(_t);
    _t = nil;
}

Glyph* PolyGraphic::clone () const {
    Graphic* pg = new PolyGraphic((Graphic*)this);

    GlyphIndex count = _body->count();
    for (GlyphIndex i = 0; i < count; i++) {
        Glyph* gr = _body->component(i);
        pg->append(gr->clone());
    }
    return pg;
}
    
GlyphIndex PolyGraphic::count_ () const { return _body->count(); }

Graphic* PolyGraphic::child_ (GlyphIndex i) const { 
    return (Graphic*) _body->component(i);
}

long PolyGraphic::first_containing (PointObj& pt, Graphic*& target) {
    GlyphIndex count = _body->count();
    target = nil;

    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);

	if (gr->contains(pt)) {
	    target = gr;
            return i;
	}
    }
    return -1;
}

long PolyGraphic::last_containing (PointObj& pt, Graphic*& target) {
    GlyphIndex count = _body->count();
    target = nil;

    for (GlyphIndex i = count-1; i >= 0; i--) {
        Graphic* gr = (Graphic*) _body->component(i);

	if (gr->contains(pt)) {
	    target = gr;
            return i;
	}
    }
    return -1;
}

long PolyGraphic::first_intersecting (BoxObj& b, Graphic*& target) {
    GlyphIndex count = _body->count();
    target = nil;

    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);

	if (gr->intersects(b)) {
	    target = gr;
            return i;
	}
    }
    return -1;
}

long PolyGraphic::last_intersecting (BoxObj& b, Graphic*& target) {
    GlyphIndex count = _body->count();
    target = nil;

    for (GlyphIndex i = count-1; i >= 0; i--) {
        Graphic* gr = (Graphic*) _body->component(i);

	if (gr->intersects(b)) {
	    target = gr;
            return i;
	}
    }
    return -1;
}

long PolyGraphic::first_within (BoxObj& gb, Graphic*& target) {
    Coord l, b, r, t;

    GlyphIndex count = _body->count();
    target = nil;

    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        gr->getbounds(l, b, r, t);

        BoxObj box(l, b, r, t);
	if (box.Within(gb)) {
            target = gr;
            return i;
	}
    }
    return -1;
}

long PolyGraphic::last_within (BoxObj& gb, Graphic*& target) {
    Coord l, b, r, t;

    GlyphIndex count = _body->count();
    target = nil;

    for (GlyphIndex i = count-1; i >= 0; i--) {
        Graphic* gr = (Graphic*) _body->component(i);
        gr->getbounds(l, b, r, t);

        BoxObj box(l, b, r, t);
	if (box.Within(gb)) {
            target = gr;
            return i;
	}
    }
    return -1;
}

void PolyGraphic::draw_gs (Canvas* c, Graphic* gs) {
    Graphic gstemp;

    GlyphIndex count = _body->count();
    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
	concat_(gr, gr, gs, &gstemp);
	draw_(gr, c, &gstemp);
    }
}

boolean PolyGraphic::drawclipped_gs (
    Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    Graphic gstemp;
    boolean flag = true;
    
    GlyphIndex count = _body->count();
    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        concat_(gr, gr, gs, &gstemp);
        flag = drawclipped_(gr, c, l, b, r, t, &gstemp) || flag;
    }
    return flag;
}

void PolyGraphic::getextent_gs (
    Coord& l, Coord& b, Coord& cx, Coord& cy, float& tol, Graphic* gs
) {
    Extent e;
    l = b = cx = cy = tol = 0.0;
    if (_cache != nil) {
        e = *_cache;
    } else {
        if (count_() == 0) {
            return;
        } else {
            Graphic gstemp;
            Transformer ttemp;
            Extent te;
            gstemp.transformer(&ttemp);
            
            GlyphIndex count = _body->count();
            for (GlyphIndex i = 0; i < count; i++) {
                Graphic* gr = (Graphic*) _body->component(i);
                
                concatgs_(gr, gr, gs, &gstemp);
                concatXform_(gr, nil, gr->transformer(), &ttemp);
                getextent_(gr, te._l, te._b, te._cx, te._cy, te._tol, &gstemp);
                e.Merge(te);
            }
            uncacheExtent();
            _cache = new Extent(e._l,  e._b, e._cx, e._cy, e._tol);
            gstemp.transformer(nil); // to avoid deleting ttemp explicitly
        }
    }
    l = e._l; b = e._b; cx = l+(e._cx-l)*2.0; cy = b+(e._cy-b)*2.0;
    tol = e._tol;
            
    Transformer* tx = gs->transformer();
    if (tx != nil) {
        corners(l, b, cx, cy, *tx);
    }
    cx = (cx + l)/2.0;
    cy = (cy + b)/2.0;
}

boolean PolyGraphic::contains_gs (PointObj& po, Graphic* gs) {
    GlyphIndex count = _body->count();
    Graphic gstemp;
    Transformer ttemp;

    gstemp.transformer(&ttemp);
    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        concat_(gr, gr, gs, &gstemp);
        
        if (contains_(gr, po, &gstemp)) {
            gstemp.transformer(nil);
            return true;
        }
    }
    gstemp.transformer(nil); /* to avoid deleting ttemp explicitly*/
    return false;
}

boolean PolyGraphic::intersects_gs (BoxObj& box, Graphic* gs) {
    GlyphIndex count = _body->count();
    Graphic gstemp;
    Transformer ttemp;

    gstemp.transformer(&ttemp);
    for (GlyphIndex i = 0; i < count; i++) {
        Graphic* gr = (Graphic*) _body->component(i);
        concat_(gr, gr, gs, &gstemp);
        
        if (intersects_(gr, box, &gstemp)) {
            gstemp.transformer(nil);
            return true;
        }
    }
    gstemp.transformer(nil); /* to avoid deleting ttemp explicitly*/
    return false;
}

/**********************************************************************/
RootGraphic::RootGraphic (Graphic* gr, const Color* bg) : PolyGraphic(gr) {
    _bg = bg;
    Resource::ref(_bg);
}

RootGraphic::~RootGraphic () {
    Resource::unref(_bg);
}

void RootGraphic::append_ (Graphic* g) { 
    _body->append(g); 
    uncacheParents();
    uncacheExtent();
}

void RootGraphic::prepend_ (Graphic* g) { 
    _body->prepend(g); 
    uncacheParents();
    uncacheExtent();
}

void RootGraphic::insert_ (GlyphIndex i, Graphic* g) { 
    _body->insert(i, g); 
    uncacheParents();
    uncacheExtent();
}

void RootGraphic::remove_ (GlyphIndex i) { 
    _body->remove(i); 
    uncacheParents();
    uncacheExtent();
}

void RootGraphic::remove_ (Graphic* gr) {
    PolyGraphic::remove_(gr);
}

void RootGraphic::replace_ (GlyphIndex i, Graphic* g) { 
    _body->replace(i, g); 
    uncacheParents();
    uncacheExtent();
}

void RootGraphic::background(const Color* bg) {
    if (bg != nil) {
        Resource::ref(bg);
    }
    if (_bg != nil) {
        Resource::unref(_bg);
    }
    _bg = bg;
}

Glyph* RootGraphic::clone () const {
    Graphic* pg = new RootGraphic((Graphic*)this);

    GlyphIndex count = _body->count();
    for (GlyphIndex i = 0; i < count; i++) {
        Glyph* gr = _body->component(i);
        pg->append(gr->clone());
    }
    return pg;
}

void RootGraphic::request(Requisition& req) const {
    PolyGraphic::request(req);
    Requirement& rx = req.x_requirement();
    rx.alignment(0.0);
    
    Requirement& ry = req.y_requirement();
    ry.alignment(0.0);
}

void RootGraphic::undraw () {
    Coord l, b, r, t;
    Allocation a;
    _a = a;
    getbounds(l, b, r, t);
    translate(-l-(r-l)/2.0, -b-(t-b)/2.0);
    PolyGraphic::undraw();
}

void RootGraphic::translate(Coord dx, Coord dy) {
    if (count_() == 1) {
        child_(0)->translate(dx, dy);
    } else {
        translate(dx, dy);
    }
}

void RootGraphic::allocate (Canvas* c, const Allocation& a, Extension& ext) {
    if (!_a.equals(a, 0.001)) {
        Allocation b(a);
        Allotment& ax = _a.x_allotment();
        Allotment& ay = _a.y_allotment();
        Allotment& bx = b.x_allotment();
        Allotment& by = b.y_allotment();

        translate((bx.span()-ax.span())/2.0, (by.span()-ay.span())/2.0);
        translate(bx.begin()-ax.begin(), by.begin()-ay.begin());
        _a = a;
    }
    if (c != nil) {
        PolyGraphic::allocate(c, a, ext);
    }
}

boolean RootGraphic::drawclipped_gs(
    Canvas* c, Coord l, Coord b, Coord r, Coord t, Graphic* gs
) {
    boolean flag = true;
    c->push_clipping();
    c->clip_rect(_a.left(), _a.bottom(), _a.right(), _a.top());
    if (_bg != nil) {
        c->fill_rect(l, b, r, t, _bg);
    }
    flag = PolyGraphic::drawclipped_gs(c, l, b, r, t, gs);

    c->pop_clipping();
    return flag;
}

/**********************************************************************/
Line::Line(
    const Brush* brush, const Color* stroke, const Color* fill,
    Coord x1, Coord y1, Coord x2, Coord y2, Transformer* t
) : Graphic(brush, stroke, fill, nil, false, false, 2, t) {
    add_point(x1, y1);
    add_point(x2, y2);
}

Line::~Line () { }

Glyph* Line::clone () const {
    return new Line(_brush, _stroke, _fill, _x[0], _y[0], _x[1], _y[1], _t);
}

/**********************************************************************/
Rectangle::Rectangle (
    const Brush* brush, const Color* stroke, const Color* fill,
    Coord l, Coord b, Coord r, Coord t, Transformer* tx
) : Graphic(brush, stroke, fill, nil, true, false, 4, tx) {
    add_point(r, b);
    add_point(r, t);
    add_point(l, t);
    add_point(l, b);
}

Rectangle::~Rectangle () { }

Glyph* Rectangle::clone () const {
    return new Rectangle(
        _brush, _stroke, _fill, _x[2], _y[0], _x[0], _y[1], _t
    );
}

/**********************************************************************/
static float p0 = 1.00000000;
static float p1 = 0.89657547;   // cos 30 * sqrt(1 + tan 15 * tan 15)
static float p2 = 0.70710678;   // cos 45 
static float p3 = 0.51763809;   // cos 60 * sqrt(1 + tan 15 * tan 15)
static float p4 = 0.26794919;   // tan 15

Circle::Circle (
    const Brush* brush, const Color* stroke, const Color* fill,
    Coord x, Coord y, Coord r, Transformer* t
) : Graphic(brush, stroke, fill, nil, true, true, 25, t) {
    float px0 = p0 * r, py0 = p0 * r;
    float px1 = p1 * r, py1 = p1 * r;
    float px2 = p2 * r, py2 = p2 * r;
    float px3 = p3 * r, py3 = p3 * r;
    float px4 = p4 * r, py4 = p4 * r;
    
    add_point(x + r, y);
    add_curve(x + px2, y + py2, x + px0, y + py4, x + px1, y + py3);
    add_curve(x, y + r, x + px3, y + py1, x + px4, y + py0);
    add_curve(x - px2, y + py2, x - px4, y + py0, x - px3, y + py1);
    add_curve(x - r, y, x - px1, y + py3, x - px0, y + py4);
    add_curve(x - px2, y - py2, x - px0, y - py4, x - px1, y - py3);
    add_curve(x, y - r, x - px3, y - py1, x - px4, y - py0);
    add_curve(x + px2, y - py2, x + px4, y - py0, x + px3, y - py1);
    add_curve(x + r, y, x + px1, y - py3, x + px0, y - py4);
}

Circle::~Circle () { }

Glyph* Circle::clone () const {
    Coord r = _x[0] - _x[6];
    Coord x = _x[6];
    Coord y = _y[0];
    return new Circle(_brush, _stroke, _fill, x, y, r, _t);
}

/**********************************************************************/
Ellipse::Ellipse (
    const Brush* brush, const Color* stroke, const Color* fill,
    Coord x, Coord y, Coord rx, Coord ry, Transformer* t
) : Graphic(brush, stroke, fill, nil, true, true, 25, t) {
    float px0 = p0 * rx, py0 = p0 * ry;
    float px1 = p1 * rx, py1 = p1 * ry;
    float px2 = p2 * rx, py2 = p2 * ry;
    float px3 = p3 * rx, py3 = p3 * ry;
    float px4 = p4 * rx, py4 = p4 * ry;
    
    add_point(x + rx, y);
    add_curve(x + px2, y + py2, x + px0, y + py4, x + px1, y + py3);
    add_curve(x, y + ry, x + px3, y + py1, x + px4, y + py0);
    add_curve(x - px2, y + py2, x - px4, y + py0, x - px3, y + py1);
    add_curve(x - rx, y, x - px1, y + py3, x - px0, y + py4);
    add_curve(x - px2, y - py2, x - px0, y - py4, x - px1, y - py3);
    add_curve(x, y - ry, x - px3, y - py1, x - px4, y - py0);
    add_curve(x + px2, y - py2, x + px4, y - py0, x + px3, y - py1);
    add_curve(x + rx, y, x + px1, y - py3, x + px0, y - py4);
}

Ellipse::~Ellipse () { }

Glyph* Ellipse::clone () const {
    Coord rx = _x[0] - _x[6];
    Coord ry = _y[6] - _y[0];
    Coord x = _x[6];
    Coord y = _y[0];
    return new Ellipse(_brush, _stroke, _fill, x, y, rx, ry, _t);
}

/**********************************************************************/
Open_BSpline::Open_BSpline (
    const Brush* brush, const Color* stroke, const Color* fill, 
    Coord* x, Coord* y, int n, Transformer* t
) : Graphic(brush, stroke, fill, nil, false, true, (n + 2) * 3 + 1, t) {
    Bspline_move_to(x[0], y[0], x[0], y[0], x[0], y[0]);
    Bspline_curve_to(x[0], y[0], x[0], y[0], x[1], y[1]);
    for (int i = 1; i < n - 1; ++i) {
        Bspline_curve_to(x[i], y[i], x[i-1], y[i-1], x[i+1], y[i+1]);
    }
    Bspline_curve_to(x[n-1], y[n-1], x[n-2], y[n-2], x[n-1], y[n-1]);
    Bspline_curve_to(x[n-1], y[n-1], x[n-1], y[n-1], x[n-1], y[n-1]);
}

Open_BSpline::Open_BSpline (Open_BSpline* gr) : Graphic(gr) {}


Open_BSpline::~Open_BSpline () { }

Glyph* Open_BSpline::clone () const {
    Graphic* gr =  new Open_BSpline((Open_BSpline*) this);
    gr->ctrlpts(_x, _y, _ctrlpts);
    return gr;
}

/**********************************************************************/
Closed_BSpline::Closed_BSpline (
    const Brush* brush, const Color* stroke, const Color* fill, 
    Coord* x, Coord* y, int n, Transformer* t
) : Graphic(brush, stroke, fill, nil, true, true, n * 3 + 1, t) {
    Bspline_move_to(x[0], y[0], x[n-1], y[n-1], x[1], y[1]);
    for (int i = 1; i < n - 1; ++i) {
        Bspline_curve_to(x[i], y[i], x[i-1], y[i-1], x[i+1], y[i+1]);
    }
    Bspline_curve_to(x[n-1], y[n-1], x[n-2], y[n-2], x[0], y[0]);
    Bspline_curve_to(x[0], y[0], x[n-1], y[n-1], x[1], y[1]);
}

Closed_BSpline::Closed_BSpline (Closed_BSpline* gr) : Graphic(gr) {}

Closed_BSpline::~Closed_BSpline () { }

Glyph* Closed_BSpline::clone () const {
    Graphic* gr =  new Closed_BSpline((Closed_BSpline*) this);
    gr->ctrlpts(_x, _y, _ctrlpts);
    return gr;
}
/**********************************************************************/
Polyline::Polyline (
    const Brush* brush, const Color* stroke, const Color* fill,
    Coord* x, Coord* y, int n, Transformer* t
) : Graphic(brush, stroke, fill, nil, false, false, n, t) {
    add_point(x[0], y[0]);
    for (int i = 1; i < n; ++i) {
        add_point(x[i], y[i]);
    }
}

Polyline::~Polyline () { }

Glyph* Polyline::clone () const {
    return new Polyline(_brush, _stroke, _fill, _x, _y, _ctrlpts, _t);
}

/**********************************************************************/
Polygon::Polygon (
    const Brush* brush, const Color* stroke, const Color* fill, 
    Coord* x, Coord* y, int n, Transformer* t
) : Graphic(brush, stroke, fill, nil, true, false, n, t) {
    add_point(x[0], y[0]);
    for (int i = 1; i < n; ++i) {
        add_point(x[i], y[i]);
    }
}

Polygon::~Polygon () { }

Glyph* Polygon::clone () const {
    return new Polygon(_brush, _stroke, _fill, _x, _y, _ctrlpts, _t);
}

/**********************************************************************/
Text::Text (
    const Font* f, const Color* fg, const char* text, Transformer* tx
) {
    _stroke = fg;
    Resource::ref(_stroke);
    _font = f;
    Resource::ref(_font);
    _t = new Transformer;
    if (tx != nil) {
        *_t = *tx;
    }
    _fill = fg; /* a hack */

    _text = nil;
    if (text != nil) {
        _text = new String(text);
    }
    LayoutKit* layout = LayoutKit::instance();
    _body = layout->overlay();
    init();
}

Text::~Text () {
    delete _text;
    delete _body;
}

Glyph* Text::clone () const {
    return new Text(_font, _stroke, _text->string(), _t);
}

void Text::text (const char* text) {
    delete _text;
    _text = nil;
    if (text != nil) {
        _text = new String(text);
    }
    init();
}

const char* Text::text () {
    return _text->string();
}

void Text::init () {
    LayoutKit* layout = LayoutKit::instance();
    PolyGlyph* col = layout->vbox();
    PolyGlyph* line = layout->hbox();
    FontBoundingBox bbox;
    _font->font_bbox(bbox);
    Coord lineheight = bbox.ascent() + bbox.descent();
    char ch;
    
    for (int i = 0; (*_text)[i] != '\0'; i++) {
        ch = (*_text)[i];

        if (ch == '\n') {
            line->append(layout->strut(_font));
            col->append(layout->fixed_dimension(line, Dimension_Y,lineheight));
            line = layout->hbox();
        } else if (ch == ' ') {
            line->append(new Character(' ', _font, _stroke));
        } else if (ch != ')' && ch != '(') {
            if (ch == '\\') {
                ch = (*_text)[++i];
                if (isdigit(ch)) {
                    ch -= '0';
                    ch *= 8;
                    char digit;
                    digit = (*_text)[i++];
                    ch = (ch * 8) + digit - '0';
                    digit = (*_text)[i++];
                    ch = (ch * 8) + digit - '0';
                }
            }
            line->append(new Character(ch, _font, _stroke));
        }
    }

    Transformer fixtext;
    fixtext.translate(0, bbox.descent());
    _t->premultiply(fixtext);
    _body->append(col);
}

void Text::draw (Canvas* c, const Allocation& a) const {
    if (_t != nil) {
        c->push_transform();
        c->transform(*_t);
    } 
    _body->draw(c, a);
    if (_t != nil) {
        c->pop_transform();
    }
}

void Text::draw_gs (Canvas* c, Graphic* gs) {
    Transformer* tx = gs->transformer();
    if (tx != nil) {
        c->push_transform();
        c->transform(*tx);
    }
    if (_ctrlpts == 0) {
        Requisition req;
        request(req);
    }
    Allocation b(_a);
    Allotment& bx = b.x_allotment();
    Allotment& by = b.y_allotment();
    bx.origin(0.0);
    by.origin(0.0);
    bx.span(_x[2]-_x[0]);
    by.span(_y[2]-_y[0]);

    _body->draw(c, b);
    if (tx != nil) {
        c->pop_transform();
    }
}

void Text::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    if (_t != nil) {
        c->push_transform();
        c->transform(*_t);
    }
    if (_ctrlpts == 0) {
        Requisition req;
        request(req);
    }
    _a = a;
    Allocation b(_a);
    Allotment& bx = b.x_allotment();
    Allotment& by = b.y_allotment();
    bx.origin(0.0);
    by.origin(0.0);
    bx.span(_x[2]-_x[0]);
    by.span(_y[2]-_y[0]);
    _body->allocate(c, b, ext);
    if (_t != nil) {
        c->pop_transform();
    }
}

void Text::getextent_gs (
    Coord& l, Coord& b, Coord& cx, Coord& cy, float& tol, Graphic* gr
) {
    if (_ctrlpts == 0) {
        Requisition req;
        request(req);
    }
    Coord left, bottom, right, top;

    right = _x[2];
    top = _y[2];
    left = _x[0];
    bottom = _y[0];

    Transformer* t = gr->transformer();
    if (t != nil) {
        corners(left, bottom, right, top, *t);
    }
    tol = 3.0;
    l = left;
    b = bottom;
    cx = (left + right)/2.0;
    cy = (top + bottom)/2.0;
}

void Text::request (Requisition& req) const { 
    _body->request(req);
    Requirement& rx = req.x_requirement();
    Requirement& ry = req.y_requirement();

    Coord left, bottom, right, top;
    left = -rx.natural()*rx.alignment();
    right = left + rx.natural();
    bottom = -ry.natural()*ry.alignment();
    top = bottom + ry.natural();

    Text* text = (Text*) this;
    text->_ctrlpts = 4;
    text->_x[0] = left;
    text->_y[0] = bottom;
    text->_x[1] = left;
    text->_y[1] = top;
    text->_x[2] = right;
    text->_y[2] = top;
    text->_x[3] = right;
    text->_y[3] = bottom;

    if (_t != nil) {
        corners(left, bottom, right, top, *_t);
    }
    rx.natural(right - left);
    rx.stretch(0.0);
    rx.shrink(0.0);
    rx.alignment(-left / rx.natural());

    ry.natural(top - bottom);
    ry.stretch(0.0);
    ry.shrink(0.0);
    ry.alignment(-bottom / ry.natural());
}


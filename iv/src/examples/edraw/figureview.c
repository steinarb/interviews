/*
 * planar figures
 */

#include "figureview.h"
#include "pacemaker.h"
#include "globals.h"
#include "glyphviewer.h"
#include "grabber.bm"
#include "grabberMask.bm"

#include <InterViews/Bitmaps/enlargeHit.bm>
#include <InterViews/Bitmaps/enlargeMask.bm>
#include <InterViews/Bitmaps/reducerHit.bm>
#include <InterViews/Bitmaps/reducerMask.bm>

#include <InterViews/bitmap.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/cursor.h>
#include <InterViews/display.h>
#include <InterViews/event.h>
#include <InterViews/transformer.h>
#include <InterViews/window.h>
#include <IV-look/kit.h>
#include <IV-X11/xcanvas.h>
#include <OS/math.h>
#include <math.h>
#include <stdio.h>

static Cursor* zoom_in_cursor = nil;
static Cursor* zoom_out_cursor = nil;
static Cursor* grabber_cursor = nil;
static Cursor* window_cursor = nil;
static double zoom_factor = 600.0;

GrTarget::GrTarget(ViewIndex i, Graphic* t) {
    _index = i;
    _target = t;
}

ViewAction::ViewAction (Graphic* gr) {
    _gr = gr;
}

Graphic* ViewAction::target () const {
    return _gr;
}

boolean ViewAction::executable () const {
    return false;
}

class Translator : public ViewAction {
public:
    Translator(Graphic*, Coord, Coord);
    virtual void execute();
    virtual boolean executable() const;
protected:
    Coord _lx, _ly;
};

Translator::Translator (
    Graphic* gr, Coord lx, Coord ly
) : ViewAction(gr) {
    _lx = lx;
    _ly = ly;
}

void Translator::execute () {
    _gr->translate(_lx, _ly);
}

boolean Translator::executable () const { return true; }

class Scaler : public ViewAction {
public:
    Scaler(Graphic*, Coord, Coord, Coord, Coord);
    virtual void execute();
    virtual boolean executable() const;
protected:
    Coord _cx, _cy, _sx, _sy;
};

Scaler::Scaler (
    Graphic* gr, Coord sx, Coord sy, Coord cx, Coord cy
) : ViewAction(gr) {
    _sx = sx;
    _sy = sy;
    _cx = cx;
    _cy = cy;
}

void Scaler::execute () {
    _gr->scale(_sx, _sy, _cx, _cy);
}

boolean Scaler::executable () const { return true; }

class Rotator : public ViewAction {
public:
    Rotator(Graphic*, Coord, Coord, Coord);
    virtual void execute();
    virtual boolean executable() const;
protected:
    Coord _cx, _cy, _angle;
};

Rotator::Rotator (
    Graphic* gr, Coord angle, Coord cx, Coord cy
) : ViewAction(gr) {
    _angle = angle;
    _cx = cx;
    _cy = cy;
}

void Rotator::execute () {
    _gr->rotate(_angle, _cx, _cy);
}

boolean Rotator::executable () const { return true; }

static void initcursors() {
    if (zoom_in_cursor == nil) {
        Bitmap* zoom_in = new Bitmap(
            enlarger_hit_bits, enlarger_hit_width, enlarger_hit_height,
            enlarger_hit_x_hot, enlarger_hit_y_hot
        );
        Bitmap* zoom_in_mask = new Bitmap(
            enlarger_mask_bits, enlarger_mask_width, enlarger_mask_height,
            enlarger_mask_x_hot, enlarger_mask_y_hot
        );

        Bitmap* zoom_out = new Bitmap(
            reducer_hit_bits, reducer_hit_width, reducer_hit_height,
            reducer_hit_x_hot, reducer_hit_y_hot
        );
        Bitmap* zoom_out_mask = new Bitmap(
            reducer_mask_bits, reducer_mask_width, reducer_mask_height,
            reducer_mask_x_hot, reducer_mask_y_hot
        );

        zoom_in_cursor = new Cursor(zoom_in, zoom_in_mask);
        zoom_out_cursor = new Cursor(zoom_out, zoom_out_mask);

        Bitmap* grabber = new Bitmap(
            grabber_bits, grabber_width, grabber_height,
            grabber_x_hot, grabber_y_hot
        );
        Bitmap* grabber_mask = new Bitmap(
            grabberMask_bits, grabberMask_width, grabberMask_height,
            grabberMask_x_hot, grabberMask_y_hot
        );
        grabber_cursor = new Cursor(grabber, grabber_mask);
    }
}

static void initviews (GrView* pv) {
    Graphic* p = pv->getgraphic();
    if (p != nil) {
        ViewIndex p_count = p->count_();
        for (ViewIndex i = 0; i < p_count; i++) {
            Graphic* c = p->child_(i);
            if (c->count_() > 0) {
                GrView* cv = new PolyView(c);
                pv->append(cv);
                //initviews(cv);
            } else {
                GrView* cv = new GrView(c);
                pv->append(cv);
            }
        }
    }
}

GrView::GrView (Graphic* gr) {
    _gr = gr;
    Resource::ref(_gr);
    _index = 0;
}

GrView::~GrView () {
    Resource::unref(_gr);
}

Graphic* GrView::getgraphic () const {
    return _gr;
}

void GrView::setgraphic (Graphic* gr) {
    Resource::ref(gr);
    Resource::unref(_gr);
    _gr = gr; 
}

void GrView::setindex (ViewIndex i) {
    _index = i;
}

ViewIndex GrView::getindex () const {
    return _index;
}

boolean GrView::grasp (const Event& e, Tool& tool, ViewAction*& action) {
    action = nil;
    ToolState& ts = tool.toolstate();
    ts._init = e;
    ts._last = e;
    Graphic* gr = getgraphic();
    gr->getbounds(ts._l, ts._b, ts._r, ts._t);
    if (tool.tool() == Tool::scale) {
        Coord lx, ly;
        Coord cx, cy;
        lx = e.pointer_x(); 
        ly = e.pointer_y();
        cx = (ts._l + ts._r)/2.0;
        cy = (ts._b + ts._t)/2.0;

        action = new Scaler(
            gr, Math::abs((lx-cx)*2.0/(ts._r-ts._l)), 
            Math::abs((ly-cy)*2.0/(ts._t-ts._b)),
            cx, cy
        );
        action->ref();
    }
    return true;
}

boolean GrView::manipulating (
    const Event& e, Tool& tool, ViewAction*& action
) {
    action = nil;
    if (e.type() == Event::up) {
        return false;
    } else {
        unsigned int tool_type = tool.tool();
        ToolState& ts = tool.toolstate();

        if (tool_type != Tool::nop) {
            float pi = 3.14159;

            Graphic* gr = getgraphic();
            Transformer* tx = ts._gs.transformer();
            
            Coord x, y, lx, ly;
            
            x = ts._last.pointer_x();
            y = ts._last.pointer_y();
            lx = e.pointer_x(); 
            ly = e.pointer_y();
            
            switch(tool_type) {
            case Tool::select:
                break;
            case Tool::move:
                {
                    if (tx != nil) {
                        tx->inverse_transform(lx, ly);
                        tx->inverse_transform(x, y);
                    }
                    ts._last = e;
                    action = new Translator(gr, lx-x, ly-y);
                    action->ref();
                }
                break;
            case Tool::scale:
                {
                    Coord cx, cy;
                    cx = (ts._l + ts._r)/2.0;
                    cy = (ts._b + ts._t)/2.0;
                    
                    ts._last = e;
                    action = new Scaler(
                        gr, (lx-cx)/(x-cx), (ly-cy)/(y-cy), cx, cy
                    );
                    action->ref();
                } 
                break;
            case Tool::rotate:
                {
                    Coord cx, cy;
                    cx = (ts._l + ts._r)/2.0;
                    cy = (ts._b + ts._t)/2.0;

                    float ldy = ly-cy; float ldx = lx-cx;
                    float dy = y-cy; float dx = x-cx;

                    float cur = atan(ldy/ldx)/pi*180.0;
                    float last = atan(dy/dx)/pi*180.0;

                    if (ldx < 0.0) {
                        cur += 180.0;
                    }
                    if (dx < 0.0) {
                        last += 180.0;
                    }
                    ts._last = e;
                    action = new Rotator(gr, cur-last, cx, cy);
                    action->ref();
                }
                break;
            }
        }
    } 
    return true;
}

void GrView::effect (const Event&, Tool&, ViewAction*& action) {
    action = nil;
}

void GrView::append(GrView*) {}
void GrView::prepend(GrView*) {}
void GrView::insert(ViewIndex, GrView*) {}
void GrView::remove(ViewIndex) {}
void GrView::remove(GrView*) {}

ViewIndex GrView::count () const { return 0; }
GrView* GrView::child (ViewIndex) const { return nil; }

/************************************************************************/
declarePtrList(GrViewList, GrView);
implementPtrList(GrViewList, GrView);

PolyView::PolyView (Graphic* gr) : GrView (gr) {
    _body = new GrViewList;
}

PolyView::~PolyView () {
    ViewIndex count = _body->count();
    for (ViewIndex i = 0; i < count; i++) {
        GrView* grview = _body->item(i);
        delete grview;
    }
    delete _body;
    _gr->remove_all_();
}

void PolyView::append (GrView* g) { 
    _body->append(g);
    g->setindex(count()-1);
}

void PolyView::prepend (GrView* g) { 
    insert(0, g);
}

void PolyView::insert (ViewIndex i, GrView* g) { 
    _body->insert(i, g); 
    for (ViewIndex j = i; j < count(); j++) {
        child(j)->setindex(j);
    }
}

void PolyView::remove (ViewIndex i) { 
    _body->remove(i); 
    for (ViewIndex j = i; j < count(); j++) {
        child(j)->setindex(j);
    }
}

void PolyView::remove (GrView* gr) {
    ViewIndex count = _body->count();
    for (int i = 0; i < count; i++) {
        if (child(i) == gr) {
            remove(i);
            break;
        }
    }
}

ViewIndex PolyView::count () const { return _body->count(); }

GrView* PolyView::child (ViewIndex i) const { 
    return _body->item(i);
}

/**********************************************************************/
implementList(TargetList,GrTarget);

RootView::RootView (
    RootGraphic* rootgr, Graphic* gr, const Color* bg, GlyphViewer* gviewer
) : PolyView(gr) {
    _gviewer = gviewer;
    _targets = new TargetList(5);
    _bg = bg;
    Resource::ref(_bg);
    _rootgr = rootgr;
    if (_rootgr == nil) {
        _rootgr = new RootGraphic(nil, _bg);
        _rootgr->ref();
    }
    if (gr == nil) {
        setgraphic(new PolyGraphic);
    }
    _lx = _ly = 0.0;
    initcursors();
}

RootView::~RootView () {
    _rootgr->remove_(getgraphic());
    delete _targets;
    Resource::unref(_rootgr);
    Resource::unref(_bg);
}

void RootView::setgraphic (Graphic* gr) {
    if (gr == nil) {
        gr = new PolyGraphic;
    }
    Graphic* orig = getgraphic();
    if (gr != orig) {
        if (orig != nil) {
            _rootgr->remove_(orig);
        }
        ViewIndex count = _body->count();
        for (ViewIndex i = 0; i < count; i++) {
            GrView* grview = _body->item(i);
            delete grview;
        }
        _body->remove_all();
        PolyView::setgraphic(gr);
        initviews(this);
        _rootgr->append_(gr);
        _rootgr->undraw();
    }
}

void RootView::background(const Color* bg) {
    if (bg != nil) {
        Resource::ref(bg);
    }
    if (_bg != nil) {
        Resource::unref(_bg);
    }
    _bg = bg;
    _rootgr->background(bg);
}

boolean RootView::grasp (const Event& e, Tool& tool, ViewAction*& action) {
    action = nil;
    Window* w = e.window();
    if (window_cursor == nil) {
        window_cursor = w->cursor();
    }
    Graphic* target;
    boolean flag = true;
    float tol = 2.0;
    unsigned int tool_type = tool.tool();
    Graphic* gr = getgraphic();
    WidgetKit* kit = WidgetKit::instance();

    switch (tool_type) {
    case Tool::select:
        break;
    case Tool::rate_scroll:
        {
            w->cursor(kit->ufast_cursor());
            _lx = e.pointer_x();
            _ly = e.pointer_y();
            GrTarget sel(-1, getgraphic());
            _targets->append(sel);
        }
        break;
    case Tool::grab_scroll:
        {
            w->cursor(kit->hand_cursor());
            _lx = e.pointer_x();
            _ly = e.pointer_y();
            GrTarget sel(-1, getgraphic());
            _targets->append(sel);
        }
        break;
    case Tool::rate_zoom:
        {
            w->cursor(zoom_in_cursor);
            _lx = e.pointer_x();
            _ly = e.pointer_y();
            GrTarget sel(-1, getgraphic());
            _targets->append(sel);
        }
        break;
    case Tool::move:
    case Tool::scale:
    case Tool::rotate:
        {
            BoxObj ibox(
                e.pointer_x()-tol, e.pointer_y()-tol, 
                e.pointer_x()+tol, e.pointer_y()+tol
            );
            long index = gr->last_intersecting(ibox, target);
            if (target != nil) {
                target->alpha_stroke(false);  //hack
                target->alpha_fill(false);    //hack

                Window* w = e.window();
                Canvas* c = w->canvas();
                w->cursor(grabber_cursor);

                ToolState& ts = tool.toolstate();
                gr->total_gs(ts._gs);

                ViewAction* kidact = nil;
                flag = child(index)->grasp(e, tool, kidact);
                action = kidact;
                GrTarget sel(index, target);
                _targets->append(sel);
            }
        } 
        break;
    }
    return flag;
}

boolean RootView::manipulating (
    const Event& e, Tool& tool, ViewAction*& action
) {
    action = nil;
    boolean flag = true;
    ViewIndex count = _targets->count();
    Window* w = e.window();
    WidgetKit* kit = WidgetKit::instance();

    Graphic* gr = getgraphic();
    unsigned int tool_type = tool.tool();
    switch (tool_type) {
    case Tool::select:
    case Tool::narrow:
        if (e.type() == Event::up) {
            flag = false;
        }
        break;
    case Tool::rate_scroll:
        {
            if (e.type() == Event::up) {
                flag = false;
            } else {
                Coord dx = _lx - e.pointer_x();
                Coord dy = _ly - e.pointer_y();
                
                if (dx != 0.0 && dy != 0.0) {
                    double angle = atan2(dy, dx)*180/M_PI;
                    
                    if (angle < -157.5) {
                        w->cursor(kit->rfast_cursor());
                    } else if (angle < -112.5) {
                        w->cursor(kit->rufast_cursor());
                    } else if (angle < -67.5) {
                        w->cursor(kit->ufast_cursor());
                    } else if (angle < -22.5) {
                        w->cursor(kit->lufast_cursor());
                    } else if (angle < 22.5) {
                        w->cursor(kit->lfast_cursor());
                    } else if (angle < 67.5) {
                        w->cursor(kit->ldfast_cursor());
                    } else if (angle < 112.5) {
                        w->cursor(kit->dfast_cursor());
                    } else if (angle < 157.5) {
                        w->cursor(kit->rdfast_cursor());
                    } else {
                        w->cursor(kit->rfast_cursor());
                    }
                }
                action = new Translator(gr, dx, dy);
                action->ref();
            }
        }
        break;
    case Tool::grab_scroll:
        {
            if (e.type() == Event::up) {
                flag = false;
            } else {
                Coord dx = e.pointer_x() - _lx;
                Coord dy = e.pointer_y() - _ly;
                
                if (dx != 0.0 || dy != 0.0) {
                    action = new Translator(gr, dx, dy);
                    action->ref();
                    _lx = e.pointer_x();
                    _ly = e.pointer_y();
                }
            }
        }
        break;
    case Tool::rate_zoom:
        {
            if (e.type() == Event::up) {
                flag = false;
            } else {
                Coord dx = 0.0;
                Coord dy = _ly - e.pointer_y();
                
                if (dy != 0.0) {
                    double factor;
                    if (dy > 0.0) {
                        w->cursor(zoom_out_cursor);
                        factor = zoom_factor/(zoom_factor+dy);
                        
                    } else {
                        w->cursor(zoom_in_cursor);
                        factor = (zoom_factor-dy)/zoom_factor;
                    }
                    action = new Scaler(gr, factor, factor, _lx, _ly);
                    action->ref();
                }
            }
        }
        break;
    default:
        {
            ViewAction* kidact = nil;
            for (ViewIndex i = 0; i < count && flag; i++) {
                ViewIndex index = _targets->item_ref(i)._index;
                
                flag = child(index)->manipulating(e, tool, kidact);
            }
            action = kidact;
        }
        break;
    }
    return flag;
}
    
void RootView::effect (const Event& e, Tool& tool, ViewAction*& action) {
    action = nil;
    Window* w = e.window();
    if (w != nil) {
        w->cursor(window_cursor);
        Canvas* c = w->canvas();
        ViewIndex count = _targets->count();
        Graphic* gr = getgraphic();

        ViewAction* kidact = nil;
        for (ViewIndex i = 0; i < count; i++) {
            ViewIndex index = _targets->item_ref(i)._index;
            if (index >= 0) {
                child(index)->effect(e, tool, kidact);
                //gr->child_(index)->flush();
            }
        }
        action = kidact;
        _targets->remove_all();
    } 
}

Tool::Tool (unsigned int cur_tool) {
    _cur_tool = cur_tool;
    _toolstate = new ToolState;
}

Tool::~Tool () {
    delete _toolstate;
}

unsigned int Tool::tool () { return _cur_tool; }

void Tool::tool (unsigned int cur_tool) { _cur_tool = cur_tool; }

void Tool::reset () {
    delete _toolstate;
    _toolstate = new ToolState;
}


ToolState& Tool::toolstate () { return *_toolstate; }

void Tool::toolstate(ToolState* toolstate) {
    delete _toolstate;
    _toolstate = toolstate;
}


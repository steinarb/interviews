#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/display.h>
#include <InterViews/event.h>
#include <InterViews/handler.h>
#include <InterViews/hit.h>
#include <InterViews/polyglyph.h>
#include <InterViews/session.h>
#include <InterViews/target.h>
#include <InterViews/transformer.h>
#include <InterViews/window.h>

#include <OS/list.h>

#include <InterViews/brush.h>
#include <IV-X11/xevent.h>
#include <IV-X11/xcanvas.h>
#include <sys/time.h>

#include <math.h>
#include <stdio.h>
#include "figure.h"
#include "figureview.h"
#include "globals.h"
#include "glyphviewer.h"

#include "pacer.h"
#include "pacemaker.h"
#include "pacekeeper.h"
#include "gcollector.h"

static const long LEVELS = 6;

static long count_leaves (Pacer* p) {
    long count = 0;
    if (p->count() > 0) {
        for (long i = 0; i < p->count(); i++) {
            Pacer* kid = p->child(i);
            count += count_leaves(kid);
        }
    } else {
        count = 1;
    }
    return count;
}

static long inc;
static long cur_inc;
static long cur_level;

static void RecurAutoGrade(Pacer* p) {
    if (p->count() > 0) {
        for (long i = 0; i < p->count(); i++) {
            Pacer* kid = p->child(i);
            RecurAutoGrade(kid);
        }
    } else {
        cur_inc--;
        if (cur_inc < 0) {
            cur_inc = inc;
            if (cur_level > 0) {
                cur_level--;
            }
        }
        GraphicPacer* gp = (GraphicPacer*) p;
        gp->set_grades(0, cur_level);
    }
}

static void AutoGrade (Pacer* p) {
    long count = count_leaves(p);

    cur_level = LEVELS-1;
    inc = count/LEVELS;
    cur_inc = count/LEVELS;
    
    RecurAutoGrade(p);
}

static Pacer* CreatePacers(Canvas* c, Graphic* gr) {
    Pacer* d;
    if (gr->count_() > 0) {
        d = new Grader(new Distributor);
        for (GlyphIndex i = 0; i < gr->count_(); i++) {
            d->append(CreatePacers(c, gr->child_(i)));
        }
    } else {
        d = new GraphicPacer(c, gr);
    }
    return d;
}

static BoxObj allot;

static void effect (ViewAction* action, BoxObj& box) {
    Graphic* gr;
    Coord l, b, r, t;

    gr = action->target();
    gr->getbounds(l, b, r, t);
    BoxObj before(l, b, r, t);
    action->execute();
    gr->getbounds(l, b, r, t);
    BoxObj after(l, b, r, t);
    box = before+after;
    box = allot-box;

// a hack to get around xor dudus    
    Coord pad = 5.0;
    box._l -= pad;
    box._b -= pad;
    box._r += pad;
    box._t += pad;
}

class ClearAction : public Action {
public:
    ClearAction();
    virtual void execute();
};

ClearAction::ClearAction () {}

void ClearAction::execute () { GraphicPacer::clear(); }

class ContAction : public Action {
public:
    ContAction(ViewAction*, Pacer*);
    virtual ~ContAction();

    virtual void execute();
protected:
    ViewAction* _vaction;
    Pacer* _pacer;
};

ContAction::ContAction (
    ViewAction* vaction, Pacer* pacer
) {
    _vaction = vaction;
    Resource::ref(_vaction);
    _pacer = pacer;
}

ContAction::~ContAction () {
    Resource::unref(_vaction);
}

void ContAction::execute () {
    BoxObj box;
    if (_vaction != nil) {
        effect(_vaction, box);

        for (long i = 0; i < _pacer->count(); i++) {
            FCGraphicPacer* fc = (FCGraphicPacer*) _pacer->child(i);
            fc->incur(box);
        }
    }
}

class FullFixer : public GraphicPacer {
public:
    FullFixer(Canvas* c, Graphic* _gr, const Color* fill);

    virtual void set_graphic(Graphic*);
    void set_clipped (BoxObj&);
    virtual void play(RunTime&);
protected:
    void xor();
protected:
    BoxObj _allot;
    const Color* _fill;

    Color* _color;
    Brush* _brush;
    BoxObj _box;
    Transformer* _t;
    boolean _drawn;
};

inline void FullFixer::set_clipped(BoxObj& allot) { _allot = allot; }

FullFixer::FullFixer (
    Canvas* c, Graphic* gr, const Color* fill
) : GraphicPacer (c, gr) {
    _fill = fill;
    Resource::ref(_fill);

    _color = new Color(*fill, 1.0, Color::Xor);
    _color->ref();
    _brush = new Brush(0.0);
    _brush->ref();
    _drawn = false;

    set_graphic(gr);
}

void FullFixer::set_graphic(Graphic* gr) {
    GraphicPacer::set_graphic(gr);
    _t = new Transformer;

    _gr->eqv_transformer(*_t);
    _gr->getbounds(_box._l, _box._b, _box._r, _box._t);
    _t->inverse_transform(_box._l, _box._b);
    _t->inverse_transform(_box._r, _box._t);
}

void FullFixer::xor () {
    boolean doit = false;
    if (!_drawn) {
        _drawn = true;
        doit = true;
    } else {
        doit = true;
        _drawn = false;
    }
    if (doit) {
        _canvas->push_clipping();
        _canvas->clip_rect(_allot._l, _allot._b, _allot._r, _allot._t);
        _canvas->front_buffer();
        _canvas->push_transform();
        _canvas->transform(*_t);
        _canvas->rect(
            _box._l, _box._b, _box._r, _box._t, _color, _brush
        );
        _canvas->pop_transform();
        _canvas->pop_clipping();
        _canvas->back_buffer();
    }
}

void FullFixer::play (RunTime&) {
/*
    CanvasRep* crep = _canvas->rep();
    BoxObj box = _allot-_damage;

    _canvas->damage(box._l, box._b, box._r, box._t);
    crep->start_repair();
    //_canvas->fill_rect(_damage._l, _damage._b, _damage._r, _damage._t, _fill);
    //_gr->drawclipped(_canvas,_damage._l, _damage._b, _damage._r, _damage._t);

    _gr->alpha_fill(true);
    _gr->alpha_stroke(true);

    _gr->drawclipped(_canvas,_damage._l, _damage._b, _damage._r, _damage._t);

    _gr->alpha_fill(false);
    _gr->alpha_stroke(false);
*/

    if (_drawn) {
        xor();
        _drawn = false;
    }
    _gr->eqv_transformer(*_t);
    xor();
    GraphicPacer::clear();
}

class GlyphGrabber : public Handler {
public:
    GlyphGrabber(GlyphViewer*, Graphic* master);
    virtual ~GlyphGrabber();

    virtual boolean event(Event&);
protected:
    boolean _busy;
    int _orig_tool;

    PaceMaker* _pmaker;

    PaceKeeper* _contkeeper;
    PaceKeeper* _ekeeper;
    PaceKeeper* _dampedkeeper;
    PaceKeeper* _pckeeper;
    PaceKeeper* _cohkeeper;

    GlyphViewer* _gviewer;
    Graphic* _master;


    UIScheduler* _ui;
    ActionPacer* _view0;
    Grader* _gp00;

    Grader* _view1;
    ActionPacer* _action;
    Clipper* _clipper10;
    Clipper* _clipper11;

    Repairer* _fixer;
    Repairer* _filler;

    SelectPacer* _selector;

    Grader* _gp10;
    Grader* _gp11;

    boolean _continue;
    boolean _control_is_down;

    XorPacer* _xor;
};

GlyphGrabber::GlyphGrabber (GlyphViewer* gviewer, Graphic* master) {
    _gviewer = gviewer;

    _busy = false;
    _orig_tool = -1;
    _master = master;

    Resource::ref(_master);

    const Color* bg = gviewer->getroot()->background();
    Canvas* canvas = gviewer->canvas();

    _ui = new UIScheduler(new DragMapper);

    _view0 = new ActionPacer;   
    _gp00 = new Grader(new Tiler);
    
    _view0->set_kid(_gp00);
    for (long i = 0; i < _master->count_(); i++) {
        _gp00->append(new FCGraphicPacer(canvas, _master->child_(i), bg));
    }
    _view1 = new Grader(new Tiler);

    _fixer = new Fixer(canvas, _master->child_(0));
    _filler = new Filler(canvas, _master->child_(0), bg);

    _action = new ActionPacer(nil, new ClearAction);

    _selector = new SelectPacer(canvas, _master);
    _selector->set_damaged(&GraphicPacer::damaged_area());
    
    _clipper10 = new Clipper(canvas, &_fixer->damaged_area());
    _clipper11 = new Clipper(canvas, &GraphicPacer::damaged_area());

    _gp10 = new Grader(new Normalizer);
    _gp11 = new Grader(new Tiler);

    _view1->append(_clipper10);
    _clipper10->set_kid(_action);

    _action->set_kid(_gp10);

    _gp10->append(_filler);
    _gp10->append(_fixer);

    _view1->append(_selector);
    _selector->set_kid(_clipper11);

    _clipper11->set_kid(_gp11);

    _xor = new XorPacer(canvas, nil, bg);
    //_view1->append(_xor);

    _contkeeper = new Continuator(_ui);
    _ekeeper = new EventKeeper(_ui);
    _dampedkeeper = new DampedKeeper(_ui);
    _pckeeper = new PCKeeper(_ui);
    _cohkeeper = new CoherenceKeeper(_ui);

    _pmaker = new PaceMaker(_cohkeeper);
}

GlyphGrabber::~GlyphGrabber () {
    Resource::unref(_master);
    _ui->set_kid(nil);
    delete _ui;
    delete _contkeeper;
    delete _ekeeper;
    delete _dampedkeeper;
    delete _pckeeper;
    delete _cohkeeper;

    delete _view0;
    delete _view1;
}

class SelGraphic : public PolyGraphic {
public:
    SelGraphic();

    virtual void append_(Graphic*);
    virtual void prepend_(Graphic*);
    virtual void insert_(GlyphIndex, Graphic*);
    virtual void remove_(GlyphIndex);
    virtual void remove_(Graphic*);
    virtual void replace_(GlyphIndex, Graphic*);
};

SelGraphic::SelGraphic() {}

void SelGraphic::append_ (Graphic* g) { 
    _body->append(g); 
}

void SelGraphic::prepend_ (Graphic* g) { 
    _body->prepend(g); 
}

void SelGraphic::insert_ (GlyphIndex i, Graphic* g) { 
    _body->insert(i, g); 
}

void SelGraphic::remove_ (GlyphIndex i) { 
    _body->remove(i); 
}

void SelGraphic::remove_ (Graphic* gr) {
    PolyGraphic::remove_(gr);
}

void SelGraphic::replace_ (GlyphIndex i, Graphic* g) { 
    _body->replace(i, g); 
}

boolean GlyphGrabber::event (Event& e) {
    RootView* rv = _gviewer->getroot();
    Canvas* canvas = _gviewer->canvas();
    const Allocation& a = _gviewer->allocation();
    if (e.type() == Event::down && !_busy) {
        _busy = true;
        Display* d = e.display();
        Canvas* c = e.window()->canvas();
        
        d->grab(e.window(), this);
        _orig_tool = -1;

        ViewAction* action = nil;
        BoxObj box(a.left(), a.bottom(), a.right(), a.top());

        Tool& tool = _gviewer->tool();
        _control_is_down = e.control_is_down();

        if (_control_is_down) {
            _ui->set_kid(_view0);
            for (long i = 0; i < _gp00->count(); i++) {
                FCGraphicPacer* fc = (FCGraphicPacer*) _gp00->child(i);
                fc->set_clipped(box);
            }
            e.poll();
            _orig_tool = tool.tool();
            if (e.left_is_down()) {
                tool.tool(Tool::rate_scroll);
                _ui->set_mapper(new ContMapper);
                _continue = rv->grasp(e, tool, action);
                _view0->set_before(
                    new ContAction(action, _gp00)
                );
            } else if (e.middle_is_down()) {
                tool.tool(Tool::grab_scroll);
                _continue = rv->grasp(e, tool, action);
                _ui->set_mapper(new DragMapper);
                _view0->set_before(nil);

            } else if (e.right_is_down()) {
                tool.tool(Tool::rate_zoom);
                _continue = rv->grasp(e, tool, action);
                ContMapper* mapper = new ContMapper;
                mapper->set_dimension(Cont_Y);
                _ui->set_mapper(mapper);
                _view0->set_before(
                    new ContAction(action, _gp00)
                );
            }                
        } else {
            _ui->set_kid(_view1);
            _ui->set_mapper(new DragMapper);
            
            _clipper10->set_clipped(box);
            _clipper11->set_clipped(box);
            _selector->set_clipped(box);
            
            _continue = rv->grasp(e, tool, action);
        }
        _ui->cur_event(e);

        if (_continue && action != nil) {
            BoxObj box;
            
            if (action->executable()) {
                effect(action, box);
                if (_control_is_down) {
                    for (long i = 0; i < _gp00->count(); i++) {
                        FCGraphicPacer* fc = 
                            (FCGraphicPacer*) _gp00->child(i);
                        fc->incur(box);
                    }
                } else {
                    GraphicPacer::incur(box);
                    _fixer->incur(box);
                    _filler->incur(box);
                }
            }
        }
        if (!_control_is_down) {
            _gp11->delete_all();
            
            const TargetList* t = rv->cur_targets();
            long count = t->count();
            if (count == 1 && t->item_ref(0)._target == rv->getgraphic()) {
                /* not supported */
            } else {
                if (count == 1) {
                    for (long i = 0; i < _master->count_(); i++) {
                        Graphic* sel = _master->child_(i)->child_(
                            t->item_ref(0)._index
                        );
                        _gp11->append(new GraphicPacer(canvas, sel));
                    }
                    Graphic* sel = _master->child_(0)->child_(
                        t->item_ref(0)._index
                    );
                    //_xor->set_clipped(box);
                    //_xor->set_graphic(sel);
                } else {
                    /* not supported */
                } 
            }            
            
        }
        _ui->regrade();
        switch(_gviewer->get_policy()) {
        case PaceKeeper::NoPK:
            _pmaker->set_pacekeeper(_contkeeper);
            break;
        case PaceKeeper::EDriven:
            _pmaker->set_pacekeeper(_ekeeper);
            break;
        case PaceKeeper::DampedOsc:
            _pmaker->set_pacekeeper(_dampedkeeper);
            break;
        case PaceKeeper::PC:
            _pmaker->set_pacekeeper(_pckeeper);
            break;
        case PaceKeeper::Coh:
            _pmaker->set_pacekeeper(_cohkeeper);
            break;
        default:
            _pmaker->set_pacekeeper(_cohkeeper);
            break;
        }

        _pmaker->startall();

    } else if (e.type() == Event::up && _busy) {
        _busy = false;

        Display* d = e.display();
        d->ungrab(this);
        Canvas* c = e.window()->canvas();
        
        if (_continue) {
            ViewAction* action = nil;
            rv->effect(e, _gviewer->tool(), action);
            if (action != nil) {
                BoxObj box;
                
                if (action->executable()) {
                    effect(action, box);
                    if (_control_is_down) {
                        for (long i = 0; i < _gp00->count(); i++) {
                            FCGraphicPacer* fc = 
                                (FCGraphicPacer*) _gp00->child(i);
                            fc->incur(box);
                        }
                    } else {
                        GraphicPacer::incur(box);
                        _fixer->incur(box);
                        _filler->incur(box);
                        //_xor->incur(box);
                    }
                }
                action->unref();
            }
        }
        _ui->cur_event(e);
        d->ungrab(this);
        _pmaker->stopall();

        Canvas* canvas = _gviewer->canvas();
        canvas->damage_all();

        const Allocation& a = _gviewer->allocation();

        canvas->fill_rect(
            a.left(), a.bottom(), a.right(), a.top(), 
            _gviewer->getroot()->background()
        );
        _gviewer->getroot()->getgraphic()->draw(canvas, a);

        _fixer->clear();
        _filler->clear();
        GraphicPacer::clear();
        for (long i = 0; i < _gp00->count(); i++) {
            FCGraphicPacer* fc = (FCGraphicPacer*) _gp00->child(i);
            fc->clear();
        }

        if (_orig_tool >= 0) {
            Tool& tool = _gviewer->tool();
            tool.tool(_orig_tool);
            _orig_tool = -1;
        }

    } else if (_busy) {
        if (_continue) {
            Canvas* c = e.window()->canvas();
            ViewAction* action = nil;
            _continue = rv->manipulating(e, _gviewer->tool(), action);
            if (action != nil) {
                if (action->executable()) {
                    BoxObj box;
                    effect(action, box);

                    if (_control_is_down) {
                        for (long i = 0; i < _gp00->count(); i++) {
                            FCGraphicPacer* fc = 
                                (FCGraphicPacer*) _gp00->child(i);
                            fc->incur(box);
                        }
                    } else {
                        GraphicPacer::incur(box);
                        _fixer->incur(box);
                        _filler->incur(box);
                        //_xor->incur(box);
                    }
                }
                Tool& tool = _gviewer->tool();
                if (_control_is_down && tool.tool() != Tool::grab_scroll) {
                    _view0->set_before(
                        new ContAction(action, _gp00)
                    );
                }
                action->unref();
            }
            _ui->cur_event(e);
        }
    }
    return true;
}    

GlyphViewer::GlyphViewer(
    float w, float h
) : MonoGlyph(nil) {
    _canvas = nil;

    _root = new RootView;
    _root->ref();
    _root->viewer(this);
    _root->background(new Color(1.0, 1.0, 1.0, 1.0));

    _width = w;
    _height = h;

    _grabber = nil;

    body(new Target(_root->getrootgr(),TargetAlwaysHit));

    initshape();
    initgraphic();
}

GlyphViewer::~GlyphViewer () {
    _grabber->unref();
    _root->unref();
}

Tool& GlyphViewer::tool () {
    return _tool;
}

void GlyphViewer::set_policy (unsigned int policy) {
    _policy = policy;
}

unsigned int GlyphViewer::get_policy () {
    return _policy;
}

RootView* GlyphViewer::getroot () { return _root; }

void GlyphViewer::setrootgr (Graphic* root) {
    if (root != _root->getgraphic()) {
        Extension ext;
        Allocation a;
        
        _master = root;

        if (_master->count_() > 0) {
            _root->setgraphic(root->child_(0));
        } else {
            _root->setgraphic(nil);
        }
        root = _root->getrootgr();
        root->transformer(nil);
        body(new Target(root, TargetAlwaysHit));

        initgraphic();
        Canvas* c = canvas();
        if (c != nil) {
            Requisition req;
            root->request(req);
            root->allocate(c, _a, ext);
        }
        Resource::unref(_grabber);
        if (_master->count_() > 0) {
            _grabber = new GlyphGrabber(this, _master);
        } else {
            _grabber = nil;
        }
        Resource::ref(_grabber);
    }
}

void GlyphViewer::initshape () {
    if (_width < -0.5 && _height < -0.5) {
        Coord l, b, r, t;
        _root->getgraphic()->getbounds(l, b, r, t);
        _width = r - l;
        _height = t - b;
    }
    Allotment& ax = _a.x_allotment();
    Allotment& ay = _a.y_allotment();
    ax.span(_width);
    ay.span(_height);
}

void GlyphViewer::initgraphic () {
    Coord l, b, r, t;
    _root->getgraphic()->getbounds(l, b, r, t);
    _root->getgraphic()->translate(-l-(r-l)/2.0, -b-(t-b)/2.0);

/*
    if (_master->count_() > 0) {
        Graphic* ref = _master->child_(0);
        for (long i = 1; i < _master->count_(); i++) {
            ref->align(Center, _master->child_(i), Center);
        }
    }
*/
    Canvas* c = canvas();
    if (c != nil) {
        c->damage_all();
    }
}

void GlyphViewer::allocate(Canvas* c, const Allocation& a, Extension& ext) {
    _canvas = c;
    _a = a;
    allot._l = _a.left();
    allot._b = _a.bottom();
    allot._r = _a.right();
    allot._t = _a.top();

    MonoGlyph::allocate(c, a, ext);
    ext.merge(c, a);
}

void GlyphViewer::request(Requisition& req) const {
    Requirement& rx = req.x_requirement();
    rx.natural(_width);
    rx.stretch(fil);
    rx.shrink(_width);
    rx.alignment(0.0);
    
    Requirement& ry = req.y_requirement();
    ry.natural(_height);
    ry.stretch(fil);
    ry.shrink(0.0);
    ry.alignment(0.0);
}

void GlyphViewer::draw(Canvas* c, const Allocation& a) const {
    _root->getrootgr()->draw(c, a);
}

void GlyphViewer::pick(Canvas* c, const Allocation& a, int depth, Hit& h) {
    const Event* e = h.event();
    if (e->type() == Event::down && _grabber != nil) {
        h.begin(depth, this, 0, _grabber);
        MonoGlyph::pick(c, a, depth, h);
        h.end();
    }
}


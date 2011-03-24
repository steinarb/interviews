#include "figure.h"
#include "pacer.h"
#include "pacemaker.h"
#include "pacekeeper.h"
#include "gcollector.h"

#include <InterViews/action.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/display.h>
#include <InterViews/brush.h>
#include <InterViews/geometry.h>
#include <InterViews/transformer.h>
#include <InterViews/window.h>
#include <IV-X11/xcanvas.h>

#include <OS/math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Coord Area(Coord l, Coord b, Coord r, Coord t) {
    return (r-l)*(t-b);
}

static boolean damaged (BoxObj& box) {
    return !(
        box._l == 0 && box._b == 0 && box._r == 0 && box._t == 0
    );
}

static void clear (BoxObj& box) {
    box._l = 0;
    box._b = 0;
    box._r = 0;
    box._t = 0;
}

static const short UniLevel = 0;
static const long SLACK = 1000;

/**********************************************************/
EventMapper::EventMapper () {}
EventMapper::~EventMapper () {}

/*****************************************************************************/

class DragImpl {
public:
    DragImpl (USec, USec, long, long, float);
    virtual ~DragImpl();

    USec init(Event&);
    USec map(Event&);
    USec relax();
public:
    USec _high;
    USec _low;
    float _relax;

    USec _cur_rate;

    long _hspeed;
    long _lspeed;

    Event _last;
};

DragImpl::DragImpl (USec high, USec low, long l, long h, float relax) {
    _high = high;
    _low = low;
    _cur_rate = _high;
    _relax = relax;

    _hspeed = h;
    _lspeed = l;

    _last.window(nil);
}
    
DragImpl::~DragImpl () {}

USec DragImpl::init (Event& e) {
    _last = e;
    return _high;
}

USec DragImpl::map (Event& e) {
    USec value = _cur_rate;
    if (_last.window() != nil) {
        Coord distx = e.pointer_x()-_last.pointer_x();
        Coord disty = e.pointer_y()-_last.pointer_y();
        Coord total_dist = Math::abs(distx)+Math::abs(disty);
        unsigned long len = e.time() - _last.time();

        long speed;
        if (len > 0) {
            speed = total_dist*1e6/len;
        } else {
            speed = _hspeed + 1;
        }
        
        if (speed > _hspeed) {
            value = _low;
            
        } else if (speed < _lspeed) {
            value = _high;
            
        } else {
            float factor = ((float)(speed-_lspeed))/(_hspeed-_lspeed);
            value = (USec) (_high - factor*(_high-_low));
        }
    }
    _last = e;
    if (value > _cur_rate) {
        value = _cur_rate;
    } else {
        _cur_rate = value;
    }

    return value;
}    

USec DragImpl::relax () {
    if (_cur_rate < _high) {
        _cur_rate *= _relax;
    }
    if (_cur_rate > _high) {
        _cur_rate = _high;
    }
    return _cur_rate;
}

/*****************************************************************************/

DragMapper::DragMapper (USec high, USec low, long l, long h, float relax) {
    _impl = new DragImpl(high, low, l, h, relax);
}

DragMapper::~DragMapper () {
    delete _impl;
}

USec DragMapper::init (Event& e) {
    return _impl->init(e);
}

USec DragMapper::map (Event& e) {
    return _impl->map(e);
}

USec DragMapper::relax () {
    return _impl->relax();
}

/*****************************************************************************/
class ContImpl {
public:
    ContImpl( USec, USec, Coord, Coord, ContDimension);
    virtual ~ContImpl();

    USec init(Event&);
    USec map(Event&);
    USec relax();

    void set_dimension(ContDimension);
public:
    USec _high;
    USec _low;
    USec _cur_rate;
    Coord _short;
    Coord _long;
    ContDimension _dimension;

    Event _last;
};

ContImpl::ContImpl (USec high, USec low, Coord s, Coord l, ContDimension d) {
    _high = high;
    _low = low;

    _short = s;
    _long = l;
    _cur_rate = _high;
    _dimension = d;
    _last.window(nil);
}
    
ContImpl::~ContImpl () {}

USec ContImpl::init (Event& e) {
    _last = e;
    return _high;
}

void ContImpl::set_dimension(ContDimension d) {
    _dimension = d;
}

USec ContImpl::map (Event& e) {
    USec value = _cur_rate;
    if (_last.window() != nil) {
        Coord distx = e.pointer_x()-_last.pointer_x();
        Coord disty = e.pointer_y()-_last.pointer_y();
        Coord total_dist;

        if (_dimension == Cont_XY) {
            total_dist = Math::abs(distx)+Math::abs(disty);

        } else if (_dimension == Cont_Y) {
            total_dist = Math::abs(disty);

        } else {
            total_dist = Math::abs(distx);
        }
        if (total_dist > _long) {
            value = _low;

        } else if (total_dist < _short) {
            value = _high;

        } else {
            float factor = (total_dist-_short)/(_long-_short);
            value = (USec) (_high - factor*(_high-_low));
        }
    }
    _cur_rate = value;
    return value;
}    

USec ContImpl::relax () {
    return _cur_rate;
}

/*****************************************************************************/
ContMapper::ContMapper (
    USec high, USec low, Coord s, Coord l, ContDimension dimension
) {
    _impl = new ContImpl(high, low, s, l, dimension);
}

ContMapper::~ContMapper () {
    delete _impl;
}

USec ContMapper::init (Event& e) {
    return _impl->init(e);
}

USec ContMapper::map (Event& e) {
    return _impl->map(e);
}

USec ContMapper::relax () {
    return _impl->relax();
}

void ContMapper::set_dimension(ContDimension d) {
    _impl->set_dimension(d);
}

/*****************************************************************************/

Pacer::Pacer () { _parent = nil; }

Pacer::~Pacer () {}

void Pacer::append (Pacer*) {}

void Pacer::prepend (Pacer*) {}

void Pacer::insert(PacerIndex, Pacer*) {}

void Pacer::remove (PacerIndex) {}

void Pacer::remove_all () {}

void Pacer::delete_all () {}

PacerIndex Pacer::count () const { return 0; }

Pacer* Pacer::child (PacerIndex) const { return nil; }

Pacer* Pacer::get_parent () const { return _parent; }

void Pacer::set_parent (Pacer* p) { _parent = p; }

void Pacer::regrade () {}

void Pacer::pick(SchedulerList&) {}

/**********************************************************/

PolyPacer::PolyPacer () {
    _child_list = new PacerList(10);
}

PolyPacer::~PolyPacer () {
    for (int i = 0; i < count(); i++) {
        Pacer* c = child(i);
        delete c;
    }
    delete _child_list;
}

void PolyPacer::pick(SchedulerList& plist) {
    for (PacerIndex i = 0; i < count(); i++) {
        Pacer* p = child(i);
        p->pick(plist);
    }
}

void PolyPacer::regrade () {
    for (PacerIndex i = 0; i < count(); i++) {
        child(i)->regrade();
    }
}

void PolyPacer::append (Pacer* gp) {
    if (gp != nil) {
        _child_list->append(gp);
        gp->set_parent(this);
    }
}

void PolyPacer::prepend (Pacer* gp) {
    if (gp != nil) {
        _child_list->prepend(gp);
        gp->set_parent(this);
    }
}

void PolyPacer::insert(PacerIndex i, Pacer* gp) {
    if (gp != nil) {
        _child_list->insert(i, gp);
        gp->set_parent(this);
    }
}

void PolyPacer::remove (PacerIndex i) {
    Pacer* p = child(i);
    if (p != nil) {
        _child_list->remove(i);
        p->set_parent(nil);
    }
}

void PolyPacer::remove_all () {
    for (PacerIndex i = 0; i < count(); i++) {
        Pacer* p = child(i);
        p->set_parent(nil);
    }
    _child_list->remove_all();
}

void PolyPacer::delete_all () {
    for (PacerIndex i = 0; i < count(); i++) {
        Pacer* p = child(i);
        delete p;
    }
    _child_list->remove_all();
}

PacerIndex PolyPacer::count () const {
    return _child_list->count();
}

Pacer* PolyPacer::child (PacerIndex i) const {
    return _child_list->item(i);
}

/**********************************************************/

Grader::Grader (GradeCollector* g) { 
    _collector = g;
    if (_collector != nil) {
        _collector->set_pacer(this);
    }
}

Grader::~Grader () {
    delete _collector;
}

GradeCollector* Grader::get_collector () const { return _collector; }

void Grader::set_collector (GradeCollector* g) { 
    if (g != _collector) {
        delete _collector;
        _collector = g; 
        if (_collector != nil) {
            _collector->set_pacer(this);
        }
    }
}

void Grader::play (RunTime& rtime) {
    GradeCollector* collector = get_collector();
    if (collector != nil) {
        collector->play(rtime);
    }
}

void Grader::get_grades(Grade& low, Grade& high) {
    GradeCollector* collector = get_collector();
    if (collector != nil) {
        Grade l, h;
        collector->get_grades(l, h);
        low = l; high = h;
    }
}

void Grader::regrade () {
    GradeCollector* collector = get_collector();
    if (collector != nil) {
        collector->regrade();
    }
}

/**********************************************************/

UniPacer::UniPacer () { _kid = nil; }
    
UniPacer::~UniPacer () { delete _kid; }

Pacer* UniPacer::get_kid () const { return _kid; }

void UniPacer::set_kid (Pacer* k) { 
    if (_kid != k) {
        if (_kid != nil) {
            _kid->set_parent(nil);
        }
        _kid = k; 
        if (_kid != nil) {
            _kid->set_parent(this);
        }
    }
}

void UniPacer::pick(SchedulerList& plist) {
    if (_kid != nil) {
        _kid->pick(plist);
    }
}

void UniPacer::regrade () {
    if (_kid != nil) {
        _kid->regrade();
    }
}

void UniPacer::append (Pacer* p) {
    set_kid(p);
}

void UniPacer::prepend (Pacer* p) {
    set_kid(p);
}

void UniPacer::insert (PacerIndex i, Pacer* p) {
    if (i == 0) {
        set_kid(p);
    }
}

void UniPacer::remove (PacerIndex i) {
    if (i == 0) {
        if (_kid != nil) {
            _kid->set_parent(nil);
        }
        _kid = nil;
    }
}

void UniPacer::remove_all () {
    remove(0);
}

void UniPacer::delete_all () {
    delete _kid;
    _kid = nil;
}

PacerIndex UniPacer::count () const {
    if (_kid == nil) {
        return 0;
    } else {
        return 1;
    }
}

Pacer* UniPacer::child (PacerIndex i) const {
    if (i == 0) {
        return _kid;
    } else {
        return nil;
    }
}

void UniPacer::play (RunTime& rtime) {
    if (_kid != nil) {
        _kid->play(rtime);
    }
}

void UniPacer::get_grades (Grade& low, Grade& high) {
    if (_kid != nil) {
        Grade l, h;
        _kid->get_grades(l, h);
        low = l; high = h;
    } else {
        low = high = 0;
    }
}

/**********************************************************/
Scheduler::Scheduler (USec period) {
    _period = period;
}

void Scheduler::set_period (USec period) {
    if (period != _period) {
        _period = period;
        PaceMaker* maker = PaceMaker::instance();
        maker->get_pacekeeper()->adjust(this);
    }
}

USec Scheduler::get_period () { return _period; }

void Scheduler::pick (SchedulerList& plist) {
    plist.append(this);
}

void Scheduler::start (Sec, USec) {}

void Scheduler::stop (Sec, USec) {}

void Scheduler::play (RunTime& rtime) {
    Grade low, high;
    get_grades(low, high);

    FadeType f = fadetype(
        rtime._cur_grade, rtime._fut_grade, low, high
    );
    
    if (f != no_fade) {
        //USec slack = SLACK;
        PaceMaker::instance()->cont(this, _period);
        UniPacer::play(rtime);
        //PaceMaker::instance()->cont(this, slack);
    }
}
    
/**********************************************************/
UIScheduler::UIScheduler (
    EventMapper* emapper
) : Scheduler(-1) {
    _emapper = emapper;
    _playing = false;
}

UIScheduler::~UIScheduler () {
    delete _emapper;
}

void UIScheduler::start (Sec, USec) {
    _playing = true;
}

void UIScheduler::stop (Sec, USec) {
    _playing = false;
}

void UIScheduler::play (RunTime& rtime) {
    if (_playing) {
        Scheduler::play(rtime);
        set_period(_emapper->relax());
    }
}

void UIScheduler::cur_event (Event& e) {
    if (!_playing) {
        set_period(_emapper->init(e));
    } else {
        set_period(_emapper->map(e));
        PaceMaker::instance()->cont(this, -1);
    }
}

void UIScheduler::set_mapper (EventMapper* emapper) {
    delete _emapper;
    _emapper = emapper;
}

/**********************************************************/
ActionPacer::ActionPacer (Action* before, Action* after) {
    _before = before;
    _after = after;
    Resource::ref(_before);
    Resource::ref(_after);
}

ActionPacer::~ActionPacer () {
    Resource::unref(_before);
    Resource::unref(_after);
}

void ActionPacer::set_before (Action* before) {
    Resource::ref(before);
    Resource::unref(_before);
    _before = before;
}

void ActionPacer::set_after (Action* after) {
    Resource::ref(after);
    Resource::unref(_after);
    _after = after;
}

void ActionPacer::play (RunTime& rtime) {
    if (_before != nil) {
        _before->execute();
    }
    UniPacer::play(rtime);
    if (_after != nil) {
        _after->execute();
    }
}

/**********************************************************/
GraphicPacer::GraphicPacer (Canvas* c, Graphic* gr) {
    _gr = gr;
    _canvas = c;
    Resource::ref(_gr);
    _low = _high = UniLevel;
}

GraphicPacer::~GraphicPacer () {
    Resource::unref(_gr);
}

BoxObj GraphicPacer::_damage;

BoxObj& GraphicPacer::damaged_area () { return _damage; }

void GraphicPacer::incur (BoxObj& box) {
    if (!damaged(_damage)) {
        _damage = box;
    } else {
        _damage = _damage + box;
    }
}

void GraphicPacer::clear () {
    ::clear(_damage);
}

void GraphicPacer::get_grades (Grade& low, Grade& high) {
    low = _low;
    high = _high;
}

void GraphicPacer::set_grades (Grade low, Grade high) {
    _low = low;
    _high = high;
}

void GraphicPacer::set_graphic (Graphic* gr) { 
    Resource::ref(gr);
    Resource::unref(_gr);
    _gr = gr; 
}

void GraphicPacer::play (RunTime& rtime) {
    if (_canvas == nil) {
        return;
    }

    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    FadeType f = fadetype(cur, fut, _low, _high);

    boolean to_draw = false;

    switch (f) {
    case sustain: 
        {
            to_draw = true;
        }
        break;
    case fadein_low:
    case fadein_high:
        {
            //_gr->alpha_stroke(true);
            //_gr->alpha_fill(true);
            to_draw = true;
        }
        break;
    case fadeout_low:
    case fadeout_high:
        {
/*
            Coord l, b, r, t;
            _gr->getbounds(l, b, r, t);
            if ((r-l)*(t-b) < 100000.0) {
                _gr->alpha_stroke(true);
                _gr->alpha_fill(true);
                to_draw = true;
            }
*/
        }
        break;
    case no_fade:
        break;
    }
    if (to_draw) {
        _gr->drawclipped(
            _canvas, _damage._l, _damage._b, _damage._r, _damage._t
        );
    }
    _gr->alpha_stroke(false);
    _gr->alpha_fill(false);
}

/**********************************************************/

Repairer::Repairer (
    Canvas* c, Graphic* gr
) : GraphicPacer(c, gr) {}

void Repairer::incur (BoxObj& box) {
    if (!damaged(_total)) {
        _total = box;
    } else {
        _total = _total + box;
    }
}

void Repairer::clear () {
    ::clear(_total);
}

BoxObj& Repairer::damaged_area () { return _total; }

/**********************************************************/

Filler::Filler (
    Canvas* c, Graphic* gr, const Color* fill
) : Repairer(c, gr) {
    _fill = fill;
    Resource::ref(_fill);
}

Filler::~Filler () {
    Resource::unref(_fill);
}    

void Filler::play (RunTime& rtime) {
    if (_canvas == nil) {
        return;
    }

    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    FadeType f = fadetype(cur, fut, _low, _high);
    if (f != no_fade) {
        if (damaged(_total)) {
            _canvas->fill_rect(
                _total._l, _total._b, _total._r, _total._t, _fill
            );
            ::clear(_total);
        }
    } 
}

/**********************************************************/

FCGraphicPacer::FCGraphicPacer (
    Canvas* c, Graphic* gr, const Color* fill
) : Filler(c, gr, fill) {}

FCGraphicPacer::~FCGraphicPacer () {}    

void FCGraphicPacer::play (RunTime& rtime) {
    if (_canvas == nil) {
        return;
    }

    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    FadeType f = fadetype(cur, fut, _low, _high);
    if (f == fadein_low || f == fadein_high || f == sustain) {
        if (damaged(_total)) {
            CanvasRep* crep = _canvas->rep();
            BoxObj box = _clip-_total;

            _canvas->damage(box._l, box._b, box._r, box._t);
            crep->start_repair();
            _canvas->fill_rect(
                _total._l, _total._b, _total._r, _total._t, _fill
            );
            _gr->drawclipped(
                _canvas, _total._l, _total._b, _total._r, _total._t
            );
            _gr->alpha_fill(false);
            _gr->alpha_stroke(false);

            crep->finish_repair();
            _canvas->window()->display()->sync();

            ::clear(_total);
        }
    }
}

/**********************************************************/

Fixer::Fixer (Canvas* c, Graphic* gr) : Repairer(c, gr) {}

void Fixer::play (RunTime& rtime) {
    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    FadeType f = fadetype(cur, fut, _low, _high);
    if (f != no_fade) {
        _gr->drawclipped(
            _canvas, _total._l, _total._b, _total._r, _total._t
        );
        ::clear(_total);
    }
}
/**********************************************************/
SelectPacer::SelectPacer (Canvas* c, Graphic* gr) {
    _canvas = c;
    _gr = gr;
    Resource::ref(_gr);
}

SelectPacer::~SelectPacer () {
    Resource::unref(_gr);
}

void SelectPacer::set_graphic (Graphic* gr) { 
    Resource::ref(gr);
    Resource::unref(_gr);
    _gr = gr; 
}

void SelectPacer::play (RunTime& rtime) {
    if (!damaged(GraphicPacer::damaged_area())) {
        return;
    }
    _gr->alpha_fill(true);
    _gr->alpha_stroke(true);

    UniPacer::play(rtime);
    
    _gr->alpha_fill(false);
    _gr->alpha_stroke(false);

    GraphicPacer::clear();
}
/**********************************************************/
Clipper::Clipper (Canvas* c, BoxObj* box) {
    _canvas = c;
    _box = box;
}

Clipper::~Clipper () {}

void Clipper::play (RunTime& rtime) {
    if (damaged(*_box)) {
        CanvasRep* crep = _canvas->rep();
        BoxObj box = _allot-*_box;
        
        _canvas->damage(box._l, box._b, box._r, box._t);
        crep->start_repair();
        
        UniPacer::play(rtime);
        
        crep->finish_repair();
        _canvas->window()->display()->sync();
        
        GraphicPacer::clear();
    }
}

/**********************************************************/
XorPacer::XorPacer (
    Canvas* c, Graphic* g, const Color* color
) : GraphicPacer(c, g) {
    _color = new Color(*color, 1.0, Color::Xor);
    _color->ref();
    _brush = new Brush(0.0);
    _brush->ref();
    _drawn = false;
    
    _t = nil;
    set_graphic(g);
}

XorPacer::~XorPacer () {
    Resource::unref(_color);
    Resource::unref(_brush);
    Resource::unref(_t);
}

void XorPacer::set_graphic (Graphic* g) {
    GraphicPacer::set_graphic(g);
    if (g != nil) {
        Resource::unref(_t);
        _t = new Transformer;
        _gr->eqv_transformer(*_t);
        
        if (_gr->count_() > 0) {
            _gr->getbounds(_box._l, _box._b, _box._r, _box._t);
            _t->inverse_transform(_box._l, _box._b);
            _t->inverse_transform(_box._r, _box._t);
        } else {
            _gr->get_max_min(_box._l, _box._b, _box._r, _box._t);
        }
    }
}

void XorPacer::play (RunTime& rtime) {
    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    FadeType f = fadetype(cur, fut, _low, _high);
    if (f == fadeout_low || f == fadeout_high) {
        if (_drawn) {
            printf("fade out\n");
            xor();
        }
    } else if (f == fadein_low || f == fadein_high) {
        if (!_drawn) {
            _gr->eqv_transformer(*_t);
            xor();
            GraphicPacer::clear();
        }
    } else if (f != no_fade) {
        if (_drawn) {
            xor();
        }
        _gr->eqv_transformer(*_t);
        xor();
        GraphicPacer::clear();
    }
}

void XorPacer::xor () {
    boolean doit = false;
    if (!_drawn) {
        if (damaged(_damage)) {
            _drawn = true;
            doit = true;
        }
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
        _canvas->window()->display()->sync();
    }
}

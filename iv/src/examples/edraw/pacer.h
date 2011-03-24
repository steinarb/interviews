#ifndef pacer_h
#define pacer_h

#include <InterViews/event.h>
#include "globals.h"

class Action;
class Brush;
class Canvas;
class Color;
class ContImpl;
class Graphic;
class GradeCollector;
class Transformer;
class DragImpl;

static const long SlowRate = 1e6;
static const long FastRate = 50e3;
static const long RelaxRate = 5e3;

static const long LowSpeed = 10000;
static const long HighSpeed = 1000000;

static const float ShortDist = 10.0;
static const float LongDist = 50.0;

class EventMapper {
public:
    virtual ~EventMapper();

    virtual USec init(Event&) = 0;
    virtual USec map(Event&) = 0;
    virtual USec relax() = 0;
protected:
    EventMapper();
};

class DragMapper : public EventMapper {
public:
    DragMapper(
        USec = SlowRate, USec = FastRate, long = LowSpeed, long = HighSpeed,
        float _relax_factor = 1.15
    );
    virtual ~DragMapper();

    virtual USec init(Event&);
    virtual USec map(Event&);
    virtual USec relax();
private:
    DragImpl* _impl;
};

typedef unsigned int ContDimension;

enum {
    Cont_X = 0, Cont_Y, Cont_XY
};

class ContMapper : public EventMapper {
public:
    ContMapper(
        USec = SlowRate, USec = FastRate,
        Coord = ShortDist, Coord = LongDist, ContDimension = Cont_XY
    );
    virtual ~ContMapper();

    virtual USec init(Event&);
    virtual USec map(Event&);
    virtual USec relax();

    void set_dimension(ContDimension);

private:
    ContImpl* _impl;
};

class Pacer {
public:
    virtual ~Pacer();
    
    virtual void append(Pacer*);
    virtual void prepend(Pacer*);
    virtual void insert(PacerIndex, Pacer*);
    virtual void remove(PacerIndex);
    virtual void remove_all();
    virtual void delete_all();
    virtual PacerIndex count() const;
    virtual Pacer* child(PacerIndex) const;

    virtual Pacer* get_parent() const;
    virtual void set_parent(Pacer*);

    virtual void play(RunTime&) = 0;
    virtual void pick(SchedulerList&);

    virtual void get_grades(Grade& low, Grade& high) = 0;
    virtual void regrade();

protected:
    Pacer();
protected:
    Pacer* _parent;
};

class PolyPacer : public Pacer {
public:
    virtual ~PolyPacer();

    virtual void play(RunTime&) = 0;
    virtual void get_grades(Grade& low, Grade& high) = 0;

    virtual void pick(SchedulerList&);
    virtual void regrade();

    virtual void append(Pacer*);
    virtual void prepend(Pacer*);
    virtual void insert(PacerIndex, Pacer*);
    virtual void remove(PacerIndex);
    virtual void remove_all();
    virtual void delete_all();
    virtual PacerIndex count() const;
    virtual Pacer* child(PacerIndex) const;
protected:
    PolyPacer();
protected:
    PacerList* _child_list;
};

class Grader : public PolyPacer {
public:
    Grader(GradeCollector* = nil);
    virtual ~Grader();

    GradeCollector* get_collector() const;
    void set_collector(GradeCollector*);

    virtual void play(RunTime&);

    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();
protected:
    GradeCollector* _collector;
};

class UniPacer : public Pacer {
public:
    UniPacer();
    virtual ~UniPacer();

    virtual Pacer* get_kid() const;
    virtual void set_kid(Pacer*);

    virtual void append(Pacer*);
    virtual void prepend(Pacer*);
    virtual void insert(PacerIndex, Pacer*);
    virtual void remove(PacerIndex);
    virtual void remove_all();
    virtual void delete_all();
    virtual PacerIndex count() const;
    virtual Pacer* child(PacerIndex) const;

    virtual void play(RunTime&);
    virtual void regrade();

    virtual void pick(SchedulerList&);
    virtual void get_grades(Grade& low, Grade& high);
protected:
    Pacer* _kid;
};

class Scheduler : public UniPacer {
public: 
    Scheduler(USec period);

    virtual void set_period(USec period);
    virtual USec get_period();

    virtual void start(Sec, USec);
    virtual void stop(Sec, USec);

    virtual void play(RunTime&);

    virtual void pick(SchedulerList&);
protected:
    USec _period;
};    

class UIScheduler : public Scheduler {
public:
    UIScheduler(EventMapper*);
    virtual ~UIScheduler();

    virtual void start(Sec, USec);
    virtual void stop(Sec, USec);

    virtual void play(RunTime&);
    virtual void cur_event(Event&);

    void set_mapper(EventMapper*);
protected:
    EventMapper* _emapper;
    boolean _playing;

    USec _event_usec;
    Sec _event_sec;
};

class ActionPacer : public UniPacer {
public:
    ActionPacer(Action* before = nil, Action* after = nil);
    virtual ~ActionPacer();
    
    virtual void play(RunTime&);

    void set_before(Action*);
    void set_after(Action*);
protected:
    Action* _before;
    Action* _after;
};

class GraphicPacer : public Pacer {
public:
    GraphicPacer(Canvas*, Graphic*);
    virtual ~GraphicPacer();


    virtual Graphic* get_graphic() const;
    virtual void set_graphic(Graphic*);
    void set_grades(Grade low, Grade high);

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);

    static BoxObj& damaged_area();
    static void incur(BoxObj&);
    static void clear();
protected:
    static BoxObj _damage;
protected:
    Canvas* _canvas;
    Graphic* _gr;
    Grade _low, _high;
};

inline Graphic* GraphicPacer::get_graphic () const { return _gr; }

class Repairer : public GraphicPacer {
public:
    virtual void incur(BoxObj&);
    virtual void clear();
    virtual BoxObj& damaged_area();

    virtual void play(RunTime&) = 0;
protected:
    Repairer(Canvas*, Graphic*);
protected:
    BoxObj _total;
};

class Filler : public Repairer {
public:
    Filler(Canvas*, Graphic*, const Color* fill);
    virtual ~Filler();

    virtual void play(RunTime&);
protected:
    const Color* _fill;
};    

class FCGraphicPacer : public Filler {
public:
    FCGraphicPacer(Canvas*, Graphic*, const Color* fill);
    virtual ~FCGraphicPacer();

    void set_clipped (BoxObj&);

    virtual void play(RunTime&);
protected:
    BoxObj _clip;
};

inline void FCGraphicPacer::set_clipped(BoxObj& clip) { _clip = clip; }

class Fixer : public Repairer {
public:
    Fixer(Canvas*, Graphic*);

    virtual void play(RunTime&);
};    
    
class SelectPacer : public UniPacer {
public:
    SelectPacer(Canvas*, Graphic*);
    virtual ~SelectPacer();

    void set_damaged(BoxObj*);
    void set_clipped (BoxObj&);
    void set_graphic(Graphic*);

    virtual void play(RunTime&);
protected:
    Canvas* _canvas;
    Graphic* _gr;
    BoxObj* _box;
    BoxObj _allot;
};    
    
inline void SelectPacer::set_damaged(BoxObj* box) { _box = box; }
inline void SelectPacer::set_clipped(BoxObj& allot) { _allot = allot; }


class Clipper : public UniPacer {
public:
    Clipper(Canvas*, BoxObj*);
    virtual ~Clipper();

    void set_damaged(BoxObj*);
    void set_clipped (BoxObj&);

    virtual void play(RunTime&);
protected:
    Canvas* _canvas;
    BoxObj* _box;
    BoxObj _allot;
};    
    
inline void Clipper::set_damaged(BoxObj* box) { _box = box; }
inline void Clipper::set_clipped(BoxObj& allot) { _allot = allot; }


class XorPacer : public GraphicPacer {
public:
    XorPacer(Canvas*, Graphic*, const Color*);
    virtual ~XorPacer();

    virtual void set_graphic(Graphic*);
    void set_clipped (BoxObj&);

    virtual void play(RunTime&);
protected:
    void xor();
protected:
    Color* _color;
    Brush* _brush;
    BoxObj _box;
    BoxObj _allot;
    Transformer* _t;
    boolean _drawn;
};

inline void XorPacer::set_clipped(BoxObj& allot) { _allot = allot; }

class Player : public PolyPacer {
public:
    Player(Pacer*, TimeSlice&);

    virtual void play(RunTime&);
protected:
    Pacer* _actor;
    TimeSlice _schedule;
};
    
#endif

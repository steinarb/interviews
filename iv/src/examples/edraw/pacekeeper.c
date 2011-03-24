#include "pacekeeper.h"
#include "pacemaker.h"
#include "pacer.h"
#include <OS/list.h>
#include <sys/time.h>
#include <stdio.h>

static Sec MaxTransit = 30000;
static SessionID globalID = 0;

typedef unsigned int ImplState;

static const unsigned int TOL = 0;
static const unsigned int MIN_CYCLE = 2;
static const unsigned int MAX_CYCLE = 10;
static const unsigned int USEC_TO_CYCLE = 10000;

static long find_player(Scheduler* s, SchedulerList* plist) {
    for (long i = 0; i < plist->count(); i++) {
        if (s == plist->item(i)) {
            return i;
        }
    }
    return -1;
}

class CCell {
public:
    CCell();
public:
    float _coherence;

    USec _cpf;
    USec _tleft;
    USec _tdelta;
};

CCell::CCell () { 
    _coherence = 1.0;
    _cpf = 0;
    _tleft = 0;
    _tdelta = 0;
}

declareList(CCellList, CCell);
implementList(CCellList, CCell);

declarePtrList(PlayList, CCellList);
implementPtrList(PlayList, CCellList);

/********************************************************************/
PaceKeeper::PaceKeeper () {}

PaceKeeper::~PaceKeeper () {}

void PaceKeeper::eval (Sec, USec) {}
void PaceKeeper::adjust (Scheduler*) {}

/********************************************************************/
class ContinueImpl {
public:
    ContinueImpl(Pacer* root);
    virtual ~ContinueImpl();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);
public:
    Pacer* _root;
    Grade _low, _high;
    SchedulerList* _playerlist;
    boolean _running;
};

ContinueImpl::ContinueImpl (Pacer* root) {
    _root = root;
    _playerlist = nil;
    _running = false;
}

ContinueImpl::~ContinueImpl () {
    delete _playerlist;
}

void ContinueImpl::startall (Sec sec, USec usec) {
    globalID++;
    _running = true;
    if (_playerlist == nil) {
        _playerlist = new SchedulerList(5);
        _root->pick(*_playerlist);
        _root->get_grades(_low, _high);
    }
    for (long k = 0; k < _playerlist->count(); k++) {
        Scheduler* player = _playerlist->item(k);
        player->start(sec, usec);
        play(player, sec, usec);
    }
}

void ContinueImpl::stopall (Sec sec, USec usec) {
    _running = false;
    if (_playerlist != nil) {
        for (long k = 0; k < _playerlist->count(); k++) {
            Scheduler* player = _playerlist->item(k);
            player->stop(sec, usec);
        }
    }
}

void ContinueImpl::start (Scheduler* p, Sec sec, USec usec) {
    if (!_running) {
        _running = true;
        globalID++;
    }

    p->start(sec, usec);
    play(p, sec, usec);
}

void ContinueImpl::stop (Scheduler* p, Sec sec, USec usec) {
    p->stop(sec, usec);
}

void ContinueImpl::play (Scheduler* s, Sec sec, USec usec) {
    RunTime rtime;

    rtime._cur_grade = _low;
    rtime._fut_grade = _low;
    rtime._id = globalID;

    USec period = s->get_period();

    rtime._period = period;
    rtime._sec = sec;
    rtime._usec = usec;

    s->play(rtime);
}
    
/********************************************************************/
Continuator::Continuator (Pacer* root) {
    _impl = new ContinueImpl(root);
}

Continuator::~Continuator () {
    delete _impl;
}

void Continuator::startall (Sec sec, USec usec) {
    _impl->startall(sec, usec);
}

void Continuator::stopall (Sec sec, USec usec) {
    _impl->stopall(sec, usec);
}

void Continuator::start (Scheduler* s, Sec sec, USec usec) {
    _impl->start(s, sec, usec);
}

void Continuator::play (Scheduler* s, Sec sec, USec usec) {
    _impl->play(s, sec, usec);
}

void Continuator::stop (Scheduler* s, Sec sec, USec usec) {
    _impl->stop(s, sec, usec);
}

/*******************************************************************/
class EventImpl {
public:
    enum {sustain, transit, trial};

    EventImpl(Pacer* r);
    virtual ~EventImpl();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);
    virtual void eval(Sec, USec);
    virtual void adjust(Scheduler*);
protected:
    Pacer* _root;
    SchedulerList* _playerlist;
    PlayList* _playlist;

    ImplState _fut_state;
    ImplState _cur_state;

    Grade _cur_grade;
    Grade _fut_grade;
    Grade _low, _high;

    boolean _running;
};

EventImpl::EventImpl (Pacer* r) {
    _root = r;
    _running = false;
    _playerlist = nil;
    _playlist = nil;
}

EventImpl::~EventImpl () {
    delete _playerlist;
    if (_playlist != nil) {
        for (long i = 0; i < _playlist->count(); i++) {
            delete _playlist->item(i);
        }
        delete _playlist;
    }
}

void EventImpl::stopall (Sec sec, USec usec) {
    _running = false;
    if (_playerlist != nil) {
        for (long k = 0; k < _playerlist->count(); k++) {
            Scheduler* player = _playerlist->item(k);
            player->stop(sec, usec);
        }
    }
}

void EventImpl::startall (Sec sec, USec usec) {
    _running = true;
    globalID++;

    if (_playerlist == nil) {
        _playerlist = new SchedulerList(5);
        _root->get_grades(_low, _high);
        _root->pick(*_playerlist);
        
        long grades = _high - _low + 1;
        
        CCell cell;
        
        _playlist = new PlayList(_playerlist->count());
        for (long i = 0; i < _playerlist->count(); i++) {
            CCellList* clist = new CCellList(grades);
            _playlist->append(clist);
            for (long j = 0; j < grades; j++) {
                clist->append(cell);
            }
        }
    }
    _cur_grade = _low;
    _fut_grade = _low;
    _cur_state = sustain;
    _fut_state = sustain;
    
    RunTime rtime;
        
    rtime._cur_grade = _cur_grade;
    rtime._fut_grade = _fut_grade;
    rtime._id = globalID;
    rtime._sec = sec;
    rtime._usec = usec;
        
    for (long k = 0; k < _playerlist->count(); k++) {
        Scheduler* player = _playerlist->item(k);
        player->start(sec, usec);

        USec period = player->get_period();
        rtime._period = period;
        
        player->play(rtime);
    }
}    

void EventImpl::play (Scheduler* p, Sec sec, USec usec) {
    RunTime rtime;

    rtime._cur_grade = _cur_grade;
    rtime._fut_grade = _fut_grade;
    rtime._id = globalID;

    USec period = p->get_period();

    rtime._period = period;
    rtime._sec = sec;
    rtime._usec = usec;

    timeval b, a;
    gettimeofday(&b, nil);

    p->play(rtime);

    gettimeofday(&a, nil);
    USec len = (a.tv_usec-b.tv_usec)+(a.tv_sec-b.tv_sec)*1e6;
    
    long index = find_player(p, _playerlist);
    
    if (index >= 0) {
        CCell& cc = _playlist->item(index)->item_ref(_fut_grade);
        USec cpf = len;
        
        boolean to_eval = false;
        if (cc._cpf > 0) {
            cc._tdelta += cpf - cc._cpf;
        } else {
            to_eval = true;
        }
        cc._cpf = cpf;
        cc._tleft += rtime._period-len;
        if (to_eval && cc._tleft < 0) {
            _cur_state = _fut_state;
            _fut_state = sustain;
            eval(a.tv_sec, a.tv_usec);
        }
    }
}

void EventImpl::eval (Sec, USec) {
    if (!_running) {
        return;
    }
    switch(_fut_state) {
    case trial:
        {
            _cur_state = _fut_state;
            
            /*  See if need to downgrade  */
            if (_fut_grade < _high) {
                for (long i = 0; i < _playlist->count(); i++) {
                    CCell& cc = _playlist->item(i)->item_ref(_fut_grade);
                    
                    if (cc._tleft < 0) {
                        for (long j = 0; j < _playlist->count(); j++) {
                            CCell& cell0 = _playlist->item(j)->item_ref(
                                _fut_grade
                            );
                            cell0._tleft = 0;
                            cell0._tdelta = 0;
                            
                            CCell& cell1 = _playlist->item(j)->item_ref(
                                _fut_grade+1
                            );
                            cell1._tleft = 0;
                            cell1._tdelta = 0;
                            cell1._cpf = 0;
                        }
                        _fut_state = transit;
                        break;
                    }
                }
            }
            if (_fut_state != transit) {
                _fut_state = sustain;
                _cur_grade = _fut_grade;
           } else {
                _cur_grade = _fut_grade;
                _fut_grade += 1;
            }
        }
        break;
    case transit:
        {
            if (_cur_state == sustain) {
                _cur_grade = _fut_grade;
                _cur_state = _fut_state;
                if (_fut_grade > _cur_grade) {
                    _fut_state = sustain;
                } else {
                    _fut_state = trial;
                }
            } else {
                _cur_grade = _fut_grade;
                _cur_state = _fut_state;
                _fut_state = sustain;
            }
        }
        break;
    case sustain:
        {
            _cur_state = _fut_state;
            _cur_grade = _fut_grade;
            
            /*  See if need to downgrade  */
            
            if (_fut_grade < _high) {
                for (long i = 0; i < _playlist->count(); i++) {
                    CCell& cc = _playlist->item(i)->item_ref(_cur_grade);
                    
                    if (cc._tleft < 0) {
                        for (long j = 0; j < _playlist->count(); j++) {
                            CCell& cell0 = _playlist->item(j)->item_ref(
                                _cur_grade
                            );
                            cell0._tleft = 0;
                            cell0._tdelta = 0;

                            CCell& cell1 = _playlist->item(j)->item_ref(
                                _cur_grade+1
                            );
                            cell1._tleft = 0;
                            cell1._tdelta = 0;
                            cell1._cpf = 0;
                        }
                        _fut_state = transit;
                        _fut_grade = _cur_grade+1;
                        PaceMaker::instance()->cont_eval(
                            MaxTransit + SAMPLE_PERIOD
                        );
                        return;
                    }
                }
            }
        }
        break;
    }
    if (_fut_state == transit) {
        PaceMaker::instance()->cont_eval(MaxTransit + SAMPLE_PERIOD);
    } else {
        PaceMaker::instance()->cont_eval(SAMPLE_PERIOD);
    }
}

void EventImpl::start (Scheduler* p, Sec sec, USec usec) {
    if (!_running) {
        _running = true;
        globalID++;
    }

    long index = find_player(p, _playerlist);
    if (index >= 0) {
        CCell cell;
        CCellList* clist = _playlist->item(index);
        for (long i = 0; i < clist->count(); i++) {
            clist->item_ref(i) = cell;
        }
    }
    p->start(sec, usec);
    play(p, sec, usec);
}

void EventImpl::stop (Scheduler* p, Sec sec, USec usec) {
    long index = find_player(p, _playerlist);
    if (index >= 0) {
        CCell cell;
        CCellList* clist = _playlist->item(index);
        for (long i = 0; i < clist->count(); i++) {
            clist->item_ref(i) = cell;
        }
    }
    p->stop(sec, usec);
}

void EventImpl::adjust (Scheduler* p) {
    if (_playerlist == nil) {
        return;
    }
    USec period = p->get_period();
    long index = find_player(p, _playerlist);
    if (index < 0) {
        return;
    }
    CCellList* clist = _playlist->item(index);
    Grade level = clist->count()-1;
    
    for (long i = 0; i < clist->count(); i++) {
        if (clist->item_ref(i)._cpf < period) {
            level = i;
            break;
        }
    }
    if (level < _fut_grade) {
        level = _fut_grade - 1;
        //printf("attempted by EventKeeper\n");
    }
    if (level != _fut_grade) {
        _cur_state = _fut_state;
        _fut_state = transit;
        _cur_grade = _fut_grade;
        _fut_grade = level;

        for (long j = 0; j < _playlist->count(); j++) {
            CCell& cell0 = _playlist->item(j)->item_ref(
                _cur_grade
            );
            cell0._tleft = 0;
            cell0._tdelta = 0;
            
            CCell& cell1 = _playlist->item(j)->item_ref(
                _fut_grade
            );
            cell1._tleft = 0;
            cell1._tdelta = 0;
            if (_fut_grade < _cur_grade) {
                cell0._cpf = 0;
                cell1._cpf = 0;
            } else {
                cell1._cpf = 0;
            }
        }
        PaceMaker::instance()->cont_eval(MaxTransit+SAMPLE_PERIOD);
    }
}

/*******************************************************************/
EventKeeper::EventKeeper (Pacer* root) {
    _impl = new EventImpl(root);
}

EventKeeper::~EventKeeper () {
    delete _impl;
}

void EventKeeper::adjust (Scheduler* s) {
    _impl->adjust(s);
}

void EventKeeper::eval (Sec sec, USec usec) {
    _impl->eval(sec, usec);
}

void EventKeeper::startall (Sec sec, USec usec) {
    _impl->startall(sec, usec);
}

void EventKeeper::stopall (Sec sec, USec usec) {
    _impl->stopall(sec, usec);
}

void EventKeeper::start (Scheduler* s, Sec sec, USec usec) {
    _impl->start(s, sec, usec);
}

void EventKeeper::play (Scheduler* s, Sec sec, USec usec) {
    _impl->play(s, sec, usec);
}

void EventKeeper::stop (Scheduler* s, Sec sec, USec usec) {
    _impl->stop(s, sec, usec);
}

/*******************************************************************/
class Cycle {
public:
    Cycle();
public:
    unsigned long _cycle;
    unsigned long _cur_cycle;
};

Cycle::Cycle () { 
    _cycle = MIN_CYCLE; 
    _cur_cycle = 0;
}

declareList (CycleList, Cycle);
implementList (CycleList, Cycle);

class DampedImpl : public EventImpl {
public:
    DampedImpl(Pacer* r);
    virtual ~DampedImpl();

    virtual void startall(Sec, USec);
    virtual void eval(Sec, USec);
    virtual void adjust(Scheduler*);
protected:
    CycleList* _cyclelist;
};

DampedImpl::DampedImpl (Pacer* r) : EventImpl (r) {
    _cyclelist =  nil;
}

DampedImpl::~DampedImpl () {
    delete _cyclelist;
}

void DampedImpl::startall (Sec sec, USec usec) {
    EventImpl::startall(sec, usec);
    if (_cyclelist == nil) {
        Grade grades = _high-_low+1;
        _cyclelist = new CycleList(grades);
        Cycle cycle;
        for (long i = 0; i < grades; i++) {
            _cyclelist->append(cycle);
        }
    }
}

void DampedImpl::eval (Sec sec, USec usec) {
    if (!_running) {
        return;
    }
    switch(_fut_state) {
    case trial:
        {
            _cur_state = _fut_state;
            
            /*  See if need to downgrade  */
            if (_fut_grade < _high) {
                for (long i = 0; i < _playlist->count(); i++) {
                    CCell& cc = _playlist->item(i)->item_ref(_fut_grade);
                    
                    if (cc._tleft < 0) {
                        for (long j = 0; j < _playlist->count(); j++) {
                            CCell& cell0 = _playlist->item(j)->item_ref(
                                _fut_grade
                            );
                            cell0._tleft = 0;
                            cell0._tdelta = 0;
                            
                            CCell& cell1 = _playlist->item(j)->item_ref(
                                _fut_grade+1
                            );
                            cell1._tleft = 0;
                            cell1._tdelta = 0;
                            cell1._cpf = 0;
                            
                        }
                        _fut_state = transit;
                        
                        break;
                    }
                }
            }
            if (_fut_state != transit) {
                //printf("succeeded\n");

                _fut_state = sustain;
                _cur_grade = _fut_grade;
                Cycle& cycle = _cyclelist->item_ref(_fut_grade);
                cycle._cur_cycle = 0;

                if (_fut_grade < _high) {
                    Cycle& past_cycle = _cyclelist->item_ref(_fut_grade+1);
                    if (past_cycle._cycle > MIN_CYCLE) {
                        past_cycle._cycle--;
                    }
                    past_cycle._cur_cycle = 0;
                }
           } else {
               //printf("failed\n");
                _cur_grade = _fut_grade;
                _fut_grade += 1;
                Cycle& cycle = _cyclelist->item_ref(_fut_grade);

                if (cycle._cycle < MAX_CYCLE) {
                    cycle._cycle++;
                }
                cycle._cur_cycle = 0;
            }
        }
        break;
    case transit:
        {
            EventImpl::eval(sec, usec);
            return;
        }
        break;
    case sustain:
        {
            EventImpl::eval(sec, usec);
            if (_fut_state == sustain) {
                /*  See if need to upgrade  */
                
                if (_cur_grade > _low) {
                    Cycle& cycle = _cyclelist->item_ref(_fut_grade);
                    cycle._cur_cycle++;
                    if (cycle._cur_cycle > cycle._cycle) {
                        cycle._cur_cycle = 0;
                        
                        for (long j = 0; j < _playlist->count(); j++) {
                            CCell& cell0 = _playlist->item(j)->item_ref(
                                _cur_grade
                            );
                            cell0._tleft = 0;
                            cell0._tdelta = 0;
                            
                            CCell& cell1 = _playlist->item(j)->item_ref(
                                _cur_grade-1
                            );
                            cell1._tleft = 0;
                            cell1._tdelta = 0;
                        }
                        //printf("attempted by Damped_Osc/PCKeeper\n");
                        _fut_state = transit;
                        _fut_grade = _cur_grade-1;
                    }
                }
            } else {
                return;
            }
        }
        break;
    }
    if (_fut_state == transit) {
        PaceMaker::instance()->cont_eval(MaxTransit + SAMPLE_PERIOD);
    } else {
        PaceMaker::instance()->cont_eval(SAMPLE_PERIOD);
    }
}

void DampedImpl::adjust (Scheduler* p) {
    EventImpl::adjust(p);
    if (_fut_state == transit) {
        Cycle& cycle0 = _cyclelist->item_ref(_cur_grade);
        cycle0._cur_cycle = 0;
        Cycle& cycle1 = _cyclelist->item_ref(_fut_grade);
        cycle1._cur_cycle = 0;
    }
}

/*******************************************************************/
DampedKeeper::DampedKeeper (Pacer* root) {
    _impl = new DampedImpl(root);
}

DampedKeeper::~DampedKeeper () {
    delete _impl;
}

void DampedKeeper::adjust (Scheduler* s) {
    _impl->adjust(s);
}

void DampedKeeper::eval (Sec sec, USec usec) {
    _impl->eval(sec, usec);
}

void DampedKeeper::startall (Sec sec, USec usec) {
    _impl->startall(sec, usec);
}

void DampedKeeper::stopall (Sec sec, USec usec) {
    _impl->stopall(sec, usec);
}

void DampedKeeper::start (Scheduler* s, Sec sec, USec usec) {
    _impl->start(s, sec, usec);
}

void DampedKeeper::play (Scheduler* s, Sec sec, USec usec) {
    _impl->play(s, sec, usec);
}

void DampedKeeper::stop (Scheduler* s, Sec sec, USec usec) {
    _impl->stop(s, sec, usec);
}

/*******************************************************************/
class PCImpl : public DampedImpl {
public:
    PCImpl(Pacer*);
    virtual ~PCImpl();

    virtual void eval(Sec, USec);
};

PCImpl::PCImpl (Pacer* p) : DampedImpl (p) {}

PCImpl::~PCImpl () {}

void PCImpl::eval (Sec sec, USec usec) {
    if (!_running) {
        return;
    }
    if (_fut_state == trial) {
        _cur_state = _fut_state;
        
        USec credit = 0;
        USec penalty = 0;

        /*  See if need to downgrade  */
        if (_fut_grade < _high) {
            for (long i = 0; i < _playlist->count(); i++) {
                CCell& cc = _playlist->item(i)->item_ref(_fut_grade);
                
                if (cc._tleft < 0) {
                    penalty = -cc._tleft;
                    for (long j = 0; j < _playlist->count(); j++) {
                        CCell& cell0 = _playlist->item(j)->item_ref(
                            _fut_grade
                        );
                        cell0._tleft = 0;
                        cell0._tdelta = 0;
                        
                        CCell& cell1 = _playlist->item(j)->item_ref(
                            _fut_grade+1
                        );
                        cell1._tleft = 0;
                        cell1._tdelta = 0;
                        cell1._cpf = 0;
                        
                    }
                    _fut_state = transit;
                    break;
                } else {
                    credit += cc._tleft;
                }
            }
        }
        if (_fut_state != transit) {
            //printf("succeeded\n");
            _fut_state = sustain;
            _cur_grade = _fut_grade;
            Cycle& cycle = _cyclelist->item_ref(_fut_grade);
            cycle._cur_cycle = 0;
            
            if (_fut_grade < _high) {
                Cycle& past_cycle = _cyclelist->item_ref(_fut_grade+1);
                if (past_cycle._cycle > MIN_CYCLE) {
                    past_cycle._cycle -= credit/USEC_TO_CYCLE;
                }
                past_cycle._cur_cycle = 0;
            }

            PaceMaker::instance()->cont_eval(SAMPLE_PERIOD);
        } else {
            //printf("failed\n");
            _cur_grade = _fut_grade;
            _fut_grade += 1;
            Cycle& cycle = _cyclelist->item_ref(_fut_grade);
            
            if (cycle._cycle < MAX_CYCLE) {
                cycle._cycle += penalty/USEC_TO_CYCLE;
            }
            cycle._cur_cycle = 0;

            PaceMaker::instance()->cont_eval(MaxTransit + SAMPLE_PERIOD);
        }
    } else {
        DampedImpl::eval(sec, usec);
    }
}

/*******************************************************************/
PCKeeper::PCKeeper (Pacer* root) {
    _impl = new PCImpl(root);
}

PCKeeper::~PCKeeper () {
    delete _impl;
}

void PCKeeper::adjust (Scheduler* s) {
    _impl->adjust(s);
}

void PCKeeper::eval (Sec sec, USec usec) {
    _impl->eval(sec, usec);
}

void PCKeeper::startall (Sec sec, USec usec) {
    _impl->startall(sec, usec);
}

void PCKeeper::stopall (Sec sec, USec usec) {
    _impl->stopall(sec, usec);
}

void PCKeeper::start (Scheduler* s, Sec sec, USec usec) {
    _impl->start(s, sec, usec);
}

void PCKeeper::play (Scheduler* s, Sec sec, USec usec) {
    _impl->play(s, sec, usec);
}

void PCKeeper::stop (Scheduler* s, Sec sec, USec usec) {
    _impl->stop(s, sec, usec);
}

/*******************************************************************/
class CoherenceImpl : public EventImpl {
public:
    CoherenceImpl(Pacer* r);
    virtual ~CoherenceImpl();

    void eval(Sec, USec);
};

CoherenceImpl::CoherenceImpl (Pacer* r) : EventImpl (r) {}

CoherenceImpl::~CoherenceImpl () {}

void CoherenceImpl::eval (Sec sec, USec usec) {
    if (!_running) {
        return;
    }
    switch(_fut_state) {
    case trial:
        {
            _cur_state = _fut_state;
            
            /*  See if need to downgrade  */
            if (_fut_grade < _high) {
                for (long i = 0; i < _playlist->count(); i++) {
                    CCell& cc = _playlist->item(i)->item_ref(_fut_grade);
                    
                    if (cc._tleft < 0) {
                        for (long j = 0; j < _playlist->count(); j++) {
                            CCell& cell0 = _playlist->item(j)->item_ref(
                                _fut_grade
                            );
                            cell0._tleft = 0;
                            cell0._tdelta = 0;
                            
                            CCell& cell1 = _playlist->item(j)->item_ref(
                                _fut_grade+1
                            );
                            cell1._tleft = 0;
                            cell1._tdelta = 0;
                            cell1._cpf = 0;
                            
                        }
                        _fut_state = transit;
                        
                        CCell& acell = _playlist->item(i)->item_ref(
                            _fut_grade+1
                        );
                        acell._coherence /= 2;
                        break;
                    } else {
                        CCell& acell = _playlist->item(i)->item_ref(
                            _fut_grade+1
                        );
                        acell._coherence *= 1.2;
                        if (acell._coherence > 1.0) {
                            acell._coherence = 1.0;
                        }
                    }
                }
            }
            if (_fut_state != transit) {
                _fut_state = sustain;
                _cur_grade = _fut_grade;
                //printf("succeeded \n"); 
           } else {
                _cur_grade = _fut_grade;
                _fut_grade += 1;
                //printf("failed \n");
            }
        }
        break;
    case transit:
        {
            EventImpl::eval(sec, usec);
            return;
        }
        break;
    case sustain:
        {
            EventImpl::eval(sec, usec);
            if (_fut_state == sustain) {
                /*  See if need to upgrade  */
                
                if (_cur_grade > _low) {
                    for (long i = 0; i < _playlist->count(); i++) {
                        USec period = _playerlist->item(i)->get_period();
                        CCell& b = _playlist->item(i)->item_ref(_cur_grade);
                        CCell& a = _playlist->item(i)->item_ref(_cur_grade-1);
                        
                        if (b._tdelta > 0) {
                            PaceMaker::instance()->cont_eval(SAMPLE_PERIOD);
                            return;
                        }
                        float delta = b._tdelta;
                        float cpf1 = b._cpf-delta;
                        float cpf0 = a._cpf-a._tdelta;
                        
                        USec est = (USec) (cpf0 + cpf0/cpf1*delta*b._coherence);
                        if (est > (period-TOL)) {
                            PaceMaker::instance()->cont_eval(SAMPLE_PERIOD);
                            return;
                        }
                    }
                    //printf("attempted by CohKeeper\n");
                    for (long j = 0; j < _playlist->count(); j++) {
                        CCell& cell0 = _playlist->item(j)->item_ref(
                            _cur_grade
                        );
                        cell0._tleft = 0;
                        cell0._tdelta = 0;
                        
                        CCell& cell1 = _playlist->item(j)->item_ref(
                            _cur_grade-1
                        );
                        cell1._tleft = 0;
                        cell1._tdelta = 0;
                    }
                    _fut_state = transit;
                    _fut_grade = _cur_grade-1;
                }
            } else {
                return;
            }
        }
        break;
    }
    if (_fut_state == transit) {
        PaceMaker::instance()->cont_eval(MaxTransit + SAMPLE_PERIOD);
    } else {
        PaceMaker::instance()->cont_eval(SAMPLE_PERIOD);
    }
}

/*******************************************************************/
CoherenceKeeper::CoherenceKeeper (Pacer* r) {
    _impl = new CoherenceImpl(r);
}

CoherenceKeeper::~CoherenceKeeper () {
    delete _impl;
}

void CoherenceKeeper::startall (Sec sec, USec usec) {
    _impl->startall(sec, usec);
}

void CoherenceKeeper::stopall (Sec sec, USec usec) {
    _impl->stopall(sec, usec);
}

void CoherenceKeeper::play (Scheduler* p, Sec sec, USec usec) {
    _impl->play(p, sec, usec);
}

void CoherenceKeeper::start (Scheduler* p, Sec sec, USec usec) {
    _impl->start(p, sec, usec);
}

void CoherenceKeeper::stop (Scheduler* p, Sec sec, USec usec) {
    _impl->stop(p, sec, usec);
}

void CoherenceKeeper::eval (Sec sec, USec usec) {
    _impl->eval(sec, usec);
}

void CoherenceKeeper::adjust (Scheduler* p) {
    _impl->adjust(p);
}

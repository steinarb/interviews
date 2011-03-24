#include <Dispatch/iohandler.h>
#include <Dispatch/dispatcher.h>
#include <OS/list.h>
#include <sys/time.h>
#include "pacekeeper.h"
#include "pacer.h"
#include "pacemaker.h"

class PaceHandlerList;

class PaceMakerImpl {
public:
    PaceMakerImpl(PaceKeeper*, USec period);
    virtual ~PaceMakerImpl();

    void set_pacekeeper(PaceKeeper*);
    PaceKeeper* get_pacekeeper();

    void tick(long sec, long usec);

    void startall();
    void pause();
    void resume();

    void play(Scheduler*, Sec, USec);

    void start(Scheduler*);
    void cont(Scheduler*, USec);
    void stop(Scheduler*);
    void stopall();

    void cont_eval(USec period);
    void get_origin(Sec& sec, USec& usec);

public:
    PaceHandlerList* _plist;
    IOHandler* _ehandler;
    PaceKeeper* _pacekeeper;
    boolean _running;
    boolean _paused;

    Sec _o_sec;
    USec _o_usec;

    Sec _p_sec;
    USec _p_usec;
    USec _period;
};

class PaceHandler : public IOHandler {
public:
    PaceHandler(Scheduler* = nil, PaceMakerImpl* = nil);
    virtual ~PaceHandler();

    virtual void timerExpired(long, long);
public:
    Scheduler* _pacer;
    PaceMakerImpl* _impl;
};

PaceHandler::PaceHandler(Scheduler* p, PaceMakerImpl* impl) {
    _pacer = p;
    _impl = impl;
}

PaceHandler::~PaceHandler () {}

void PaceHandler::timerExpired (long sec, long usec) {
    _impl->play(_pacer, sec, usec);
}

declarePtrList(PaceHandlerList, PaceHandler);
implementPtrList(PaceHandlerList, PaceHandler);

class EvalHandler : public IOHandler {
public:
    EvalHandler(PaceMakerImpl*);
    virtual void timerExpired(long, long);
public:
    PaceMakerImpl* _impl;
};

EvalHandler::EvalHandler (PaceMakerImpl* impl) {
    _impl = impl;
}

void EvalHandler::timerExpired(long sec, long usec) {
    _impl->tick(sec, usec);
}

PaceMakerImpl::PaceMakerImpl (PaceKeeper* pk, USec period) {
    _running = false;
    _paused = false;
    _period = period;
    _pacekeeper = pk;

    _o_sec = 0;
    _o_usec = 0;
    _p_sec = 0;
    _p_usec = 0;

    _plist = new PaceHandlerList(5);
    _ehandler = new EvalHandler(this);
}

PaceMakerImpl::~PaceMakerImpl () {
    Dispatcher& d = Dispatcher::instance();
    for (long i = 0; i < _plist->count(); i++) {
        d.stopTimer(_plist->item(i));
        delete _plist->item(i);
    }
    d.stopTimer(_ehandler);
    delete _ehandler;
    delete _plist;
}

void PaceMakerImpl::set_pacekeeper(PaceKeeper* pacekeeper) {
    _pacekeeper = pacekeeper;
}

PaceKeeper* PaceMakerImpl::get_pacekeeper () {
    return _pacekeeper; 
}

void PaceMakerImpl::tick (long sec, long usec) {
    _pacekeeper->eval(sec-_o_sec, usec-_o_sec);
}

void PaceMakerImpl::cont_eval (USec period) {
    if (period > 0) {
        _period = period;
    }
    Dispatcher& d = Dispatcher::instance();
    d.stopTimer(_ehandler);
    d.startTimer(0, _period, _ehandler);
    
}

void PaceMakerImpl::get_origin(Sec& sec, USec& usec) {
    sec = _o_sec;
    usec = _o_usec;
}

void PaceMakerImpl::startall () {
    _running = true;

    timeval curTime;
    gettimeofday(&curTime, nil);
    
    _o_sec = curTime.tv_sec;
    _o_usec = curTime.tv_usec;
    
    _pacekeeper->startall(0, 0);

    Dispatcher& d = Dispatcher::instance();
    d.startTimer(0, _period, _ehandler);
}

void PaceMakerImpl::pause () {
    if (_running && !_paused) {
        _paused = true;

        timeval curTime;
        
        gettimeofday(&curTime, nil);
        _p_sec = curTime.tv_sec;
        _p_usec = curTime.tv_usec;

        Dispatcher& d = Dispatcher::instance();
        for (long i = 0; i < _plist->count(); i++) {
            d.stopTimer(_plist->item(i));
        }
        d.stopTimer(_ehandler);
    }
}

void PaceMakerImpl::resume () {
    if (_running && _paused) {
        timeval curTime;
        gettimeofday(&curTime, nil);
        Sec sec = _p_sec - _o_sec;
        Sec usec = _p_usec - _o_usec;

        for (long i = 0; i < _plist->count(); i++) {
            _pacekeeper->play(_plist->item(i)->_pacer, sec, usec);
        }
        
        _o_sec += curTime.tv_sec - _p_sec;
        _o_usec += curTime.tv_usec - _p_usec;
        Dispatcher& d = Dispatcher::instance();
        d.startTimer(0, _period, _ehandler);
    }
}

void PaceMakerImpl::play (Scheduler* p, Sec sec, USec usec) {
    if (_running && !_paused) {
        for (long i = 0; i < _plist->count(); i++) {
            if (_plist->item(i)->_pacer == p) {
                Dispatcher& d = Dispatcher::instance();
                d.stopTimer(_plist->item(i));
                delete _plist->item(i);
                _plist->remove(i);
                break;
            }
        }
        _pacekeeper->play(p, sec-_o_sec, usec-_o_usec);
    }
}

void PaceMakerImpl::cont (Scheduler* p, USec usec) {
    if (_running) {
        if (usec > 0) {
            PaceHandler* phandler = new PaceHandler(p, this);
            _plist->append(phandler);
            Dispatcher::instance().startTimer(0, usec, phandler);
        } else {
            timeval curTime;
            gettimeofday(&curTime, nil);
            
            play(p, curTime.tv_sec, curTime.tv_usec);
        }
    }
}

void PaceMakerImpl::start (Scheduler* p) {
    timeval curTime;

    gettimeofday(&curTime, nil);
    if (!_running) {
        _running = true;
        _o_sec = curTime.tv_sec;
        _o_usec = curTime.tv_usec;

        _pacekeeper->start(p, 0, 0);
    } else {
        _pacekeeper->start(p, curTime.tv_sec-_o_sec, curTime.tv_usec-_o_usec);
    }

}

void PaceMakerImpl::stop (Scheduler* p) {
    for (long i = 0; i < _plist->count(); i++) {
        if (_plist->item(i)->_pacer == p) {
            Dispatcher::instance().stopTimer(_plist->item(i));
            delete _plist->item(i);
            _plist->remove(i);
            break;
        }
    }
    timeval curTime;

    gettimeofday(&curTime, nil);
    _pacekeeper->stop(p, curTime.tv_sec-_o_sec, curTime.tv_usec-_o_usec);
}

void PaceMakerImpl::stopall () {
    Dispatcher& d = Dispatcher::instance();
    if (_running) {
        _running = false;
        for (long i = 0; i < _plist->count(); i++) {
            d.stopTimer(_plist->item(i));
            delete _plist->item(i);
        }
    }
    _plist->remove_all();
    d.stopTimer(_ehandler);
    if (_pacekeeper != nil) {
        timeval curTime;
        
        gettimeofday(&curTime, nil);
        _pacekeeper->stopall(curTime.tv_sec-_o_sec, curTime.tv_usec-_o_usec);
    }
}

/*************************************************************************/
static PaceMaker* _maker = nil;

PaceMaker::PaceMaker (PaceKeeper* pk, USec period) {
    _impl = new PaceMakerImpl(pk, period);
    _maker = this;
}

PaceMaker::~PaceMaker () {
    delete _impl;
    if (_maker == this) {
        _maker = nil;
    }
}

void PaceMaker::startall () {
    _impl->startall();
}

void PaceMaker::pause () {
    _impl->pause();
}

void PaceMaker::resume () {
    _impl->resume();
}

void PaceMaker::cont (Scheduler* p, USec usec) {
    _impl->cont(p, usec);
}

void PaceMaker::start (Scheduler* p) {
    _impl->start(p);
}

void PaceMaker::stop (Scheduler* p) {
    _impl->stop(p);
}

void PaceMaker::stopall () {
    _impl->stopall();
}

PaceMaker* PaceMaker::instance () {
    if (_maker == nil) {
        _maker = new PaceMaker;
    }
    return _maker;
}

void PaceMaker::set_pacekeeper(PaceKeeper* pacekeeper) {
    _impl->set_pacekeeper(pacekeeper);
}

PaceKeeper* PaceMaker::get_pacekeeper () { return _impl->get_pacekeeper(); }

void PaceMaker::cont_eval(USec period) {
    _impl->cont_eval(period);
}

void PaceMaker::get_origin (Sec& sec, USec& usec) {
    Sec s;
    USec u;
    _impl->get_origin(s, u);
    sec = s;
    usec = u;
}

#include "gcollector.h"
#include "pacer.h"
#include <stdio.h>

class TileImpl {
public:
    TileImpl();

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();
public:
    Pacer* _pacer;
    Grade _low, _high;
    SessionID _id;

    Pacer* _target;
    Grade _target_x;
};

TileImpl::TileImpl () {
    _pacer = nil;
    _low = _high = -1;
    _id = -1;
    _target = nil;
}

void TileImpl::play (RunTime& rtime) {
    Grade low, high;
    get_grades(low, high);
    FadeType f = fadetype(
        rtime._cur_grade, rtime._fut_grade, low, high
    );
    
    if (f == no_fade) {
        return;
    }

    if (rtime._id != _id) {
        /* new session */

        _id = rtime._id;
        _target = nil;
        _target_x = -1;
    }

    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    Grade orig_cur = cur;
    Grade orig_fut = fut;

    if (_target != nil) {
        cur -= _target_x;
        fut -= _target_x;
        _target->get_grades(low, high);
        FadeType f = fadetype(cur, fut, low, high);
        switch(f) {
        case no_fade:
            {
                _target = nil;
                _target_x = -1;
            }
            break;
        case sustain:
            {
                _target->play(rtime);
                cur = orig_cur;
                fut = orig_fut;
                return;
            }
            break;
        case fadeout_low:
        case fadeout_high:
            {
                _target->play(rtime);
            }
            break;
        case fadein_low:
        case fadein_high:
            break;
        }
        cur = orig_cur;
        fut = orig_fut;
    }

    if (_pacer->count() > 0) {
        Grade l, h;
        _pacer->child(0)->get_grades(l, h);
        cur -= l;
        fut -= l;
    }

    for (PacerIndex i = 0; i < _pacer->count(); i++) {
        Grade l, h;
        Pacer* p = _pacer->child(i);
        p->get_grades(l, h);
        Grade diff = h - l;

        if (cur >= 0 && cur <= diff) {
            /* fade out, target already played */
            if (_target == nil) {
                p->play(rtime); 
            }
            if (cur == fut) {
                //printf("target set to %d out of %d\n", i, _pacer->count()-1);
                //printf("cur and fut are %d %d\n", orig_cur, orig_fut);
                _target = p;
                _target_x = orig_cur - cur;
                //printf("target_x is %d\n", _target_x);
            }

        } else if (fut >= 0 && fut <= diff) {
            p->play(rtime);
            _target = p;
            _target_x = orig_fut - fut;
        }

        cur -= (diff + 1);
        fut -= (diff + 1);

        if (cur < 0 && fut < 0) {
            break;
        }
    }

    cur = orig_cur;
    fut = orig_fut;
}

void TileImpl::get_grades (Grade& low, Grade& high) {
    if (_low == -1 && _high == -1) {
        for (PacerIndex i = 0; i < _pacer->count(); i++) {
            Grade l, h;
            _pacer->child(i)->get_grades(l, h);
            _high += h - l + 1;
        }
        
        if (_pacer->count() > 0) {
            Grade l, h;
            
            _pacer->child(0)->get_grades(l, h);
            _low = l;
            _high += l;
        }
    }
    low = _low;
    high = _high;
}

void TileImpl::regrade () {
    _low = -1;
    _high = -1;

    for (PacerIndex i = 0; i < _pacer->count(); i++) {
        Grade l, h;
        _pacer->child(i)->regrade();
        _pacer->child(i)->get_grades(l, h);
        _high += h - l + 1;
    }

    if (_pacer->count() > 0) {
        Grade l, h;

        _pacer->child(0)->get_grades(l, h);
        _low = l;
        _high += l;
    }

}

/***************************************************************************/
class NormalImpl {
public:
    NormalImpl();

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();
public:
    Pacer* _pacer;
    Grade _low, _high;
    SessionID _id;
};

NormalImpl::NormalImpl () {
    _pacer = nil;
    _low = _high = -1;
    _id = -1;
}

void NormalImpl::play (RunTime& rtime) {
    Grade low, high;
    get_grades(low, high);
    FadeType f = fadetype(
        rtime._cur_grade, rtime._fut_grade, low, high
    );
    
    if (f == no_fade) {
        return;
    }
    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    Grade orig_cur = cur;
    Grade orig_fut = fut;

    Grade diff = _high - _low;
    float cur_f, fut_f;

    if (diff == 0) {
        if (cur == 0 || fut == 0) {
            cur_f = cur;
            fut_f = fut;
        } else {
            return;
        }
    } else {
       cur_f = ((float)cur)/((float)diff);
       fut_f = ((float)fut)/((float)diff);
    }

    for (PacerIndex i = 0; i < _pacer->count(); i++) {
        Grade l, h;
        Pacer* p = _pacer->child(i);
        p->get_grades(l, h);
        
        cur = (Grade)(cur_f * (h - l) + l);
        fut = (Grade)(fut_f * (h - l) + l);
        p->play(rtime);
    }
    
    cur = orig_cur;
    fut = orig_fut;
}

void NormalImpl::get_grades (Grade& low, Grade& high) {
    if (_low == -1 || _high == -1) {
        for (PacerIndex i = 0; i < _pacer->count(); i++) {
            Grade l, h;
            _pacer->child(i)->get_grades(l, h);
            if (_high < (h - l)) {
                _high = h - l;
            }
        }
    }
    low = _low;
    high = _high;
}

void NormalImpl::regrade () {
    _low = 0;
    _high = -1;
    for (PacerIndex i = 0; i < _pacer->count(); i++) {
        Grade l, h;
        _pacer->child(i)->regrade();
        _pacer->child(i)->get_grades(l, h);
        if (_high < (h - l)) {
            _high = h - l;
        }
    }
}

/***************************************************************************/
class DistribImpl {
public:
    DistribImpl();

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();
public:
    Pacer* _pacer;
    Grade _low, _high;
    SessionID _id;
};

DistribImpl::DistribImpl () {
    _pacer = nil;
    _low = _high = -1;
    _id = -1;
}

void DistribImpl::regrade () {
    _low = -1;
    _high = -1;
    if (_pacer->count() > 0) {
        _pacer->child(0)->regrade();
        _pacer->child(0)->get_grades(_low, _high);
    }
        
    for (PacerIndex i = 1; i < _pacer->count(); i++) {
        Grade l, h;
        _pacer->child(i)->regrade();
        _pacer->child(i)->get_grades(l, h);
        if (h > _high) {
            _high = h;
        }
        if (l < _low) {
            _low = l;
        }
    }
}

void DistribImpl::get_grades (Grade& low, Grade& high) {
    if (_low == -1 || _high == -1) {
        if (_pacer->count() > 0) {
            _pacer->child(0)->get_grades(_low, _high);
        }
        
        for (PacerIndex i = 1; i < _pacer->count(); i++) {
            Grade l, h;
            _pacer->child(i)->get_grades(l, h);
            if (h > _high) {
                _high = h;
            }
            if (l < _low) {
                _low = l;
            }
        }
    }
    low = _low;
    high = _high;
}

void DistribImpl::play (RunTime& rtime) {
    Grade low, high;
    get_grades(low, high);
    FadeType f = fadetype(
        rtime._cur_grade, rtime._fut_grade, low, high
    );
    
    if (f == no_fade) {
        return;
    }
    Grade& cur = rtime._cur_grade;
    Grade& fut = rtime._fut_grade;

    for (PacerIndex i = 0; i < _pacer->count(); i++) {
        Grade l, h;
        Pacer* p = _pacer->child(i);
        p->get_grades(l, h);
        
        if (cur >= l && cur <= h) {
            p->play(rtime);

        } else if (fut >= l && fut <= h) {
            p->play(rtime);
        }
    }
}

/***************************************************************************/
GradeCollector::GradeCollector () {}

GradeCollector::~GradeCollector () {}

void GradeCollector::set_pacer (Pacer*) {}

/***************************************************************************/
Tiler::Tiler () {
    _impl = new TileImpl;
}

Tiler::~Tiler () { delete _impl; }

void Tiler::play (RunTime& rtime) {
    _impl->play(rtime);
}

void Tiler::set_pacer (Pacer* p) {
    _impl->_pacer = p;
}

void Tiler::get_grades (Grade& low, Grade& high) {
    Grade l, h;
    _impl->get_grades(l, h);
    low = l; high = h;
}

void Tiler::regrade () {
    _impl->regrade();
}

Normalizer::Normalizer () {
    _impl = new NormalImpl;
}

Normalizer::~Normalizer () { delete _impl; }

void Normalizer::play (RunTime& rtime) {
    _impl->play(rtime);
}

void Normalizer::set_pacer (Pacer* p) {
    _impl->_pacer = p;
}

void Normalizer::get_grades (Grade& low, Grade& high) {
    Grade l, h;
    _impl->get_grades(l, h);
    low = l; high = h;
}

void Normalizer::regrade () {
    _impl->regrade();
}

Distributor::Distributor () {
    _impl = new DistribImpl;
}

Distributor::~Distributor () { delete _impl; }

void Distributor::play (RunTime& rtime) {
    _impl->play(rtime);
}

void Distributor::set_pacer (Pacer* p) {
    _impl->_pacer = p;
}

void Distributor::get_grades (Grade& low, Grade& high) {
    Grade l, h;
    _impl->get_grades(l, h);
    low = l; high = h;
}

void Distributor::regrade () {
    _impl->regrade();
}

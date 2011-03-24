#ifndef pacemaker_h
#define pacemaker_h

#include "globals.h"

class PaceMakerImpl;
class PaceKeeper;
class Pacer;

const USec SAMPLE_PERIOD = 500000;

class PaceMaker {
public:
    PaceMaker(PaceKeeper* = nil, USec period = SAMPLE_PERIOD);
    virtual ~PaceMaker();
    
    virtual void startall();
    virtual void pause();
    virtual void resume();

    virtual void start(Scheduler*);
    virtual void cont(Scheduler*, USec);
    virtual void stop(Scheduler*);
    virtual void stopall();

    virtual void cont_eval(USec period = -1);
    virtual void get_origin(Sec&, USec&);

    static PaceMaker* instance();

    virtual void set_pacekeeper(PaceKeeper*);
    virtual PaceKeeper* get_pacekeeper();
protected:
    PaceMakerImpl* _impl;
};

#endif

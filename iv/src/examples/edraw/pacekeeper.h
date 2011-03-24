#ifndef pacekeeper_h
#define pacekeeper_h

#include "globals.h"

class Scheduler;
class ContinueImpl;
class CoherenceImpl;
class DampedImpl;
class EventImpl;
class PCImpl;

class PaceKeeper {
public:
    enum {
        NoPK = 0, EDriven, DampedOsc, PC, Coh
    };

    virtual ~PaceKeeper();

    virtual void startall(Sec, USec) = 0;
    virtual void stopall(Sec, USec) = 0;

    virtual void start(Scheduler*, Sec, USec) = 0;
    virtual void play(Scheduler*, Sec, USec) = 0;
    virtual void stop(Scheduler*, Sec, USec) = 0;

    virtual void eval(Sec, USec);
    virtual void adjust(Scheduler*);
protected:
    PaceKeeper();
};

class Continuator : public PaceKeeper {
public:
    Continuator(Pacer* root);
    virtual ~Continuator();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);

protected:
    ContinueImpl* _impl;
};

class EventKeeper : public PaceKeeper {
public:
    EventKeeper(Pacer* root);
    virtual ~EventKeeper();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);

    virtual void adjust(Scheduler*);
    virtual void eval(Sec, USec);
protected:
    EventImpl* _impl;
};

class DampedKeeper : public PaceKeeper {
public:
    DampedKeeper(Pacer* root);
    virtual ~DampedKeeper();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);

    virtual void adjust(Scheduler*);
    virtual void eval(Sec, USec);
protected:
    DampedImpl* _impl;
};

class PCKeeper : public PaceKeeper {
public:
    PCKeeper(Pacer* root);
    virtual ~PCKeeper();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);

    virtual void adjust(Scheduler*);
    virtual void eval(Sec, USec);
protected:
    PCImpl* _impl;
};

class CoherenceKeeper : public PaceKeeper {
public:
    CoherenceKeeper(Pacer* root);
    virtual ~CoherenceKeeper();

    virtual void startall(Sec, USec);
    virtual void stopall(Sec, USec);

    virtual void start(Scheduler*, Sec, USec);
    virtual void play(Scheduler*, Sec, USec);
    virtual void stop(Scheduler*, Sec, USec);
    virtual void eval(Sec, USec);
    virtual void adjust(Scheduler*);
private:
    CoherenceImpl* _impl;
};

#endif

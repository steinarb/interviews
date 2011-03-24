#ifndef gcollector_h
#define gcollector_h

#include <OS/list.h>
#include "globals.h"

class Pacer;
class TileImpl;
class NormalImpl;
class DistribImpl;

class GradeCollector {
public:
    virtual ~GradeCollector();

    virtual void play(RunTime&) = 0;
    virtual void get_grades(Grade& low, Grade& high) = 0;
    virtual void regrade() = 0;

    virtual void set_pacer(Pacer*);
protected:
    GradeCollector();
};

class Tiler : public GradeCollector {
public:
    Tiler();
    virtual ~Tiler();

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();

    virtual void set_pacer(Pacer*);
private:
    TileImpl* _impl;
};

class Normalizer : public GradeCollector {
public:
    Normalizer();
    virtual ~Normalizer();

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();

    virtual void set_pacer(Pacer*);
private:
    NormalImpl* _impl;
};

class Distributor : public GradeCollector {
public:
    Distributor();
    virtual ~Distributor();

    virtual void play(RunTime&);
    virtual void get_grades(Grade& low, Grade& high);
    virtual void regrade();

    virtual void set_pacer(Pacer*);
private:
    DistribImpl* _impl;
};

#endif




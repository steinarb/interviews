/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Adjuster - button-like interactors for incremental adjustment of an
 * interactor's perspective.
 */

#ifndef adjuster_h
#define adjuster_h

#include <InterViews/interactor.h>

static const int NO_AUTOREPEAT = -1;

class Bitmap;
class Shape;

class Adjuster : public Interactor {
public:
    Adjuster(Interactor*, int = NO_AUTOREPEAT);
    Adjuster(const char*, Interactor*, int = NO_AUTOREPEAT);
    Adjuster(Interactor*, int, Painter*);
    ~Adjuster();

    void Handle(Event&);
    virtual void Highlight(boolean on);
    void Redraw(Coord, Coord, Coord, Coord);
    void Reshape(Shape&);
protected:
    Interactor* view;
    Bitmap* plain;
    Bitmap* hit;
    Bitmap* mask;
    int delay;
    Perspective* shown;
    boolean highlighted;

    virtual void AdjustView(Event&);
    void AutoRepeat();
    void Flash();
    void HandlePress();
    void Invert();
    virtual void Reconfig();
    void TimerOn();
    void TimerOff();
private:
    void Init(Interactor*, int);
};

class Zoomer : public Adjuster {
public:
    Zoomer(Interactor*, float factor);
    Zoomer(const char*, Interactor*, float factor);
    Zoomer(Interactor*, float factor, Painter*);
protected:
    void AdjustView(Event&);
private:
    float factor;

    void Init(float);
};

class Enlarger : public Zoomer {
public:
    Enlarger(Interactor*);
    Enlarger(const char*, Interactor*);
    Enlarger(Interactor*, Painter*);
private:
    void Init();
};

class Reducer : public Zoomer {
public:
    Reducer(Interactor*);
    Reducer(const char*, Interactor*);
    Reducer(Interactor*, Painter*);
private:
    void Init();
};

class Mover : public Adjuster {
public:
    Mover(Interactor*, int delay, int moveType);
    Mover(const char*, Interactor*, int delay, int moveType);
    Mover(Interactor*, int delay, int moveType, Painter*);
protected:
    int moveType;
    void AdjustView(Event&);
private:
    void Init(int);
};

class LeftMover : public Mover {
public:
    LeftMover(Interactor*, int delay = NO_AUTOREPEAT);
    LeftMover(const char*, Interactor*, int delay = NO_AUTOREPEAT);
    LeftMover(Interactor*, int delay, Painter*);
private:
    void Init();
};

class RightMover : public Mover {
public:
    RightMover(Interactor*, int delay = NO_AUTOREPEAT);
    RightMover(const char*, Interactor*, int delay = NO_AUTOREPEAT);
    RightMover(Interactor*, int delay, Painter*);
private:
    void Init();
};

class UpMover : public Mover {
public:
    UpMover(Interactor*, int delay = NO_AUTOREPEAT);
    UpMover(const char*, Interactor*, int delay = NO_AUTOREPEAT);
    UpMover(Interactor*, int delay, Painter*);
private:
    void Init();
};

class DownMover : public Mover {
public:
    DownMover(Interactor*, int delay = NO_AUTOREPEAT);
    DownMover(const char*, Interactor*, int delay = NO_AUTOREPEAT);
    DownMover(Interactor*, int delay, Painter*);
private:
    void Init();
};

#endif

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
 * Panner - an interactor for two-dimensional scrolling and zooming.
 */

#ifndef panner_h
#define panner_h

#include <InterViews/scene.h>

class Panner : public MonoScene {
public:
    Panner(Interactor*, int size = 0);
    Panner(const char*, Interactor*, int size = 0);
    Panner(Interactor*, int size, Painter*);
protected:
    int size;

    virtual void Reconfig();
private:
    Interactor* adjusters;
    Interactor* slider;

    void Init(Interactor*, int);
};

class Slider : public Interactor {
public:
    Slider(Interactor*);
    Slider(const char*, Interactor*);
    Slider(Interactor*, Painter*);
    ~Slider();

    virtual void Draw();
    virtual void Handle(Event&);
    virtual void Update();
    virtual void Reshape(Shape&);
    virtual void Resize();
protected:
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
private:
    Interactor* interactor;
    Perspective* view;
    Perspective* shown;
    Coord left, bottom, right, top;
    Coord prevl, prevb, prevr, prevt;	// for smart update
    Coord llim, blim, rlim, tlim;	// sliding limits
    boolean constrained, syncScroll;
    int moveType;
    Coord origx, origy;

    void Init(Interactor*);
    Coord ViewX(Coord);
    Coord ViewY(Coord);
    Coord SliderX(Coord);
    Coord SliderY(Coord);
    void CalcLimits(Event&);		// calculate sliding limits
    void SizeKnob();			// calculate size of slider knob
    boolean Inside(Event&);		// true if inside slider knob
    void Constrain(Event&);		// constrain slider knob motion
    void Move(Coord dx, Coord dy);	// move view to reflect slider position
    void Slide(Event&);			// rubberband rect while mousing
    void Jump(Event&);			// for click outside knob
};

#endif

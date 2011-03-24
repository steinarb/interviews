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
 * General scrolling interface.
 */

#ifndef scroller_h
#define scroller_h

#include <InterViews/interactor.h>

class Scroller : public Interactor {
protected:
    Interactor* interactor;
    int size;
    Perspective* view;
    Perspective* shown;
    double scale;
    Sensor* tracking;
    boolean syncScroll;

    Scroller(Interactor*, int);
    Scroller(const char*, Interactor*, int);
    Scroller(Interactor*, int, Painter* out);
    ~Scroller();

    void Background(Coord, Coord, Coord, Coord);
    void MakeBackground();
    virtual void Resize();
private:
    void Init();
};

class HScroller : public Scroller {
public:
    HScroller(Interactor*, int size = 0);
    HScroller(const char*, Interactor*, int size = 0);
    HScroller(Interactor*, int size, Sensor* in, Painter* out);

    virtual void Handle(Event&);
    virtual void Update();
private:
    void Bar(Coord, int);
    void Border(Coord);
    void GetBarInfo(Perspective*, Coord&, int&);
    void Init();
    void Outline(Coord, int);
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
    void Sides(Coord, Coord);
    Coord Slide(Event&);
};

class VScroller : public Scroller {
public:
    VScroller(Interactor*, int size = 0);
    VScroller(const char*, Interactor*, int size = 0);
    VScroller(Interactor*, int size, Sensor* in, Painter* out);

    virtual void Handle(Event&);
    virtual void Update();
private:
    void Bar(Coord, int);
    void Border(Coord);
    void GetBarInfo(Perspective*, Coord&, int&);
    void Init();
    void Outline(Coord, int);
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
    void Sides(Coord, Coord);
    Coord Slide(Event&);
};

#endif

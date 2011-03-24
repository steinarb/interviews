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
 * A viewport contains another interactor.  Unlike a MonoScene or Frame,
 * a viewport always gives the interactor its desired shape.  However,
 * the interactor may not be entirely viewable through the viewport.
 * The viewport's perspective can be used to adjust what portion is visible.
 */

#ifndef viewport_h
#define viewport_h

#include <InterViews/scene.h>

class Viewport : public MonoScene {
public:
    Viewport(Interactor* = nil, Alignment = Center);
    Viewport(const char*, Interactor* = nil, Alignment = Center);
    Viewport(Sensor*, Painter*, Interactor* = nil, Alignment = Center);
    ~Viewport();

    virtual void Adjust(Perspective&);
    virtual void Resize();

    void AdjustTo(float px, float py, float zx, float zy);
    void AdjustBy(float dpx, float dpy, float dzx, float dzy);

    void ScrollTo(float px, float py);
    void ScrollXTo(float px);
    void ScrollYTo(float py);
    void ScrollBy(float dpx, float dpy);
    void ScrollXBy(float dpx);
    void ScrollYBy(float dpy);

    void ZoomTo(float zx, float zy);
    void ZoomXTo(float zx);
    void ZoomYTo(float zy);
    void ZoomBy(float dzx, float dzy);
    void ZoomXBy(float dzx);
    void ZoomYBy(float dzy);

    float XPos();
    float YPos();
    float XMag();
    float YMag();
protected:
    Alignment align;
    Painter* background;

    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
    virtual void DoMove(Interactor*, Coord& x, Coord& y);
private:
    int cwidth;
    int cheight;

    void Init(Interactor*, Alignment);
    void DoAdjust(float px, float py, float zx, float zy);
};

#endif

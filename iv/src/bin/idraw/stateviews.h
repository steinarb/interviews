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

// $Header: stateviews.h,v 1.9 89/10/09 14:49:56 linton Exp $
// declares classes StateView and StateView's subclasses.

#ifndef stateviews_h
#define stateviews_h

#include <InterViews/interactor.h>

// Declare imported types.

class Graphic;
class State;

// A StateView attaches itself to a State and displays some of the
// State's information.

class StateView : public Interactor {
public:

    StateView(State*, const char*);
    ~StateView();

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();

    State* state;		// stores subject whose attribute view displays
    char* label;		// stores view's text label

    Coord label_x, label_y;	// stores position at which to display label

};

// A BrushView displays the current brush.

class BrushView : public StateView {
public:

    BrushView(State*);
    ~BrushView();

    void Update();

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();

    Graphic* brushindic;	// displays line to demonstrate brush's effect

};

// A DrawingNameView displays the drawing's name.

class DrawingNameView : public StateView {
public:

    DrawingNameView(State*);

    void Update();

protected:

    void Reconfig();
    void Resize();
    const char* GetDrawingName(State*);

};

// A FontView displays the current font.

class FontView : public StateView {
public:

    FontView(State*);
    ~FontView();

    void Update();

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    const char* GetPrintFontAndSize(State*);

    Painter* background;		// draws background behind label

};

// A GriddingView displays the current status of the grid's gridding.

class GriddingView : public StateView {
public:

    GriddingView(State*);

    void Update();

protected:

    const char* GetGridding(State*);

};

// A MagnifView displays the current magnification.

class MagnifView : public StateView {
public:

    MagnifView(State*, Interactor*);

    void Update();

protected:

    void Resize();
    const char* GetMagnif(State*);

};

// A ModifStatusView displays the current modification status.

class ModifStatusView : public StateView {
public:

    ModifStatusView(State*);

    void Update();

protected:

    const char* GetModifStatus(State*);

};

// A PatternView displays the current pattern.

class PatternView : public StateView {
public:

    PatternView(State*);
    ~PatternView();

    void Update();

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);

    Painter* patindic;		// fills rect to demonstrate pat's effect

};

#endif

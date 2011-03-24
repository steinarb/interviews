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

// $Header: drawingview.h,v 1.11 89/10/09 14:47:53 linton Exp $
// declares class DrawingView.

#ifndef drawingview_h
#define drawingview_h

#include <InterViews/Graphic/grblock.h>

// Declare imported types.

class Damage;
class Page;
class Rubberband;
class Selection;
class SelectionList;
class State;
class TextEdit;

// A DrawingView displays the user's drawing.

class DrawingView : public GraphicBlock {
public:

    DrawingView(Page*);
    ~DrawingView();

    void SetSelectionList(SelectionList*);
    void SetState(State*);
    void SetTools(Interactor*);

    void Draw();
    void Handle(Event&);

    void Manipulate(
	Event&, Rubberband*, int, boolean constrain=true, boolean erase=true
    );
    void Edit(Event&, TextEdit*, Graphic* = nil);

    void DrawHandles();
    void RedrawHandles();
    void EraseHandles();
    void EraseExcessHandles(SelectionList*);
    void ErasePickedHandles(Selection*);
    void ErasePickedHandles(SelectionList*);
    void EraseUngraspedHandles(Selection*);
    void ResetAllHandles();

    void Added();
    void Damaged();
    void Repair();

    void Magnify(Coord, Coord, Coord, Coord);
    void Reduce();
    void Enlarge();
    void NormalSize();
    void ReduceToFit();
    void CenterPage();

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();
    float LimitMagnification(float);
    void StartTextEditing(Event&, TextEdit*, Graphic*);
    void EndTextEditing();
    void RedrawTextEditor();

    Damage* damage;		// keeps track of damaged areas of drawing
    Graphic* gs;		// remembers text being edited
    Page* page;			// snaps points to grid
    Painter* rasterxor;		// stores painter with which to draw handles
    SelectionList* sl;		// lists current Selections
    State* state;		// stores Graphic and nonGraphic attributes
    TextEdit* textedit;		// redraws in-place texteditor if necessary
    Interactor* tools;		// handles events

};

#endif

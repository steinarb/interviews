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

// $Header: drawingview.c,v 1.16 89/10/09 14:47:51 linton Exp $
// implements class DrawingView.

#include "drawingview.h"
#include "istring.h"
#include "listselectn.h"
#include "page.h"
#include "slpict.h"
#include "state.h"
#include "textedit.h"
#include <InterViews/Graphic/damage.h>
#include <InterViews/event.h>
#include <InterViews/painter.h>
#include <InterViews/perspective.h>
#include <InterViews/rubband.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>
#include <math.h>

// DrawingView caches its canvas' contents if possible to speed up
// redrawing after expose events.

static const int PAD = 0;	// we don't want any padding around graphic

DrawingView::DrawingView (Page* p) : (updownEvents, p, PAD, Center, Binary) {
    damage = nil;
    gs = nil;
    page = p;
    rasterxor = nil;
    sl = nil;
    state = nil;
    textedit = nil;
    tools = nil;
    SetCanvasType(CanvasSaveContents);
}

// Free storage allocated to store members.

DrawingView::~DrawingView () {
    delete damage;
    Unref(rasterxor);
}

// Define access functions to set members' values.  Only Idraw sets
// their values.

void DrawingView::SetSelectionList (SelectionList* slist) {
    sl = slist;
}

void DrawingView::SetState (State* s) {
    state = s;
}

void DrawingView::SetTools (Interactor* t) {
    tools = t;
}

// Draw draws the entire view.  Draw calls Check for its side effect
// of flushing any redraws caused by a dialog box's removal before
// drawing the view.

void DrawingView::Draw () {
    if (graphic != nil) {
	Graphic* backup = graphic;
	graphic = nil;
	Check();
	graphic = backup;

	GraphicBlock::Draw();
	damage->Reset();
	ResetAllHandles();
	RedrawHandles();
	RedrawTextEditor();
    }
}

// Handle delegates input events to the tools.

void DrawingView::Handle (Event& e) {
    tools->Handle(e);
}

// Manipulate lets the user manipulate the Rubberband with the mouse
// until a specified event occurs.

void DrawingView::Manipulate (Event& e, Rubberband* rubberband, int et,
boolean constrain, boolean erase) {
    rubberband->SetPainter(rasterxor);
    rubberband->SetCanvas(canvas);
    Listen(allEvents);
    while (e.eventType != et) {
	if (e.eventType == MotionEvent) {
	    rubberband->Track(e.x, e.y);
	}
	Read(e);
	if (constrain) {
	    page->Constrain(e.x, e.y);
	}
    }
    Listen(input);
    if (erase) {
	rubberband->Erase();
    }
}

// Edit lets the user enter text into the drawing.

void DrawingView::Edit (Event& e, TextEdit* textedit, Graphic* gs) {
    StartTextEditing(e, textedit, gs);

    textedit->Grasp(e);
    Listen(allEvents);
    while (textedit->Editing(e)) {
	Read(e);
    }
    UnRead(e);
    Listen(input);

    EndTextEditing();
}

// DrawHandles tells all the Selections to draw their handles unless
// they've already been drawn.

void DrawingView::DrawHandles () {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->DrawHandles(rasterxor, canvas);
    }
}

// RedrawHandles tells all the Selections to redraw their handles
// whether or not they've already been drawn.

void DrawingView::RedrawHandles () {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->RedrawHandles(rasterxor, canvas);
    }
}

// EraseHandles tells all the Selections to erase their handles unless
// they've already been erased.

void DrawingView::EraseHandles () {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->EraseHandles(rasterxor, canvas);
    }
}

// EraseExcessHandles erases the excess Selections' handles if it
// doesn't find the Selections in the current SelectionList.

void DrawingView::EraseExcessHandles (SelectionList* newsl) {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* oldselection = sl->GetCur()->GetSelection();
	if (!newsl->Find(oldselection)) {
	    oldselection->EraseHandles(rasterxor, canvas);
	}
    }
}

// ErasePickedHandles erases the picked Selection's handles if it
// finds the picked Selection in the SelectionList.

void DrawingView::ErasePickedHandles (Selection* pick) {
    if (sl->Find(pick)) {
	sl->GetCur()->GetSelection()->EraseHandles(rasterxor, canvas);
    }
}

// ErasePickedHandles erases the picked Selections' handles if it
// finds the picked Selections in the SelectionList.

void DrawingView::ErasePickedHandles (SelectionList* pl) {
    for (pl->First(); !pl->AtEnd(); pl->Next()) {
	Selection* pick = pl->GetCur()->GetSelection();
	if (sl->Find(pick)) {
	    sl->GetCur()->GetSelection()->EraseHandles(rasterxor, canvas);
	}
    }
}

// EraseUngraspedHandles erases all of the handles only if the
// SelectionList does not already include the picked Selection.

void DrawingView::EraseUngraspedHandles (Selection* pick) {
    if (!sl->Find(pick)) {
	EraseHandles();
    }
}

// ResetAllHandles resets all of the handles because the Selections
// may have moved out from under their handles.

void DrawingView::ResetAllHandles () {
    PictSelection* picture = page->GetPicture();
    for (picture->First(); !picture->AtEnd(); picture->Next()) {
	Selection* s = picture->GetCurrent();
	s->ResetHandles();
    }
}

// Added adds the Selections to the list of Selections in the drawing
// to be drawn for the first time.

void DrawingView::Added () {
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
        damage->Added(sl->GetCur()->GetSelection());
    }
}

// Damaged adds the areas covered by the selected Selections
// (including their handles, too) to the list of damaged areas in the
// drawing to be repaired.

void DrawingView::Damaged () {
    BoxObj box;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	sl->GetCur()->GetSelection()->GetPaddedBox(box);
        damage->Incur(box);
    }
}

// Repair repairs the drawing's damaged areas and then redraws the
// Selections' handles.  The damaged areas must have included all the
// handles for RedrawHandles to work correctly.

void DrawingView::Repair () {
    if (damage->Incurred()) {
	damage->Repair();
	RedrawHandles();
    }
}

// Magnify magnifies the given area to fill the view.

void DrawingView::Magnify (Coord l, Coord b, Coord r, Coord t) {
    Perspective np;
    np = *GetPerspective();
    np.curx += min(l, r);
    np.cury += min(b, t);
    np.curwidth = max(abs(r - l), 1);
    np.curheight = max(abs(t - b), 1);
    Adjust(np);
}

// Reduce reduces the drawing's magnification by a factor of two.

void DrawingView::Reduce () {
    SetMagnification(GetMagnification() / 2);
}

// Enlarge enlarges the drawing's magnification by a factor of two.

void DrawingView::Enlarge () {
    SetMagnification(2 * GetMagnification());
}

// NormalSize resets the drawing's magnification.

void DrawingView::NormalSize () {
    SetMagnification(1.);
}

// ReduceToFit reduces the drawing's magnification enough to fit all
// of the drawing in the window.

void DrawingView::ReduceToFit () {
    Perspective np;
    np = *GetPerspective();
    np.curx = np.x0;
    np.cury = np.y0;
    np.curwidth = np.width;
    np.curheight = np.height;
    Adjust(np);
}

// CenterPage recenters the drawing over the window's center.

void DrawingView::CenterPage () {
    register Perspective* p = perspective;
    Coord left, bottom, right, top;

    page->Center(mag, xmax/2, ymax/2);

    GetGraphicBox(left, bottom, right, top);
    x0 = left;
    y0 = bottom;
    p->width = right - left + 2*pad;
    p->height = top - bottom + 2*pad;
    p->curwidth = xmax + 1;
    p->curheight = ymax + 1;
    p->curx = (p->width - p->curwidth)/2;
    p->cury = (p->height - p->curheight)/2;
    p->Update();
    Draw();
}

// Reconfig gives output the page's background color, creates a
// painter for drawing the rubberbands, and asks for the smallest
// possible canvas if the user wants a small window.

void DrawingView::Reconfig () {
    Color* bg = page->GetBackgroundColor();
    if (output->GetBgColor() != bg) {
	Painter* copy = new Painter(output);
	copy->Reference();
	Unref(output);
	output = copy;
	output->SetColors(output->GetFgColor(), bg);
    }
    if (rasterxor == nil) {
	rasterxor = new Painter(output);
	rasterxor->Reference();
    }
    if (strcmp(GetAttribute("small"), "true") == 0) {
	shape->width = 0;
	shape->height = 0;
    }
    GraphicBlock::Reconfig();
}

// Redraw draws a rectangular subpart of the view.

void DrawingView::Redraw (Coord l, Coord b, Coord r, Coord t) {
    if (graphic != nil) {
	GraphicBlock::Redraw(l, b, r, t);
	rasterxor->Clip(canvas, l, b, r, t);
	for (sl->First(); !sl->AtEnd(); sl->Next()) {
	    Selection* selection = sl->GetCur()->GetSelection();
	    selection->RedrawUnclippedHandles(rasterxor, canvas);
	}
	rasterxor->NoClip();
	RedrawTextEditor();
    }
}

// Resize recreates damage in case canvas changed its value.

void DrawingView::Resize () {
    GraphicBlock::Resize();
    delete damage;
    damage = new Damage(canvas, output, graphic);
    ResetAllHandles();
}

// LimitMagnification limits the factor by which DrawingView may scale
// the view of the drawing to avoid torturing the X server.  In
// addition, LimitMagnification updates State's stored magnification.
// Alternatively, State could have attached itself to DrawingView's
// perspective if it was an Interactor like MagnifView.

float DrawingView::LimitMagnification (float desired) {
    const float MIN = 1./8.;
    const float MAX = 16.;
    if (desired < MIN) {
	desired = MIN;
    } else if (desired > MAX) {
	desired = MAX;
    }
    state->SetMagnif(desired);
    return desired;
}

// StartTextEditing stores the textedit and draws it on the canvas.

void DrawingView::StartTextEditing (
    Event& e, TextEdit* textedit, Graphic* gs
) {
    DrawingView::textedit = textedit;
    DrawingView::gs = gs;

    page->Constrain(e.x, e.y);
    if (gs != nil) {
	state->SetTextGS(gs, output);
    } else {
	state->SetTextGS(e.x, e.y, output);
    }
    textedit->Redraw(
	state->GetTextPainter(), canvas, state->GetLineHt(), false
    );
}

// EndTextEditing marks the area damaged by the textedit for later
// repair and forgets it has a textedit.

void DrawingView::EndTextEditing () {
    Coord xmin, ymin, xmax, ymax;
    textedit->Bounds(xmin, ymin, xmax, ymax);
    damage->Incur(xmin, ymin, xmax, ymax);
    gs = nil;
    textedit = nil;
}

// RedrawTextEdit redraws the textedit on the canvas after a Resize or
// other asynchronous window event.

void DrawingView::RedrawTextEditor () {
    if (textedit != nil) {
	if (gs != nil) {
	    state->SetTextGS(gs, output);
	}
	textedit->Redraw(
	    state->GetTextPainter(), canvas, state->GetLineHt(), true
	);
    }
}

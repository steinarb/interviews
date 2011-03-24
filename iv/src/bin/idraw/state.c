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

// $Header: state.c,v 1.13 90/01/25 16:27:27 interran Exp $
// implements class State.

#include "ipaint.h"
#include "istring.h"
#include "listintrctr.h"
#include "mapipaint.h"
#include "page.h"
#include "state.h"
#include <InterViews/graphic.h>
#include <InterViews/interactor.h>
#include <InterViews/painter.h>
#include <InterViews/transformer.h>

// State stores some Graphic and nonGraphic attributes.

State::State (Interactor* i, Page* p) {
    drawingname = nil;
    graphicstate = new FullGraphic;
    graphicstate_t = new Transformer;
    magnif = 1.0;
    mapibrush = new MapIBrush(i, "brush");
    mapifgcolor = new MapIColor(i, "fgcolor");
    mapibgcolor = new MapIColor(i, "bgcolor");
    mapifont = new MapIFont(i, "font");
    mapipattern = new MapIPattern(i, "pattern");
    modifstatus = Unmodified;
    page = p;
    textgs = new FullGraphic;
    textgs_t = new Transformer;
    textpainter = new Painter;
    textpainter->Reference();
    textpainter_t = new Transformer;
    viewlist = new InteractorList;

    graphicstate->SetBrush(mapibrush->GetInitial());
    graphicstate->SetColors(
	mapifgcolor->GetInitial(), mapibgcolor->GetInitial()
    );
    graphicstate->FillBg(true);
    graphicstate->SetFont(mapifont->GetInitial());
    graphicstate->SetPattern(mapipattern->GetInitial());
    graphicstate->SetTransformer(graphicstate_t);
    textgs->SetTransformer(textgs_t);
    textpainter->SetTransformer(textpainter_t);
}

// ~State frees storage allocated to store members.

State::~State () {
    delete drawingname;
    delete graphicstate;
    delete mapibrush;
    delete mapifgcolor;
    delete mapibgcolor;
    delete mapifont;
    delete mapipattern;
    delete textgs;
    Unref(textpainter);
    delete viewlist;
}

// The following functions add Page operations to State.

void State::Constrain (Coord& x, Coord& y) {
    page->Constrain(x, y);
}

void State::ToggleOrientation () {
    page->ToggleOrientation();
}

// The following functions return Graphic and nonGraphic attributes of
// the State.

IBrush* State::GetBrush () {
    return (IBrush*) graphicstate->GetBrush();
}

IColor* State::GetFgColor () {
    return (IColor*) graphicstate->GetFgColor();
}

IColor* State::GetBgColor () {
    return (IColor*) graphicstate->GetBgColor();
}

const char* State::GetDrawingName () {
    return drawingname;
}

boolean State::GetFillBg () {
    return graphicstate->BgFilled();
}

IFont* State::GetFont () {
    return (IFont*) graphicstate->GetFont();
}

Graphic* State::GetGraphicGS () {
    return graphicstate;
}

boolean State::GetGridGravity () {
    return page->GetGridGravity();
}

double State::GetGridSpacing (boolean in_pixels) {
    return page->GetGridSpacing(in_pixels);
}

boolean State::GetGridVisibility () {
    return page->GetGridVisibility();
}

int State::GetLineHt () {
    return lineHt;
}

float State::GetMagnif () {
    return magnif;
}

MapIBrush* State::GetMapIBrush () {
    return mapibrush;
}

MapIColor* State::GetMapIFgColor () {
    return mapifgcolor;
}

MapIColor* State::GetMapIBgColor () {
    return mapibgcolor;
}

MapIFont* State::GetMapIFont () {
    return mapifont;
}

MapIPattern* State::GetMapIPattern () {
    return mapipattern;
}

ModifStatus State::GetModifStatus () {
    return modifstatus;
}

IPattern* State::GetPattern () {
    return (IPattern*) graphicstate->GetPattern();
}

Graphic* State::GetTextGS () {
    return textgs;
}

Painter* State::GetTextPainter () {
    return textpainter;
}

// The following functions set Graphic and nonGraphic attributes of
// the State.

void State::SetBrush (IBrush* b) {
    graphicstate->SetBrush(b);
}

void State::SetFgColor (IColor* fg) {
    graphicstate->SetColors(fg, graphicstate->GetBgColor());
}

void State::SetBgColor (IColor* bg) {
    graphicstate->SetColors(graphicstate->GetFgColor(), bg);
}

void State::SetDrawingName (const char* name) {
    delete drawingname;
    drawingname = name ? strdup(name) : nil;
}

void State::SetFillBg (boolean fill) {
    graphicstate->FillBg(fill);
}

void State::SetFont (IFont* f) {
    graphicstate->SetFont(f);
}

void State::SetGraphicT (Transformer& t) {
    *graphicstate_t = t;
    graphicstate_t->Invert();

    Transformer tnew;
    float left, top;
    graphicstate_t->InvTransform(textx, texty, left, top);
    tnew.Scale(magnif, magnif);
    tnew.Translate(left, top);
    *textpainter_t = tnew;
    tnew.Postmultiply(graphicstate_t);
    *textgs_t = tnew;
}

void State::SetGridGravity (boolean g) {
    page->SetGridGravity(g);
}

void State::SetGridSpacing (double s, boolean in_pixels) {
    page->SetGridSpacing(s, in_pixels);
}

void State::SetGridVisibility (boolean v) {
    page->SetGridVisibility(v);
}

void State::SetMagnif (float m) {
    magnif = m;
}

void State::SetModifStatus (ModifStatus m) {
    modifstatus = m;
}

void State::SetPattern (IPattern* p) {
    graphicstate->SetPattern(p);
}

void State::SetTextGS (Coord left, Coord top, Painter* output) {
    PColor* fg = graphicstate->GetFgColor();
    PFont* f = graphicstate->GetFont();
    lineHt = ((IFont*) f)->GetLineHt();
    textgs->SetColors(fg, textgs->GetBgColor());
    textgs->SetFont(f);
    textpainter->SetColors(*fg, output->GetBgColor());
    textpainter->SetFont(*f);
    graphicstate_t->Transform(float(left), float(top), textx, texty);

    Transformer t;
    t.Scale(magnif, magnif);
    t.Translate(left, top);
    *textpainter_t = t;
    t.Postmultiply(graphicstate_t);
    *textgs_t = t;
}

void State::SetTextGS (Graphic* gs, Painter* output) {
    PColor* fg = gs->GetFgColor();
    PFont* f = gs->GetFont();
    lineHt = ((IFont*) f)->GetLineHt();
    textgs->SetColors(fg, textgs->GetBgColor());
    textgs->SetFont(f);
    textpainter->SetColors(*fg, output->GetBgColor());
    textpainter->SetFont(*f);
    gs->TotalTransformation(*textpainter_t);
    Transformer* t = gs->GetTransformer();
    if (t != nil) {
	t->Transform(0.0, 0.0, textx, texty);
    }	
}

// Attach informs us a view has attached itself to us.

void State::Attach (Interactor* i) {
    viewlist->Append(new InteractorNode(i));
}

// Detach informs us a view has detached itself from us.

void State::Detach (Interactor* i) {
    if (viewlist->Find(i)) {
	viewlist->DeleteCur();
    }
}

// UpdateViews informs all attached views we have changed our state.

void State::UpdateViews () {
    for (viewlist->First(); !viewlist->AtEnd(); viewlist->Next()) {
	Interactor* view = viewlist->GetCur()->GetInteractor();
	view->Update();
    }
}

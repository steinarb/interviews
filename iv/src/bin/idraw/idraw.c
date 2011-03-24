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

// $Header: idraw.c,v 1.13 89/10/09 14:48:08 linton Exp $
// implements class Idraw.

#include "commands.h"
#include "drawing.h"
#include "drawingview.h"
#include "editor.h"
#include "errhandler.h"
#include "idraw.h"
#include "istring.h"
#include "mapkey.h"
#include "state.h"
#include "stateviews.h"
#include "tools.h"
#include <InterViews/Graphic/ppaint.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/cursor.h>
#include <InterViews/event.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/panner.h>
#include <InterViews/perspective.h>
#include <InterViews/sensor.h>
#include <InterViews/transformer.h>
#include <InterViews/tray.h>
#include <stdio.h>
#include <stdlib.h>

// Idraw parses its command line and initializes its members.

Idraw::Idraw (int argc, char** argv) {
    ParseArgs(argc, argv);
    InitPPaint();		// I keep forgetting to call this....
    Init();
}

// Free storage allocated for members not in Idraw's scene.

Idraw::~Idraw () {
    delete drawing;
    delete editor;
    delete errhandler;
    delete mapkey;
    delete state;
}

// Run opens the initial file if one was given before starting to run.

void Idraw::Run () {
    if (initialfile != nil) {
	SetCursor(hourglass);
	editor->Open(initialfile);
	SetCursor(defaultCursor);
    }

    Interactor::Run();
}

// Handle routes keystrokes to their associated interactors.

void Idraw::Handle (Event& e) {
    switch (e.eventType) {
    case KeyEvent:
	if (e.len > 0) {
	    Interactor* i = mapkey->LookUp(e.keystring[0]);
	    if (i != nil) {
		i->Handle(e);
	    }
	}
	break;
    default:
	break;
    }
}

// Update gets the picture's total tranformation matrix whenever it
// changes and stores it in State for creating new graphics.

void Idraw::Update () {
    Transformer t;
    drawing->GetPictureTT(t);
    state->SetGraphicT(t);
}

// ParseArgs stores the name of an initial file to open if any.

void Idraw::ParseArgs (int argc, char** argv) {
    initialfile	= nil;
    if (argc == 2) {
	initialfile = argv[1];
    } else if (argc > 2) {
	fprintf(stderr, "too many arguments, usage: idraw [file]\n");
	const int PARSINGERROR = 1;
	exit(PARSINGERROR);
    }
}

// Init creates a sensor to catch keystrokes, creates members and
// initializes links between them, and composes them into a view with
// boxes, borders, glue, and frames.

void Idraw::Init () {
    input = new Sensor;
    input->Catch(KeyEvent);

    drawing	= new Drawing(8.5*inches, 11*inches, 0.05*inch);
    drawingview	= new DrawingView(drawing->GetPage());
    editor	= new Editor(this);
    errhandler	= new ErrHandler;
    mapkey	= new MapKey;
    state	= new State(this, drawing->GetPage());
    tools	= new Tools(editor, mapkey);

    drawingview->GetPerspective()->Attach(this);
    drawingview->SetSelectionList(drawing->GetSelectionList());
    drawingview->SetState(state);
    drawingview->SetTools(tools);
    editor->SetDrawing(drawing);
    editor->SetDrawingView(drawingview);
    editor->SetState(state);
    errhandler->SetEditor(editor);
    errhandler->Install();

    VBox* status = new VBox(
        new HBox(
            new ModifStatusView(state),
            new DrawingNameView(state),
            new GriddingView(state),
            new FontView(state),
            new MagnifView(state, drawingview)
        ),
        new HBorder
    );
    HBox* indics = new HBox(
        new BrushView(state),
        new VBorder,
        new PatternView(state)
    );
    HBox* cmds = new HBox(
        new Commands(editor, mapkey, state),
        new HGlue
    );
    VBox* panel = new VBox(
        tools,
        new VGlue,
        new HBorder,
        new Panner(drawingview)
    );
    panel->Propagate(false);

    HBorder* hborder = new HBorder;
    VBorder* vborder = new VBorder;

    Tray* t = new Tray;

    t->HBox(t, status, t);
    t->HBox(t, indics, vborder, cmds, t);
    t->HBox(t, hborder, t);
    t->HBox(t, panel, vborder, drawingview, t);

    t->VBox(t, status, indics, hborder, panel, t);
    t->VBox(t, status, vborder, t);
    t->VBox(t, status, cmds, hborder, drawingview, t);

    Insert(new Frame(t, 1));
}

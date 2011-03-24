/*
 * Copyright (c) 1990, 1991 Stanford University
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
 * Unidraw class implementation.
 */

#include <Unidraw/catalog.h>
#include <Unidraw/editor.h>
#include <Unidraw/globals.h>
#include <Unidraw/keymap.h>
#include <Unidraw/selection.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/viewer.h>
#include <Unidraw/Components/component.h>
#include <Unidraw/Components/grview.h>

#include <InterViews/sensor.h>
#include <InterViews/window.h>
#include <InterViews/world.h>

#include <string.h>

/*****************************************************************************/

static void DetachComponentViews (Editor* ed) {
    int i = 0;
    Viewer* viewer;
    Component* comp = ed->GetComponent();

    if (comp != nil) {
        while ((viewer = ed->GetViewer(i++)) != nil) {
            GraphicView* gv = viewer->GetGraphicView();
            comp->Detach(gv);
        }
    }
}

/*****************************************************************************/

Editor::Editor () {
    _ref = 0;
    _window = nil;
    input = new Sensor;
    input->Catch(KeyEvent);
}

Editor::~Editor () { 
    delete _window;
}

Component* Editor::GetComponent () { return nil; }
Viewer* Editor::GetViewer (int) { return nil; }
KeyMap* Editor::GetKeyMap () { return nil; }
Tool* Editor::GetCurTool () { return nil; }
Selection* Editor::GetSelection () { return nil; }
StateVar* Editor::GetState (const char*) { return nil; }

void Editor::SetComponent (Component*) { }
void Editor::SetViewer (Viewer*, int) { }
void Editor::SetCurTool (Tool*) { }
void Editor::SetKeyMap (KeyMap*) { }
void Editor::SetSelection (Selection*) { }

void Editor::Open () { }

void Editor::Close () {
    Selection* s = GetSelection();

    if (s != nil) {
        s->Clear();
    }
    DetachComponentViews(this);
}

void Editor::Handle (Event& e) {
    if (e.eventType == KeyEvent) {
        HandleKey(e);
    }
}

void Editor::HandleKey (Event& e) {
    char* kcode;

    if (e.len > 0) {
	kcode = new char[e.len + 1];
	strncpy(kcode, e.keystring, e.len);
	kcode[e.len] = '\0';
	GetKeyMap()->Execute(kcode);
	delete kcode;
    }
}

void Editor::InsertDialog (Interactor* dialog) { 
    ManagedWindow* w = GetWindow();
    if (w != nil) w->deiconify();

    World* world = GetWorld();

    Coord x, y;
    Align(Center, 0, 0, x, y);
    GetRelative(x, y, world);

    world->InsertTransient(dialog, this, x, y, Center);
}

void Editor::RemoveDialog (Interactor* dialog) {
    GetWorld()->Remove(dialog);
}

void Editor::Ref () { ++_ref; }
void Editor::Unref () { if (--_ref == 0) delete this; }

void Editor::Update () { 
    Viewer* v;

    for (int i = 0; (v = GetViewer(i)) != nil; ++i) {
        v->Update();
    }
}

boolean Editor::DependsOn (Component* parent) {
    Component* child = GetComponent();

    while (child != nil) {
        if (parent == child) {
            return true;
        }
        child = child->GetParent();
    }
    return false;
}

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
 * Base composite interactor.
 */

#include <InterViews/scene.h>
#include <InterViews/shape.h>
#include <InterViews/canvas.h>
#include <InterViews/painter.h>
#include <InterViews/strtable.h>

Scene::Scene () {
    propagate = true;
}

Scene::Scene (Sensor* in, Painter* out) : (in, out) {
    propagate = true;
}

/*
 * Assign the actual location of an interactor and call its Resize operation
 * so that if it is a scene it can place its components.
 */

void Scene::Assign (register Interactor* i, Coord x, Coord y, int w, int h) {
    i->left = x;
    i->bottom = y;
    i->xmax = w - 1;
    i->ymax = h - 1;
    i->canvas->width = w;
    i->canvas->height = h;
    i->Resize();
}

void Scene::Insert (Interactor* component) {
    Interactor* i = Wrap(component);
    PrepareToInsert(i);
    DoInsert(i, false, i->left, i->bottom);
}

void Scene::Insert (Interactor* component, Coord x, Coord y, Alignment a) {
    Interactor* i = Wrap(component);
    PrepareToInsert(i);
    Coord ax = x, ay = y;
    DoAlign(i, a, ax, ay);
    DoInsert(i, true, ax, ay);
}

void Scene::PrepareToInsert (Interactor* i) {
    if (parent != nil || (canvas != nil && canvas->status == CanvasMapped)) {
	i->Config(this);
    }
}

void Scene::DoAlign (Interactor* i, Alignment a, Coord& x, Coord& y) {
    switch (a) {
	case TopLeft:
	case CenterLeft:
	case BottomLeft:
	    /* nothing to do */
	    break;
	case TopCenter:
	case Center:
	case BottomCenter:
	    x -= i->shape->width/2;
	    break;
	case TopRight:
	case CenterRight:
	case BottomRight:
	    x -= i->shape->width;
	    break;
    }
    switch (a) {
	case BottomLeft:
	case BottomCenter:
	case BottomRight:
	    /* nothing to do */
	    break;
	case CenterLeft:
	case Center:
	case CenterRight:
	    y -= i->shape->height/2;
	    break;
	case TopLeft:
	case TopCenter:
	case TopRight:
	    y -= i->shape->height;
	    break;
    }
}

void Scene::Change (Interactor* i) {
    if (propagate) {
	DoChange(i);
	if (parent != nil) {
	    parent->Change(this);
	}
    } else if (canvas != nil) {
	Resize();
    }
}

void Scene::Remove (Interactor* i) {
    DoRemove(i);
    i->parent = nil;
    if (i->canvas != nil) {
	Unmap(i);
	if (i->GetInteractorType() != InteriorInteractor) {
	    i->Deactivate();
	}
	Orphan(i);
    }
}

void Scene::Orphan (Interactor* i) {
    Interactor* children[100];
    Interactor** a;
    int n;

    i->GetComponents(children, sizeof(children) / sizeof(Interactor*), a, n);
    if (n > 0) {
	register int index;

	for (index = 0; index < n; index++) {
	    Orphan(a[index]);
	}
	if (a != children) {
	    delete a;
	}
    }
    delete i->canvas;
    i->canvas = nil;
}

void Scene::DoInsert (Interactor* i, boolean, Coord&, Coord&) { /* nop */ }
void Scene::DoChange (Interactor*) { /* nop */ }
void Scene::DoRemove (Interactor*) { /* nop */ }
void Scene::DoRaise (Interactor*) { /* nop */ }
void Scene::DoLower (Interactor*) { /* nop */ }
void Scene::DoMove (Interactor* i, Coord&, Coord&) { /* nop */ }

/*
 * Wrap is called to put any desired layer around an interactor
 * that is inserted into a scene.  The default is to simply
 * return the interactor as is.
 */

Interactor* Scene::Wrap (Interactor* i) {
    return i;
}

void Scene::Propagate (boolean b) {
    propagate = b;
}

/*
 * Highlight a scene by highlighting each components.
 */

void Scene::Highlight (boolean b) {
    Interactor* children[100];
    Interactor** a;
    int n;

    GetComponents(children, sizeof(children) / sizeof(Interactor*), a, n);
    if (n > 0) {
	for (register int index = 0; index < n; index++) {
	    a[index]->Highlight(b);
	}
	if (a != children) {
	    delete a;
	}
    }
}

/*
 * A common case is a scene with a single subcomponent.  This construct
 * occurs when one interactor is defined in terms of another, e.g.,
 * a menu is built out of a frame around a box.  The reason a MonoScene
 * is preferred over subclassing is that it simplies implementing the virtuals.
 * In the menu example, menus can handle events independently of frames.
 */

MonoScene::MonoScene () {
    component = nil;
}

MonoScene::MonoScene (Sensor* in, Painter* out) : (in, out) {
    component = nil;
}

MonoScene::~MonoScene () {
    if (component != nil) {
        delete component;
    }
}

void MonoScene::DoInsert (Interactor* i, boolean, Coord&, Coord&) {
    if (component != nil) {
	delete component;
    }
    component = i;
}

void MonoScene::DoChange (Interactor*) {
    Reconfig();
}

void MonoScene::DoRemove (Interactor*) {
    component = nil;
}

void MonoScene::Reconfig () {
    if (component != nil) {
	*shape = *component->GetShape();
    }
}

void MonoScene::Resize () {
    if (output != nil) {
	canvas->SetBackground(output->GetBgColor());
    }
    if (component != nil) {
	Place(component, 0, 0, xmax, ymax);
    }
}

void MonoScene::Draw () {
    Scene::Draw();
    if (component != nil) {
	component->Draw();
    }
}

void MonoScene::GetComponents (
    Interactor** c, int nc, Interactor**& a, int& n
) {
    if (component == nil) {
	n = 0;
    } else if (nc > 0) {
	n = 1;
	a = c;
	a[0] = component;
    } else {
	n = 1;
	a = new Interactor*[1];
	a[0] = component;
    }
}

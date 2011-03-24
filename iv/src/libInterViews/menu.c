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
 * Implementation of common menus.
 */

#include <InterViews/box.h>
#include <InterViews/event.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/menu.h>
#include <InterViews/message.h>
#include <InterViews/painter.h>
#include <InterViews/pattern.h>
#include <InterViews/shape.h>
#include <InterViews/world.h>
#include <stdlib.h>

/** class MenuItem **/

MenuItem::MenuItem(Interactor* i) : (i) { Init(); }
MenuItem::MenuItem(const char* name, Interactor* i) : (name, i) { Init(); }

MenuItem::MenuItem(
    const char* str, Alignment al
) : (str, new Message(str, al, 2, hfil, 0)) {
    Init();
}

MenuItem::MenuItem(
    const char* name,
    const char* str, Alignment al
) : (name, new Message(str, al, 2, hfil, 0)) {
    Init();
}

void MenuItem::Init() {
    SetClassName("MenuItem");
}

/** class MenuShadow **/

class MenuShadow : public MonoScene {
public:
    int depth;

    MenuShadow(Interactor*, int depth = 4);

    virtual void Reconfig();
    virtual void Resize();
    virtual void Redraw(Coord, Coord, Coord, Coord);
};

MenuShadow::MenuShadow(Interactor* i, int d) {
    depth = d;
    if (i != nil) {
	Insert(new Frame(i));
    }
}

void MenuShadow::Reconfig() {
    MonoScene::Reconfig();
    const char* d = GetAttribute("depth");
    if (d != nil) {
	depth = atoi(d);
    }
    shape->width += depth;
    shape->height += depth;
}

void MenuShadow::Resize() {
    if (component != nil) {
	Place(component, 0, depth, xmax - depth, ymax);
    }
}

void MenuShadow::Redraw(Coord x1, Coord y1, Coord x2, Coord y2) {
    if (x2 >= depth && y1 <= ymax - depth) {
	Pattern* p = output->GetPattern();
	boolean b = output->BgFilled();
	output->SetPattern(gray);
	output->FillBg(false);
	Coord left = max(x1, depth);
	Coord top = min(y2, ymax - depth);
	output->FillRect(canvas, left, y1, x2, top);
	output->SetPattern(p);
	output->FillBg(b);
    }
}

/** class Menu **/

Menu::Menu(Interactor* i) : (i) { Init(); }
Menu::Menu(const char* name, Interactor* i) : (name, i) { Init(); }

void Menu::Init() {
    SetClassName("Menu");
    state = new ControlState;
    state->Reference();
    body = nil;
    depth = 3;
    align = BottomLeft;
    scene = nil;
    world = nil;
    rel_x = 0;
    rel_y = 0;
    ins_x = 0;
    ins_y = 0;
}

Menu::~Menu() {
    delete body;
    Unref(state);
}

void Menu::SetBody(Interactor* i) { body = i; }
void Menu::SetAlign(Alignment a) { align = a; }
void Menu::SetDepth(int d) { depth = d; }

void Menu::SetBodyState(ControlState* s) {
    Unref(state);
    state = s;
    s->Reference();
}

void Menu::SetScene(Scene* s) {
    if (scene != nil) {
	delete scene;
    }
    scene = s;
    body = scene->Root();
}

void Menu::Include(Control* c) {
    if (scene == nil) {
	scene = new VBox;
	body = new MenuShadow(scene);
    }
    scene->Insert(c);
    c->SetState(GetBodyState());
}

void Menu::Reconfig() {
    Control::Reconfig();
    world = GetWorld();
    Setup();
}

void Menu::Setup() {
    body->Config(world);
    Shape* s = body->GetShape();
    rel_x = s->width / 2;
    rel_y = s->height / 2;
}

void Menu::Popup(Event& e) {
    World* w;
    Coord wx, wy;
    e.GetAbsolute(w, wx, wy);
    if (w != world) {
	world = w;
	Setup();
    }
    InsertBody(wx - rel_x, wy - rel_y);
    State()->Selection(this);
}

void Menu::Leave() { }

void Menu::Open() {
    Coord x, y;
    Align(align, 0, 0, x, y);
    GetRelative(x, y);
    InsertBody(x, y - body->GetShape()->height);
}

void Menu::InsertBody(Coord x, Coord y) {
    ins_x = x;
    ins_y = y;
    world->InsertPopup(body, x, y);
    State()->Push(state);
    state->Activate();
    world->Flush();
}

void Menu::Close() {
    Control* action = state->Action();
    if (action != nil) {
	Shape* s = action->GetShape();
	rel_x = s->width / 2;
	rel_y = s->height / 2;
	action->GetRelative(rel_x, rel_y, body);
    }
    state->Pop();
    world->Remove(body);
    world->Flush();
}

/** class MenuBar **/

MenuBar::MenuBar() { Init(); }
MenuBar::MenuBar(const char* name) { SetInstance(name); Init(); }

void MenuBar::Init() {
    SetClassName("MenuBar");
    state = new ControlState;
    state->Reference();
}

MenuBar::~MenuBar() {
    Unref(state);
}

void MenuBar::Include(Control* c) {
    Insert(c);
    c->SetState(state);
}

/** class PulldownMenu **/

PulldownMenu::PulldownMenu(Interactor* i) : (i) { Init(); }
PulldownMenu::PulldownMenu(
    const char* name, Interactor* i
) : (name, i) {
    Init();
}

PulldownMenu::PulldownMenu(
    const char* str
) : (str, new Message(str, Center, 2)) {
    Init();
}

PulldownMenu::PulldownMenu(
    const char* name, const char* str
) : (name, new Message(str, Center, 2)) {
    Init();
}

void PulldownMenu::Init() {
    SetClassName("PulldownMenu");
    SetAlign(BottomLeft);
}

/** class PullrightMenu **/

PullrightMenu::PullrightMenu(Interactor* i) : (i) { Init(); }
PullrightMenu::PullrightMenu(
    const char* name, Interactor* i
) : (name, i) {
    Init();
}

PullrightMenu::PullrightMenu(
    const char* str
) : (str, new Message(str, Left, 2)) {
    Init();
}

PullrightMenu::PullrightMenu(
    const char* name, const char* str
) : (name, new Message(str, Left, 2)) {
    Init();
}

void PullrightMenu::Init() {
    SetClassName("PullrightMenu");
    SetAlign(TopRight);
}

/** class PopupMenu **/

PopupMenu::PopupMenu() : ((Interactor*)nil) { Init(); }

void PopupMenu::Init() {
    SetClassName("PopupMenu");
    SetState(new ControlState);
    SetAlign(Center);
}

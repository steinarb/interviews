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
 * Common menus built on top of controls
 */

#ifndef menu_h
#define menu_h

#include <InterViews/box.h>
#include <InterViews/control.h>

class MenuItem : public Control {
public:
    MenuItem(Interactor*);
    MenuItem(const char* name, Interactor*);
    MenuItem(const char* str, Alignment = Left);
    MenuItem(const char* name, const char* str, Alignment = Left);
private:
    void Init();
};

class Menu : public Control {
public:
    Menu(Interactor*);
    Menu(const char* name, Interactor* i);
    ~Menu();

    void SetBody(Interactor*);
    Interactor* GetBody() { return body; }

    void SetAlign(Alignment);
    Alignment GetAlign() { return align; }

    void SetDepth(int);
    int GetDepth() { return depth; }

    void SetBodyState(ControlState*);
    ControlState* GetBodyState() { return state; }

    void SetScene(Scene*);
    Scene* GetScene() { return scene; }
    virtual void Include(Control*);

    virtual void Popup(Event&);

    Coord InsertX() { return ins_x; }
    Coord InsertY() { return ins_y; }
protected:
    virtual void Leave();
    virtual void Open();
    virtual void Close();

    virtual void Reconfig();
    virtual void Setup();
    virtual void InsertBody(Coord, Coord);
private:
    Interactor* body;
    Scene* scene;
    ControlState* state;
    Interactor* shadow;
    unsigned int depth : 16;
    unsigned int align : 16;
    class World* world;
    Coord ins_x, ins_y;
    Coord rel_x, rel_y;
private:
    void Init();
};

class MenuBar : public HBox {
public:
    MenuBar();
    MenuBar(const char* name);
    ~MenuBar();

    virtual void Include(Control*);
protected:
    ControlState* state;
private:
    void Init();
};

class PulldownMenu : public Menu {
public:
    PulldownMenu(Interactor* i);
    PulldownMenu(const char* name, Interactor* i);
    PulldownMenu(const char* str);
    PulldownMenu(const char* name, const char* str);
private:
    void Init();
};

class PullrightMenu : public Menu {
public:
    PullrightMenu(Interactor* i);
    PullrightMenu(const char* name, Interactor* i);
    PullrightMenu(const char* str);
    PullrightMenu(const char* name, const char* str);
private:
    void Init();
};

class PopupMenu : public Menu {
public:
    PopupMenu();
private:
    void Init();
};

#endif

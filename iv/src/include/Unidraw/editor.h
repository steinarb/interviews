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
 * Editor class from which domain-specific editors are derived.
 * An editor defines the interface to editing GraphicViews; it typically
 * consists of a viewer, command menus, and tool palettes.
 */

#ifndef unidraw_editor_h
#define unidraw_editor_h

#include <InterViews/scene.h>

class Component;
class KeyMap;
class ManagedWindow;
class Selection;
class StateVar;
class Tool;
class Viewer;

class Editor : public MonoScene {
public:
    virtual ~Editor();

    virtual void Open();
    virtual void Close();
    virtual void Handle(Event&);
    virtual void Update();

    virtual Component* GetComponent();
    virtual Viewer* GetViewer(int = 0);
    virtual KeyMap* GetKeyMap();
    virtual Tool* GetCurTool();
    virtual Selection* GetSelection();

    virtual void SetComponent(Component*);
    virtual void SetViewer(Viewer*, int = 0);
    virtual void SetKeyMap(KeyMap*);
    virtual void SetCurTool(Tool*);
    virtual void SetSelection(Selection*);

    virtual StateVar* GetState(const char*);

    virtual void InsertDialog(Interactor*);
    virtual void RemoveDialog(Interactor*);
protected:
    Editor();
    virtual void HandleKey(Event&);
    virtual boolean DependsOn(Component*);

    void SetWindow(ManagedWindow*);
    ManagedWindow* GetWindow();
private:
    friend class Unidraw;
    void Ref();
    void Unref();
private:
    int _ref;
    ManagedWindow* _window;
};

inline void Editor::SetWindow (ManagedWindow* w) { _window = w; }
inline ManagedWindow* Editor::GetWindow () { return _window; }

#endif

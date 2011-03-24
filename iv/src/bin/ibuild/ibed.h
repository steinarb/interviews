/*
 * Copyright (c) 1991 Stanford University
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
 * Main class for user interface builder, derived from Editor class.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibed.h,v 1.2 91/09/27 14:13:44 tang Exp $
 */

#ifndef ibed_h
#define ibed_h

#include <Unidraw/editor.h>

class ControlState;
class EditorInfo;
class GraphicComp;
class ToolPanel;

class IBEditor : public Editor {
public:
    IBEditor(GraphicComp* = nil);
    IBEditor(const char* file);
    virtual ~IBEditor();

    GraphicComp* GetGraphicComp();
    ToolPanel* GetToolPanel();

    virtual Component* GetComponent();
    virtual Viewer* GetViewer(int = 0);
    virtual KeyMap* GetKeyMap();
    virtual Tool* GetCurTool();
    virtual Selection* GetSelection();
    virtual StateVar* GetState(const char*);

    virtual void SetComponent(Component*);
    virtual void SetViewer(Viewer*, int = 0);
    virtual void SetKeyMap(KeyMap*);
    virtual void SetSelection(Selection*);

private:
    void Init(GraphicComp* = nil);
    void InitStateVars();
    void InitViewer();
    void InitTools();
    Interactor* Interior();

    Interactor* Commands();
    Interactor* Tools();

    void Include(class Command*, class PulldownMenu* = nil);
    void Include(Tool*, class Box*);

    PulldownMenu* FileMenu();
    PulldownMenu* EditMenu();
    PulldownMenu* CompositionMenu();

    PulldownMenu* FontMenu();
    PulldownMenu* BrushMenu();
    PulldownMenu* ColorMenu(const char* name, const char* attrib);
    PulldownMenu* AlignMenu();
    PulldownMenu* ViewMenu();
private:
    GraphicComp* _comp;
    KeyMap* _keymap;
    ControlState* _curCtrl;
    Viewer* _viewer;
    Selection* _selection;
    ToolPanel* _toolpanel;

    class CompNameVar* _name;
    class ModifStatusVar* _modifStatus;
    class MagnifVar* _magnif;

    class FontVar* _font;
    class BrushVar* _brush;
    class ColorVar* _color;
    class PatternVar* _pattern;
    class UControl* _tab;
};

inline GraphicComp* IBEditor::GetGraphicComp () { return _comp; }

#endif

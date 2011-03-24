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
 * IB editor main class implementation.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibed.c,v 1.2 91/09/27 14:08:04 tang Exp $
 */

#include "ibbox.h"
#include "ibbutton.h"
#include "ibclasses.h"
#include "ibcreator.h"
#include "ibcmds.h"
#include "ibdeck.h"
#include "ibdialog.h"
#include "ibed.h"
#include "ibeditor.h"
#include "ibframe.h"
#include "ibinteractor.h"
#include "ibkybd.h"
#include "ibmenu.h"
#include "ibscene.h"
#include "ibshaper.h"
#include "ibtoolpanel.h"
#include "ibtools.h"
#include "ibvars.h"
#include "ibviewport.h"

#include <Unidraw/catalog.h>
#include <Unidraw/ctrlinfo.h>
#include <Unidraw/editor.h>
#include <Unidraw/editorinfo.h>
#include <Unidraw/globals.h>
#include <Unidraw/keymap.h>
#include <Unidraw/kybd.h>
#include <Unidraw/page.h>
#include <Unidraw/selection.h>
#include <Unidraw/statevars.h>
#include <Unidraw/stateviews.h>
#include <Unidraw/uctrls.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/viewer.h>

#include <Unidraw/Commands/align.h>
#include <Unidraw/Commands/catcmds.h>
#include <Unidraw/Commands/edit.h>
#include <Unidraw/Commands/struct.h>
#include <Unidraw/Commands/transforms.h>
#include <Unidraw/Commands/viewcmds.h>

#include <Unidraw/Commands/brushcmd.h>
#include <Unidraw/Commands/colorcmd.h>
#include <Unidraw/Commands/font.h>

#include <Unidraw/Components/line.h>
#include <Unidraw/Components/rect.h>
#include <Unidraw/Components/text.h>

#include <Unidraw/Graphic/label.h>
#include <Unidraw/Graphic/polygons.h>
#include <Unidraw/Graphic/splines.h>

#include <Unidraw/Tools/grcomptool.h>
#include <Unidraw/Tools/magnify.h>
#include <Unidraw/Tools/move.h>
#include <Unidraw/Tools/reshape.h>
#include <Unidraw/Tools/select.h>
#include <Unidraw/Tools/stretch.h>

#include <InterViews/adjuster.h>
#include <InterViews/bitmap.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/menu.h>
#include <InterViews/message.h>
#include <InterViews/panner.h>
#include <InterViews/shape.h>
#include <InterViews/window.h>
#include <InterViews/world.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/

const float VIEWER_WIDTH = 0;
const float VIEWER_HEIGHT = 6;

/*****************************************************************************/

static const char* brAttrib = "border";
static const char* fontAttrib = "font";
static const char* fgAttrib = "fgcolor";
static const char* bgAttrib = "bgcolor";
static const char* patAttrib = "pattern";

static const char* initBrAttrib = "initialborder";
static const char* initFontAttrib = "initialfont";
static const char* initFgAttrib = "initialfgcolor";
static const char* initBgAttrib = "initialbgcolor";
static const char* initPatAttrib = "initialpattern";

/*****************************************************************************/

inline void InsertSeparator (PulldownMenu* pdm) {
    pdm->GetScene()->Insert(
        new VBox(
            new VGlue(2, 0, 0),
            new HBorder,
            new VGlue(2, 0, 0)
        )
    );
}

inline PulldownMenu* MakePulldown (const char* name) {
    return new PulldownMenu(new Message(name, Center, round(.1*cm)));
}

/*****************************************************************************/

IBEditor::IBEditor (GraphicComp* comp) {
    Init(comp);
}

IBEditor::IBEditor (const char* file) {
    if (file == nil) {
	Init();

    } else {
	Catalog* catalog = unidraw->GetCatalog();
	GraphicComp* comp;

	if (catalog->Retrieve(file, (Component*&) comp)) {
	    Init(comp);

	} else {
	    Init();
	    fprintf(stderr, "ibuild: couldn't open %s\n", file);
	}
    }
}

void IBEditor::Init(GraphicComp* comp) {
    SetClassName("IBEditor");

    ManagedWindow* window = new ApplicationWindow(this);
    window->name("InterViews interface builder");
    window->icon_name("ibuild");
    SetWindow(window);
    
    if (comp == nil) {
        SceneComp* scomp = new SceneComp;
        scomp->GetClassNameVar()->SetName("[root]");
        scomp->GetClassNameVar()->SetBaseClass("[root]");
        _comp = scomp;

    } else {
        _comp = comp;
    }
    _curCtrl = new ControlState;
    _selection = new Selection;
    _tab = nil;
    _keymap = new KeyMap;

    InitStateVars();
    InitViewer();

    Insert(new Frame(Interior()));
    _keymap->Execute(CODE_SELECT);
}

void IBEditor::InitStateVars () {
    _name = new CompNameVar(_comp);
    _modifStatus = new ModifStatusVar(_comp);
    _magnif = new MagnifVar;

    _font = new FontVar;
    _brush = new BrushVar;
    _color = new ColorVar;
    _pattern = new PatternVar;

    Catalog* catalog = unidraw->GetCatalog();

    const char* brIndex = catalog->GetAttribute(initBrAttrib);
    const char* fontIndex = catalog->GetAttribute(initFontAttrib);
    const char* patIndex = catalog->GetAttribute(initPatAttrib);
    const char* fgIndex = catalog->GetAttribute(initFgAttrib);
    const char* bgIndex = catalog->GetAttribute(initBgAttrib);

    _brush->SetBrush(catalog->ReadBrush(brAttrib, atoi(brIndex)));
    _font->SetFont(catalog->ReadFont(fontAttrib, atoi(fontIndex)));
    _pattern->SetPattern(catalog->ReadPattern(patAttrib, atoi(patIndex)));
    _color->SetColors(
        catalog->ReadColor(fgAttrib, atoi(fgIndex)),
        catalog->ReadColor(bgAttrib, atoi(bgIndex))
    );
    IBNameVar::SetUniqueFlag(true);

}

IBEditor::~IBEditor () {
    delete _keymap;
    delete _selection;
    delete _tab;
}

Component* IBEditor::GetComponent () { return _comp; }
Viewer* IBEditor::GetViewer (int id) { return (id == 0) ? _viewer : nil; }
KeyMap* IBEditor::GetKeyMap () { return _keymap; }
Selection* IBEditor::GetSelection () { return _selection; }
ToolPanel* IBEditor::GetToolPanel () { return _toolpanel; }

StateVar* IBEditor::GetState (const char* name) {
    if (strcmp(name, "FontVar") == 0) {
        return _font;
    } else if (strcmp(name, "BrushVar") == 0) {
        return _brush;
    } else if (strcmp(name, "ColorVar") == 0) {
        return _color;
    } else if (strcmp(name, "PatternVar") == 0) {
        return _pattern;
    } else if (strcmp(name, "CompNameVar") == 0) {
        return _name;
    } else if (strcmp(name, "ModifStatusVar") == 0) {
        return _modifStatus;
    } else if (strcmp(name, "MagnifVar") == 0) {
        return _magnif;
    } else {
        return Editor::GetState(name);
    }
}

Tool* IBEditor::GetCurTool () {
    UControl* c = (UControl*) _curCtrl->Selection();
    return (Tool*) c->GetControlInfo()->GetOwner();
}

void IBEditor::SetComponent (Component* comp) {
    if (comp == nil || comp->IsA(GRAPHIC_COMP)) {
        _comp = (GraphicComp*) comp;
    }
}

void IBEditor::SetViewer (Viewer* v, int i) { 
    if (i == 0) {
        _viewer = v;
    }
}

void IBEditor::SetKeyMap (KeyMap* k) { _keymap = k; }
void IBEditor::SetSelection (Selection* s) { _selection = s; }

Interactor* IBEditor::Interior () {
    int gap = round(.1*cm);
    int wpanner = round(1.5*cm);
    _toolpanel = new ToolPanel(_curCtrl, _keymap);

    HBox* attr = new HBox (
        new BrushVarView(_brush, _color),
        new VBorder,
        new PatternVarView(_pattern, _color),
        new VBorder,
        new HGlue,
        new FontVarView(_font, Right)
    );
    HBox* status = new HBox(
	new HBox (
            new BrushVarView(_brush, _color),
            new VBorder,
            new PatternVarView(_pattern, _color),
            new VBorder
        ),
	new HBox (
            new HGlue(gap, 0, 0),
            new ModifStatusVarView(_modifStatus),
            new CompNameVarView(_name, Left),
            new HGlue,
            new FontVarView(_font, Right),
            new MagnifVarView(_magnif, Right)
	)
    );        
    HBox* panel = new HBox(
	new VBox(	
	   status,
	   new HBorder,
	   new HBox(Tools(), new HGlue),
	   new HBorder,
	   new HBox(Commands(), new HGlue)
	),
	new VBorder,
        new Panner(_viewer, wpanner)
    );
    VBox* staticview = new VBox(
	_viewer,
	new HBorder,
	_toolpanel
    );
    panel->Propagate(false);
    staticview->Propagate(false);

    return new VBox(
        panel,
        new HBorder,
	staticview
    );
}

Interactor* IBEditor::Commands () {
    MenuBar* commands = new MenuBar;

    commands->Include(FileMenu());
    commands->Include(EditMenu());
    commands->Include(CompositionMenu());
    commands->Include(FontMenu());
    commands->Include(BrushMenu());
    commands->Include(ColorMenu("FgColor", fgAttrib));
    commands->Include(ColorMenu("BgColor", bgAttrib));
    commands->Include(AlignMenu());
    commands->Include(ViewMenu());

    return commands;
}

Interactor* IBEditor::Tools () {
    HBox* tools = new HBox;

    Include(
        new SelectTool(new ControlInfo("Select", KLBL_SELECT, CODE_SELECT)),
        tools
    );
    Include(
        new MoveTool(new ControlInfo("Move", KLBL_MOVE, CODE_MOVE)), tools
    );
    Include(
        new StretchTool(new ControlInfo("Resize", KLBL_STRETCH, CODE_STRETCH)),
        tools
    );
    Include(
        new ExamineTool(new ControlInfo("Examine", KLBL_EXAMINE,CODE_EXAMINE)),
        tools
    );
    Include(
        new RelateTool(new ControlInfo("Relate", KLBL_RELATE,CODE_RELATE)),
        tools
    );
    Include(
        new ReshapeTool(new ControlInfo("Edit", KLBL_RESHAPE, CODE_RESHAPE)),
        tools
    );
    Include(
        new MagnifyTool(new ControlInfo("Magnify", KLBL_MAGNIFY, CODE_MAGNIFY)),
        tools
    );
    Include(
        new NarrowTool(new ControlInfo("Narrow", KLBL_NARROW,CODE_NARROW)),
        tools
    );

    return tools;
}

static Graphic* ScreenBoundary () {
    Coord x[32], y[32];
    World* world = unidraw->GetWorld();
    Coord xmax = world->Width();
    Coord ymax = world->Height();
    Coord screenWidth = xmax + round(2*inches);
    Coord screenHeight = ymax + round(0.25*inches);

    Coord sx0 = 0, sy0 = 0;
    Coord sx1 = screenWidth;
    Coord sy1 = screenHeight;
    Coord dx = (sx1 - xmax) / 2;
    Coord dy = (sy1 - ymax) / 2;
    Coord fx0 = dx;
    Coord fy0 = dy;
    Coord fx1 = dx + xmax;
    Coord fy1 = dy + ymax;
    Coord sseamx = fx0 + xmax/2;
    Coord sseamy = 0;
    Coord fseamx = sseamx;
    Coord fseamy = fy0;
    
    x[0] = x[1] = x[2] = x[11] = x[12] = x[13] = sseamx;
    y[0] = y[1] = y[2] = y[11] = y[12] = y[13] = sseamy;

    x[3] = x[4] = x[5] = x[6] = sx1;
    x[7] = x[8] = x[9] = x[10] = sx0;

    y[3] = y[4] = sy0;
    y[5] = y[6] = y[7] = y[8] = sy1;
    y[9] = y[10] = sy0;

    x[14] = x[15] = x[16] = x[29] = x[30] = x[31] = fseamx;
    y[14] = y[15] = y[16] = y[29] = y[30] = y[31] = fseamy;

    x[17] = x[18] = x[19] = x[20] = x[21] = x[22] = fx0;
    x[23] = x[24] = x[25] = x[26] = x[27] = x[28] = fx1;

    y[17] = y[18] = y[19] = fy0;
    y[20] = y[21] = y[22] = y[23] = y[24] = y[25] = fy1;
    y[26] = y[27] = y[28] = fy0;

    return new F_ClosedBSpline(x, y, 32, stdgraphic);
}

void IBEditor::InitViewer () {
    const int vw = round(VIEWER_WIDTH * inches);
    const int vh = round(VIEWER_HEIGHT * inches);

    GraphicView* view = (GraphicView*) _comp->Create(COMPONENT_VIEW);
    _comp->Attach(view);
    _viewer = new Viewer(this, view, new Page(ScreenBoundary()), nil, vw, vh);
    view->Update();

}

void IBEditor::Include (Command* cmd, PulldownMenu* pdm) {
    ControlInfo* ctrlInfo = cmd->GetControlInfo();
    UControl* ctrl = new CommandControl(ctrlInfo);
    _keymap->Register(ctrl);
    if (pdm != nil) pdm->Include(ctrl);
    cmd->SetEditor(this);
}

void IBEditor::Include (Tool* tool, Box* box) {
    ControlInfo* ctrlInfo = tool->GetControlInfo();
    UControl* ctrl = new HPanelControl(ctrlInfo, _curCtrl);
    _keymap->Register(ctrl);
    box->Insert(ctrl);
}

PulldownMenu* IBEditor::FontMenu () {
    Catalog* catalog = unidraw->GetCatalog();
    PulldownMenu* pdm = MakePulldown("Font");

    int i = 1;
    PSFont* font = catalog->ReadFont(fontAttrib, i);

    while (font != nil) {
        TextGraphic* text = new TextGraphic(
            font->GetPrintFontAndSize(), stdgraphic
        );
        text->SetFont(font);

        Include(new FontCmd(new ControlInfo(new TextComp(text)), font), pdm);
        font = catalog->ReadFont(fontAttrib, ++i);
    }
    return pdm;
}

static float MENU_WIDTH = 1;   /* in cm */
static float MENU_HEIGHT = 0.5;

PulldownMenu* IBEditor::BrushMenu () {
    Catalog* catalog = unidraw->GetCatalog();
    PulldownMenu* pdm = MakePulldown("Border");

    int i = 1;
    PSBrush* br = catalog->ReadBrush(brAttrib, i);

    while (br != nil) {
        ControlInfo* ctrlInfo;

        if (br->None()) {
            ctrlInfo = new ControlInfo("None");

        } else {
            Line* line = new Line(0, 0, round(MENU_WIDTH*cm), 0, stdgraphic);
            line->SetBrush(br);
            ctrlInfo = new ControlInfo(new LineComp(line));
        }
        Include(new BrushCmd(ctrlInfo, br), pdm);
        br = catalog->ReadBrush(brAttrib, ++i);
    }
    return pdm;
}


PulldownMenu* IBEditor::ColorMenu (const char* name, const char* attrib){
    Catalog* catalog = unidraw->GetCatalog();
    PulldownMenu* pdm = MakePulldown(name);

    int i = 1;
    PSColor* color = catalog->ReadColor(attrib, i);

    while (color != nil) {
        ControlInfo* ctrlInfo;
        Coord w = round(MENU_WIDTH*cm);
        Coord h = round(MENU_HEIGHT*cm);

        SF_Rect* sfr = new SF_Rect(0, 0, w, h, stdgraphic);
        sfr->SetColors(color, color);
        ctrlInfo = new ControlInfo(new RectComp(sfr), color->GetName());

        if (strcmp(attrib, fgAttrib) == 0) {
            Include(new ColorCmd(ctrlInfo, color, nil), pdm);
        } else {
            Include(new ColorCmd(ctrlInfo, nil, color), pdm);
        }
        color = catalog->ReadColor(attrib, ++i);
    }
    return pdm;
}

PulldownMenu* IBEditor::AlignMenu () {
    PulldownMenu* pdm = MakePulldown("Align");

    Include(
        new AlignCmd(
            new ControlInfo("Left", KLBL_ALGNLEFT, CODE_ALGNLEFT),
            Left, Left
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("Right", KLBL_ALGNRIGHT, CODE_ALGNRIGHT),
            Right, Right
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("Bottom", KLBL_ALGNBOT, CODE_ALGNBOT),	
            Bottom, Bottom
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("Top", KLBL_ALGNTOP, CODE_ALGNTOP), Top, Top
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("Vert Center", KLBL_ALGNVCTR, CODE_ALGNVCTR),
            VertCenter, VertCenter
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("Horiz Center", KLBL_ALGNHCTR, CODE_ALGNHCTR),
            HorizCenter, HorizCenter
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("Center", KLBL_ALGNCTR, CODE_ALGNCTR),
            Center, Center
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("TopLeft", " ", ""), 
            TopLeft, TopLeft
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("CenterLeft", " ", ""), 
            CenterLeft, CenterLeft
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("BottomLeft", " ", ""), 
            BottomLeft, BottomLeft
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("TopCenter", " ", ""), 
            TopCenter, TopCenter
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("BottomCenter", " ", ""), 
            BottomCenter, BottomCenter
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("TopRight", " ", ""), 
            TopRight, TopRight
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("CenterRight", " ", ""), 
            CenterRight, CenterRight
        ), pdm
    );
    Include(
        new AlignCmd(
            new ControlInfo("BottomRight", " ", ""), 
            BottomRight, BottomRight
        ), pdm
    );

    return pdm;
}

PulldownMenu* IBEditor::FileMenu () {
    PulldownMenu* pdm = MakePulldown("File");

    Include(
        new AboutCmd(new ControlInfo("About ibuild", KLBL_ABOUT, CODE_ABOUT))
    );
    Include(
        new TabCmd(new ControlInfo("Tab", KLBL_TAB, CODE_TAB))
    );
    SceneComp* scomp = new SceneComp;
    scomp->GetClassNameVar()->SetName("[root]");
    scomp->GetClassNameVar()->SetBaseClass("[root]");
    Include(
        new NewCompCmd(
            new ControlInfo("New", KLBL_NEWCOMP, CODE_NEWCOMP), scomp
        ),
        pdm
    );
    Include(
        new RevertCmd(new ControlInfo("Revert", KLBL_REVERT, CODE_REVERT)),
        pdm
    );
    InsertSeparator(pdm);
    Include(
        new ViewCompCmd(
            new ControlInfo("Open...", KLBL_VIEWCOMP, CODE_VIEWCOMP)
        ), pdm
    );
    Include(
        new SaveCompCmd(new ControlInfo("Save", KLBL_SAVECOMP, CODE_SAVECOMP)),
        pdm
    );
    Include(
        new SaveCompAsCmd(
            new ControlInfo("Save As...", KLBL_SAVECOMPAS, CODE_SAVECOMPAS)
        ), pdm
    );
    Include(
        new CodeCmd(
            new ControlInfo("Generate...", KLBL_PRINT, CODE_PRINT)
        ), pdm
    );
    Include(
        new NewToolCmd(
            new ControlInfo("Create Tool...", KLBL_NEWTOOL, CODE_NEWTOOL)
        ), pdm
    );
    Include(
        new ToolsCmd(
            new ControlInfo("Tools...", KLBL_TOOLS, CODE_TOOLS)
        ), pdm
    );
    Include(
        new ExeCmd(
            new ControlInfo("Execute...", KLBL_EXE, CODE_EXE)
        ), pdm
    );
    InsertSeparator(pdm);
    Include(
        new QuitCmd(
            new ControlInfo("Quit", KLBL_QUIT, CODE_QUIT)
        ), pdm
    );

    return pdm;
}

PulldownMenu* IBEditor::EditMenu () {
    PulldownMenu* pdm = MakePulldown("Edit");

    Include(
        new UndoCmd(
            new ControlInfo("Undo", KLBL_UNDO, CODE_UNDO)
        ), pdm
    );
    Include(
        new RedoCmd(
            new ControlInfo("Redo", KLBL_REDO, CODE_REDO)
        ), pdm
    );
    InsertSeparator(pdm);
    Include(
        new CutCmd(
            new ControlInfo("Cut", KLBL_CUT, CODE_CUT)
        ), pdm
    );
    Include(
        new CopyCmd(
            new ControlInfo("Copy", KLBL_COPY, CODE_COPY)
        ), pdm
    );
    Include(
        new PasteCmd(
            new ControlInfo("Paste", KLBL_PASTE,CODE_PASTE)
        ), pdm
    );
    Include(
        new DupCmd(
            new ControlInfo("Duplicate", KLBL_DUP, CODE_DUP)
        ), pdm
    );
    Include(
        new DeleteCmd(
            new ControlInfo("Delete", KLBL_DEL, CODE_DEL)
        ), pdm
    );
    Include(
        new SlctAllCmd(
            new ControlInfo("Select All", KLBL_SLCTALL, CODE_SLCTALL)
        ), pdm
    );
    InsertSeparator(pdm);
    Include(
        new GlueVisibilityCmd(
            new ControlInfo("Show Glue", KLBL_SHOWGLUE, CODE_SHOWGLUE), true
        ), pdm
    );
    Include(
        new GlueVisibilityCmd(
            new ControlInfo("Hide Glue", KLBL_HIDEGLUE, CODE_HIDEGLUE), false
        ), pdm
    );
    Include(
        new PlaceCmd(
            new ControlInfo(
		"Natural Size", KLBL_NATURALSIZE, CODE_NATURALSIZE
	    ), nil
        ), pdm
    );

    return pdm;
}

PulldownMenu* IBEditor::CompositionMenu () {
    PulldownMenu* pdm = MakePulldown("Composition");

    Include(
        new UngroupCmd(
            new ControlInfo("Dissolve Scene", KLBL_UNGROUP, CODE_UNGROUP)
        ), pdm
    );
    InsertSeparator(pdm);
    Include(
        new SceneCmd(
            new ControlInfo("HBox", KLBL_HBOX, CODE_HBOX), new HBoxComp
        ), pdm
    );
    Include(
        new SceneCmd(
            new ControlInfo("VBox", KLBL_VBOX, CODE_VBOX), new VBoxComp
        ), pdm
    );
    Include(
        new SceneCmd(
            new ControlInfo("Deck", KLBL_DECK, CODE_DECK), new DeckComp
        ), pdm
    );
    FrameGraphic* framegr = new FrameGraphic(nil, stdgraphic);
    FrameComp* framecomp = new FrameComp(framegr);
    Include(
        new MonoSceneCmd(
            new ControlInfo("Frame", KLBL_FRAME, CODE_FRAME), framecomp
        ), pdm
    );
    ShadowFrameGraphic* shadowgr = new ShadowFrameGraphic(nil, stdgraphic);
    FrameComp* shadowframe = new FrameComp(shadowgr);
    Include(
        new MonoSceneCmd(
            new ControlInfo(
		"ShadowFrame", KLBL_SHADOWFRAME, CODE_SHADOWFRAME), shadowframe
        ), pdm
    );
    ViewportGraphic* viewportgr = new ViewportGraphic(nil, stdgraphic);
    ViewportComp* viewportcomp = new ViewportComp(viewportgr);
    Include(
        new MonoSceneCmd(
            new ControlInfo("Viewport", KLBL_VIEWPORT, CODE_VIEWPORT), 
	    viewportcomp
        ), pdm
    );

    Include(
        new SceneCmd(
            new ControlInfo("MenuBar", KLBL_MBCMD, CODE_MBCMD),new MenuBarComp
        ), pdm
    );

    Include(
        new SceneCmd(
            new ControlInfo("Shaper", KLBL_SHAPER, CODE_SHAPER),new ShaperComp
        ), pdm
    );
/*
    Include(
        new SceneCmd(
            new ControlInfo("Popup Menu", KLBL_PUCMD, CODE_PUCMD), 
	    new PopupMenuComp
        ), pdm
    );

    InsertSeparator(pdm);
*/

    InsertSeparator(pdm);

    MonoSceneClass* monoSceneClass = new MonoSceneClass(new IBGraphic);
    Include(
        new MonoSceneCmd(
            new ControlInfo(
		"MonoScene Subclass", KLBL_MSCLASS, CODE_MSCLASS
	    ), monoSceneClass
        ), pdm
    );
    DialogClass* dialogClass = new DialogClass(new IBGraphic);
    Include(
        new MonoSceneCmd(
            new ControlInfo(
		"Dialog Subclass", KLBL_DIALOGCLASS, CODE_DIALOGCLASS
	    ), dialogClass
        ), pdm
    );
    EditorComp* edcomp = new EditorComp(new IBGraphic);
    Include(
        new MonoSceneCmd(
            new ControlInfo(
		"Editor Subclass", KLBL_EDCOMP, CODE_EDCOMP
	    ), edcomp
        ), pdm
    );

    InsertSeparator(pdm);

    Include(
        new ReorderCmd(
            new ControlInfo("Reorder", KLBL_REORDER, CODE_REORDER)
        ), pdm
    );
    Include(
        new FrontCmd(
            new ControlInfo("Raise", KLBL_FRONT, CODE_FRONT)
        ), pdm
    );
    Include(
        new BackCmd(
            new ControlInfo("Lower", KLBL_BACK, CODE_BACK)
        ), pdm
    );

    return pdm;
}

PulldownMenu* IBEditor::ViewMenu () {
    PulldownMenu* pdm = MakePulldown("View");

    Include(
        new NewViewCmd(
            new ControlInfo("New View", KLBL_NEWVIEW, CODE_NEWVIEW)
        ), pdm
    );
    Include(
        new CloseEditorCmd(
            new ControlInfo("Close View", KLBL_CLOSEEDITOR, CODE_CLOSEEDITOR)
        ), pdm
    );
    InsertSeparator(pdm);

    Include(
        new NavigateCmd(
            new ControlInfo("View Parent", KLBL_VIEWPARENT, CODE_VIEWPARENT)
        ), pdm
    );
    Include(
        new NavigateCmd(
            new ControlInfo("View Root", KLBL_VIEWROOT, CODE_VIEWROOT), true
        ), pdm
    );
    InsertSeparator(pdm);

    Include(
        new NormSizeCmd(
            new ControlInfo("Normal Size", KLBL_NORMSIZE, CODE_NORMSIZE)
        ), pdm
    );
    Include(
        new RedToFitCmd(
            new ControlInfo("Reduce to Fit", KLBL_REDTOFIT, CODE_REDTOFIT)
        ), pdm
    );
    Include(
        new CenterCmd(
            new ControlInfo("Center Screen", KLBL_CENTER, CODE_CENTER)
        ), pdm
    );

    return pdm;
}


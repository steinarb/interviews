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
 * Implementation of PanelCtrl
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibpanelctrl.c,v 1.2 91/09/27 14:11:09 tang Exp $
 */

#include "ibbutton.h"
#include "ibclasses.h"
#include "ibgrcomp.h"
#include "ibcmds.h"
#include "ibdialogs.h"
#include "ibed.h"
#include "ibeditor.h"
#include "ibmanips.h"
#include "ibtext.h"
#include "ibtools.h"
#include "ibvars.h"
#include "ibvarviews.h"
#include "ibpanelctrl.h"
#include "ibcommandctrl.h"

#include <Unidraw/catalog.h>
#include <Unidraw/clipboard.h>
#include <Unidraw/iterator.h>
#include <Unidraw/page.h>
#include <Unidraw/statevars.h>
#include <Unidraw/selection.h>
#include <Unidraw/viewer.h>
#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/Tools/tool.h>
#include <Unidraw/Graphic/pspaint.h>

#include <InterViews/brush.h>
#include <InterViews/event.h>
#include <InterViews/painter.h>
#include <InterViews/rubrect.h>
#include <InterViews/shape.h>
#include <InterViews/transformer.h>

#include <OS/math.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/

static const int HPAD = 4;          // horizontal padding around control labels
static const int VPAD = 1;          // vertical padding around control labels
static const int SEP = 8;           // separation between label & equiv
static const int MINHT = 15;        // minimum height of control labels
static const int MINWD = 15;        // minimum width of control labels

static PSFont keyfont("fixed", "fixed", "12");
/*****************************************************************************/

PanelCtrlComp::PanelCtrlComp (PanelCtrlGraphic* gr) : GrBlockComp(gr) { 
    _curCtrlVar = nil;
    _keylabel = nil;
    _toolname = nil;
    _edVar = nil;

    if (gr != nil) {
        SubclassNameVar* subclass = GetClassNameVar();
        Orientation orient = gr->GetOrientation();
        const char* name = gr->GetName();

        subclass->SetName(name);
        subclass->SetBaseClass(name);
        ITextComp* itext = new ITextComp(new TextGraphic(name, stdgraphic));
        itext->Instantiate();
        GetTop()->Append(itext);

        Catalog* catalog = unidraw->GetCatalog();
        PSColor* fg = catalog->FindColor("Black");
        PSColor* bg = catalog->FindColor("White");

        _keylabel = new ITextComp(new TextGraphic("?", stdgraphic));
        _keylabel->Instantiate();
        Graphic* keygr = _keylabel->GetGraphic();
        keygr->SetColors(fg, bg);
        keygr->SetFont(&keyfont);
        SceneComp::Append(_keylabel);
    }
}

PanelCtrlComp::~PanelCtrlComp () { 
    delete _curCtrlVar;
    delete _toolname;
    delete _edVar;
    if (_keylabel != nil) SceneComp::Remove(_keylabel);
    delete _keylabel;
}

ClassId PanelCtrlComp::GetClassId () { return PANELCONTROL_COMP; }

boolean PanelCtrlComp::IsA (ClassId id) {
    return PANELCONTROL_COMP == id || GrBlockComp::IsA(id);
}

PanelCtrlGraphic* PanelCtrlComp::GetPanelCtrlGraphic() {
    return (PanelCtrlGraphic*) GetGrBlockGraphic();
}

void PanelCtrlComp::Instantiate () {
    GrBlockComp::Instantiate();
    if (_curCtrlVar == nil) {
        _curCtrlVar = new ButtonStateVar("curCtrl");
        _curCtrlVar->GenNewName();
        _curCtrlVar->HideSetting();
    }
    if (_toolname == nil) {
        _toolname = new TrackNameVar("GraphicCompTool");
    }
    if (_edVar == nil) {
        _edVar = new MemberNameVar("", false, false);
    }
}

void PanelCtrlComp::Reconfig () {
    Shape* shape = GetShapeVar()->GetShape();
    Orientation orient = GetPanelCtrlGraphic()->GetOrientation();
    int x0, y0, x1, y1;
    TextComp* textcomp = (TextComp*) GetKeyLabel()->GetTarget();
    TextGraphic* textgr = textcomp->GetText();
    const char* kl = textgr->GetOriginal();

    GetTop()->GetGraphic()->GetBox(x0, y0, x1, y1);
    shape->width = x1 - x0;
    shape->height = y1 - y0;

    if (*kl == '\0') {
        shape->width += 2*HPAD;
        shape->height += 2*VPAD;

    } else {
	PSFont* f = stdgraphic->GetFont();
	shape->width += 2 * f->Width(kl) + HPAD;
	shape->height += f->Height();
    }
    shape->height = max(shape->height, MINHT);

    if (orient == Horizontal) {
        shape->Rigid(0, shape->width, 0, vfil);

    } else if (orient == Vertical) {
        shape->Rigid(0, hfil, shape->height, 0);
    }        
}

void PanelCtrlComp::Resize () {
    Iterator i;

    PanelCtrlGraphic* pcgr = GetPanelCtrlGraphic();
    TextComp* textcomp = (TextComp*) GetKeyLabel()->GetTarget();
    TextGraphic* textgr = textcomp->GetText();
    const char* kl = textgr->GetOriginal();

    pcgr->SetPattern(psclear);
    pcgr->Align(Center, textgr, Center);

    Picture* topgr = (Picture*) GetTop()->GetGraphic();

    if (!topgr->IsEmpty()) {
        topgr->Align(Center, pcgr, Center);
        pcgr->Align(BottomRight, textgr, BottomRight);
        textgr->Translate(-2.0, 0.0);

    } else {
        pcgr->Align(BottomRight, textgr, BottomRight);
        textgr->Translate(-2.0, 0.0);
    }
    textcomp->Notify();
    SubNotify();
}

void PanelCtrlComp::Interpret(Command* cmd) {
    if (cmd->IsA(GETCONFLICT_CMD)) {
        GetConflictCmd* gcmd = (GetConflictCmd*) cmd;
        const char* cname = gcmd->GetCName();
        UList* conflictlist = gcmd->GetConflict();
        const char* curCtrl = _curCtrlVar->GetName();
        
        if (strcmp(curCtrl, cname) == 0) {
            conflictlist->Append(new UList(_curCtrlVar->GetButtonSharedName()));
        }

    } else if (
        cmd->IsA(COLOR_CMD) || cmd->IsA(FONT_CMD) || 
        cmd->IsA(BRUSH_CMD) && !cmd->IsA(GLUEVISIBILITY_CMD)
    ) {
        Iterator i;
        GraphicComp* top = GetTop();
        for (top->First(i); !top->Done(i); top->Next(i)) {
            top->GetComp(i)->Interpret(cmd);
        }
        Propagate(cmd);
        
    } else {
        GrBlockComp::Interpret(cmd);
    }
}

void PanelCtrlComp::Uninterpret(Command* cmd) {
    if (
        cmd->IsA(COLOR_CMD) || cmd->IsA(FONT_CMD) || 
        cmd->IsA(BRUSH_CMD) && !cmd->IsA(GLUEVISIBILITY_CMD)
    ) {
        Iterator i;
        GraphicComp* top = GetTop();
        for (top->First(i); !top->Done(i); top->Next(i)) {
            top->GetComp(i)->Uninterpret(cmd);
        }
        Unpropagate(cmd);
        
    } else {
        GrBlockComp::Uninterpret(cmd);
    }
}

void PanelCtrlComp::SetState(const char* name, StateVar* stateVar) { 
    if (strcmp(name, "CurCtrlVar") == 0) {
        ButtonStateVar* curCtrlVar = (ButtonStateVar*) stateVar;
        *_curCtrlVar = *curCtrlVar;

    } else if (strcmp(name, "ToolName") == 0) {
        _toolname = (TrackNameVar*) stateVar;

    } else if (
        strcmp(name, "EditorVar") == 0 || strcmp(name, "RelatedVar") == 0
    ) {
        MemberNameVar* memberVar = (MemberNameVar*) stateVar;
        *_edVar = *memberVar;

    } else {
        GrBlockComp::SetState(name, stateVar);
    }
}

StateVar* PanelCtrlComp::GetState (const char* name) {
    StateVar* stateVar = nil;

    if (strcmp(name, "CurCtrlVar") == 0) {
        stateVar = _curCtrlVar;

    } else if (strcmp(name, "ToolName") == 0) {
        stateVar = _toolname;

    } else if (strcmp(name, "EditorVar") == 0) {
        stateVar = _edVar;

    } else {
        stateVar = GrBlockComp::GetState(name);
    }

    return stateVar;
}

InteractorComp& PanelCtrlComp::operator = (InteractorComp& comp) {
    if (comp.IsA(EDITOR_COMP)) {
        EditorComp* edcomp = (EditorComp*) &comp;
        ButtonStateVar* curCtrlVar = edcomp->GetButtonStateVar();
        *_curCtrlVar = *curCtrlVar;
        *_edVar = *edcomp->GetMemberNameVar();
        
    } else if (comp.IsA(PANELCONTROL_COMP)) {
        PanelCtrlComp* pcComp = (PanelCtrlComp*) &comp;
        ButtonStateVar* curCtrlVar = pcComp->GetButtonStateVar();
        *_curCtrlVar = *curCtrlVar;
        *_edVar = *pcComp->GetEditorVar();
        
    } else if (comp.IsA(COMMANDCONTROL_COMP)) {
        CommandCtrlComp* comComp = (CommandCtrlComp*) &comp;
        *_edVar = *comComp->GetEditorVar();
        
    }
    return *this;
}

boolean PanelCtrlComp::IsRelatableTo (InteractorComp* comp) {
    boolean ok = false;
    if (comp->IsA(EDITOR_COMP) || comp->IsA(PANELCONTROL_COMP)) {
        ok = true;
    }
    return ok;
}

void PanelCtrlComp::Read (istream& in) {
    Catalog* catalog = unidraw->GetCatalog();
    PSColor* fg = catalog->FindColor("Black");
    PSColor* bg = catalog->FindColor("White");
    
    GrBlockComp::Read(in);

    delete _curCtrlVar;
    delete _keylabel;
    delete _toolname;
    delete _edVar;

    _keylabel = (ITextComp*) catalog->ReadComponent(in);
    _curCtrlVar = (ButtonStateVar*) catalog->ReadStateVar(in);
    _toolname = (TrackNameVar*) catalog->ReadStateVar(in);
    _edVar = (MemberNameVar*) catalog->ReadStateVar(in);

    Graphic* keygr = _keylabel->GetGraphic();
    keygr->SetColors(fg, bg);
    keygr->SetFont(&keyfont);

    SceneComp::Append(_keylabel);
}

void PanelCtrlComp::Write (ostream& out) {
    Catalog* catalog = unidraw->GetCatalog();
    GrBlockComp::Write(out);
    
    catalog->WriteComponent(_keylabel, out);
    catalog->WriteStateVar(_curCtrlVar, out);
    catalog->WriteStateVar(_toolname, out);
    catalog->WriteStateVar(_edVar, out);
}

/*****************************************************************************/

PanelCtrlView::PanelCtrlView (
    PanelCtrlComp* subj
) : GrBlockView(subj) { _keylabel = nil; }


PanelCtrlComp* PanelCtrlView::GetPanelCtrlComp () {
    return (PanelCtrlComp*) GetSubject();
}

ClassId PanelCtrlView::GetClassId () { return PANELCONTROL_VIEW; }

boolean PanelCtrlView::IsA (ClassId id) {
    return PANELCONTROL_VIEW == id || GrBlockView::IsA(id);
}

Manipulator* PanelCtrlView::CreateManipulator (
    Viewer* v, Event& e, Transformer* rel, Tool* tool
) {
    Manipulator* m = nil;

    if (tool->IsA(IBGRAPHIC_COMP_TOOL)) {
	m = InteractorView::CreateManipulator(v, e, rel, tool);

    } else if (tool->IsA(GRAPHIC_COMP_TOOL)) {
        Coord x0, y0, x1, y1, halfw, halfh;

        Editor* ed = v->GetEditor();

        GraphicView* views = v->GetGraphicView();
        PanelCtrlComp* comp = GetPanelCtrlComp();
        Shape* shape = comp->GetShapeVar()->GetShape();
        
        FontVar* fontVar = (FontVar*) ed->GetState("FontVar");
        Graphic* topgr = comp->GetTop()->GetGraphic();
        PSFont* font = topgr->GetFont();

        if (font != nil) {
            font->ref();
        }
        topgr->SetFont(fontVar->GetFont());
        comp->Reconfig();
        topgr->SetFont(font);
        if (font != nil) {
            font->unref();
        }

        x0 = 0; y0 = 0;
        x1 = shape->width;
        y1 = shape->height;

        Selection* s = v->GetSelection();
        SlidingRect* sr;
        
        s->Clear();
        if (rel != nil) {
            rel->Transform(x0, y0);
            rel->Transform(x1, y1);
            halfw = Math::abs(x1 - x0) / 2;
            halfh = Math::abs(y1 - y0) / 2;
        }
        v->Constrain(e.x, e.y);
        sr = new SlidingRect(
            nil, nil, e.x - halfw, e.y - halfh, e.x + halfw, 
            e.y + halfh, e.x, e.y
        );
        m = new DragManip(v, sr, rel, tool, Gravity);
        
    } else if (tool->IsA(TAB_TOOL)) {
        Iterator i;
        First(i);
        GraphicView* gv = GetView(i);
        if (gv != nil) {
            m = gv->CreateManipulator(v, e, rel, tool);
        }

    } else {
        m = GrBlockView::CreateManipulator(v, e, rel, tool);
    }
    return m;
}

Command* PanelCtrlView::InterpretManipulator(Manipulator* m) {
    Command* cmd = nil;
    Tool* tool = m->GetTool();
    if (tool->IsA(TAB_TOOL)) {
        Iterator i;
        First(i);
        GraphicView* gv = GetView(i);
        if (gv != nil) {
            cmd = gv->InterpretManipulator(m);
        }
    } else {
        cmd = GrBlockView::InterpretManipulator(m);
    }
    return cmd;
}

void PanelCtrlView::Interpret(Command* cmd) {
    if (cmd->IsA(TAB_CMD)) {
        TabTool tabTool;
        GetViewer()->UseTool(&tabTool);
        GrBlockView::Interpret(cmd);
    } else {
        GrBlockView::Interpret(cmd);
    }
}


Graphic* PanelCtrlView::GetGraphic () {
    Graphic* g = GraphicView::GetGraphic();

    if (g == nil) {
        g = GrBlockView::GetGraphic();
        ITextComp* keycomp = GetPanelCtrlComp()->GetKeyLabel();
        _keylabel = (IView*) keycomp->Create(COMPONENT_VIEW);
        keycomp->Attach(_keylabel);
        Append(_keylabel);
        _keylabel->Update();
    }
    return g;
}

void PanelCtrlView::Update () {
    if (_keylabel != nil) {
        Viewer* viewer = GetViewer();
        if (Includes(_keylabel)) {
            Iterator i;
            SetView(_keylabel, i);
            Remove(i);
        }
        GrBlockView::Update();

        if (viewer == nil || viewer->GetGraphicView() != this) {
            Append(_keylabel);
            _keylabel->Update();
        }
    } else {
        GrBlockView::Update();
    }
}

InfoDialog* PanelCtrlView::GetInfoDialog () {
    IBEditor* ibed = (IBEditor*) GetViewer()->GetEditor();
    InfoDialog* info = InteractorView::GetInfoDialog();
    ButtonState* state = info->GetState();
    PanelCtrlComp* pcComp = GetPanelCtrlComp();

    ButtonStateVar* curCtrlVar = pcComp->GetButtonStateVar();
    TrackNameVar* toolname = pcComp->GetToolName();
    MemberNameVar* edVar = pcComp->GetEditorVar();

    info->Include(new RelatedVarView(
        edVar, state, pcComp, "Editor Name: ")
    );
    NameChooserView* toolChooser = new NameChooserView(
        toolname, state, ibed,
        "Library tools:", "Tool Name: "
    );
    toolChooser->Append("ConnectTool");
    toolChooser->Append("GraphicCompTool");
    toolChooser->Append("MagnifyTool");
    toolChooser->Append("MoveTool");
    toolChooser->Append("ReshapeTool");
    toolChooser->Append("RotateTool");
    toolChooser->Append("ScaleTool");
    toolChooser->Append("SelectTool");
    toolChooser->Append("StretchTool");

    info->Include(toolChooser);
    info->Include(new CtrlStateVarView(curCtrlVar, state, pcComp, ibed));
    return info;
}

GraphicComp* PanelCtrlView::CreateProtoComp (
    Editor* ed, Coord l, Coord b, Coord r, Coord t
) {
    ColorVar* colVar = (ColorVar*) ed->GetState("ColorVar");
    FontVar* fontVar = (FontVar*) ed->GetState("FontVar");
    
    float mcx, mcy, cx, cy;
    PanelCtrlComp* pccomp = (PanelCtrlComp*) GetPanelCtrlComp()->Copy();
    Graphic* topgr = pccomp->GetTop()->GetGraphic();
    mcx = (r-l)/2.0; mcy = (t-b)/2.0;

    topgr->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
    topgr->SetFont(fontVar->GetFont());
    
    topgr->GetCenter(cx, cy);
    topgr->Translate(mcx-cx, mcy-cy);
    return pccomp;
}
/*****************************************************************************/

PanelCtrlCode::PanelCtrlCode (PanelCtrlComp* subj) : GrBlockCode(subj) { }

PanelCtrlComp* PanelCtrlCode::GetPanelCtrlComp() {
    return (PanelCtrlComp*) GetSubject();
}

ClassId PanelCtrlCode::GetClassId () { return PANELCONTROL_CODE; }

boolean PanelCtrlCode::IsA(ClassId id) {
    return PANELCONTROL_CODE == id || GrBlockCode::IsA(id);
}

boolean PanelCtrlCode::SingleKid () {
    Iterator i;
    First(i);
    Next(i);
    return Done(i);
}

void PanelCtrlCode::HashKeyCode(char* keycode) {
    int l = strlen(keycode);
    if (l == 0 || l > 2) {
        strcpy(keycode, " ");
    } else if (l == 2) {
        if (keycode[0] == '^' && keycode[1] >= 'A' && keycode[1] <= 'Z') {
            sprintf(keycode, "\\0%2o", keycode[1]-64);
        } else {
            strcpy(keycode, " ");
        }
    }
}    
    
boolean PanelCtrlCode::Definition (ostream& out) {
    char coreclass[CHARBUFSIZE];
    char Keycode[CHARBUFSIZE];
    boolean ok = true;
    Iterator i;

    PanelCtrlComp* pcComp = GetPanelCtrlComp();
    SubclassNameVar* snamer = pcComp->GetClassNameVar();
    MemberNameVar* mnamer = pcComp->GetMemberNameVar();
    TextComp* textcomp = (TextComp*) pcComp->GetKeyLabel()->GetTarget();
    TextGraphic* textgr = textcomp->GetText();
    ButtonStateVar* curCtrlVar = pcComp->GetButtonStateVar();
    MemberNameVar* edVar = pcComp->GetEditorVar();
    const char* edname = edVar->GetName();

    const char* text = textgr->GetOriginal();
    const char* tool = pcComp->GetToolName()->GetName();
    const char* ctrl = curCtrlVar->GetName();

    const char* subclass = snamer->GetName();
    const char* baseclass = snamer->GetBaseClass();
    const char* mname = mnamer->GetName();
    strcpy(Keycode, text);
    HashKeyCode(Keycode);
    InteractorComp* dummy;

    GetCoreClassName(coreclass);
    if (*edname == '\0') {
        strcat(_errbuf, mname);
        strcat(_errbuf, " has undefined Editor.\n");
        return false;

    } else if (!Search(edVar, dummy)) {
        strcat(_errbuf, mname);
        strcat(
            _errbuf, "'s Editor is not in the same hierachy.\n"
        );
        return false;
    }

    _emitGraphicComp = true;

    if (
        _emitInstanceDecls || _emitClassHeaders || 
        _emitHeaders || _emitProperty || _emitForward
    ) {
        ok = ok && GrBlockCode::Definition(out);

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (_scope && mnamer->GetExport()&&!_namelist->Search("uctrls")) {
                _namelist->Append("uctrls");
                out << "#include <Unidraw/uctrls.h> \n";
            }
        } else {
            ok = ok && CodeView::Definition(out);
        }
        ok = ok && Iterate(out);

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("control")) {
                _namelist->Append("control");
                out << "#include <InterViews/control.h>\n";
            }
            if (!_namelist->Search("uctrls")) {
                _namelist->Append("uctrls");
                out << "#include <Unidraw/uctrls.h>\n";
            }
        }
        ok = ok && Iterate(out);

    } else if (_emitInstanceInits) {
        if (!_instancelist->Find((void*)mname)) {
            _instancelist->Append(new UList((void*)mname));

            ok = ok && EmitGraphicState(out);

            for(First(i); !Done(i); Next(i)) {
                CodeView* kid = (CodeView*) GetView(i);
                ok = ok && EmitInstanceDecls(kid, out);
            }
            ok = ok && Iterate(out);

            const char* kidname = nil;

            if (!SingleKid()) {
                
                out << "    GraphicComps* " << mname;
                out << "_comp = new GraphicComps;\n";
                
                for(First(i); !Done(i); Next(i)) {
                    CodeView* kid = (CodeView*) GetView(i);
                    MemberNameVar* kmnamer=kid->GetIComp()->GetMemberNameVar();
                    const char* kmname = kmnamer->GetName();
                    out << "    " << mname << "_comp->Append(";
                    out << kmname << "_comp);\n";
                }
            } else {
                First(i);
                CodeView* kidv = (CodeView*) GetView(i);
                MemberNameVar* kmnamer=kidv->GetIComp()->GetMemberNameVar();
                kidname = kmnamer->GetName();
            }
            out << "    ControlInfo* " << mname;
            out << "_info = new ControlInfo(";

            if (kidname == nil) {
                out << mname;
            } else {
                out << kidname;
            }
            out << "_comp, \"" << text << "\", \"" << Keycode << "\");\n";
            out << "    Tool* " << mname << "_tool = new ";

            if (*tool == '\0' || strcmp(tool, "GraphicCompTool") == 0) {
                out << "GraphicCompTool(" << mname << "_info, (GraphicComp*) ";
                if (kidname == nil) {
                    out << mname;
                } else {
                    out << kidname;
                }
                out << "_comp->Copy());\n";

            } else {
                out << tool << "(" << mname << "_info);\n";
            }
            BeginInstantiate(out);
            out << "(";
            InstanceName(out);
            out << mname << "_info, " << ctrl << ")";
            EndInstantiate(out);
            out << "    GetKeyMap()->Register(";
            out << mname << ");\n";
        }

    } else if (_emitBSDecls) {
        ButtonStateVar* bsVar = pcComp->GetButtonStateVar();
	const char* name = bsVar->GetName();

	if (_emitExport) {
	    if (bsVar->GetExport() && !_bsdeclslist->Search(name)) {
                _bsdeclslist->Append(name);
                out << "    ControlState* " << name << ";\n";
	    }

	} else {
            if (!bsVar->GetExport() && !_bsdeclslist->Search(name)) {
                _bsdeclslist->Append(name);
                out << "    ControlState* " << name << ";\n";
            }
	}

    } else if (_emitBSInits) {
        ButtonStateVar* bsVar = pcComp->GetButtonStateVar();
	const char* name = bsVar->GetName();
        boolean export = bsVar->GetExport();

	if (!_bsinitslist->Search(name)) {
	    _bsinitslist->Append(name);
            if (export) {
                out << "    " << name;

            } else {
                out << "    ControlState*" << name;
            }
            out << " = new ControlState;\n";
        }
    } else if (_emitFunctionDecls || _emitFunctionInits) {
	ok = true;

    } else if (
        _emitCoreDecls || _emitCoreInits || _emitClassDecls || _emitClassInits
    ) {
	ok = ok && GrBlockCode::Definition(out);
        
    } else if (_emitMain) {
	ok = ok && GrBlockCode::Definition(out);
        
    }
    _emitGraphicComp = true;

    return out.good() && ok;
}

boolean PanelCtrlCode::CoreConstDecls(ostream& out) { 
    out << "(const char*, ControlInfo*, ControlState* = nil);\n";
    return out.good();
}

boolean PanelCtrlCode::CoreConstInits(ostream& out) {
    InteractorComp* icomp = GetIntComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    const char* name, ControlInfo* info, ControlState* state";
    out << "\n) : ";
    out << baseclass << "(name, info, state) {\n";
    out << "    perspective = new Perspective;\n";
    out << "}\n\n";

    return out.good();
}

boolean PanelCtrlCode::ConstDecls(ostream& out) {
    out << "(const char*, ControlInfo*, ControlState* = nil);\n";
    return out.good();
}

boolean PanelCtrlCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    const char* name, ControlInfo* info, ControlState* state";
    out << "\n) : ";
    out << coreclass << "(name, info, state) {}\n\n";

    return out.good();
}

boolean PanelCtrlCode::EmitIncludeHeaders(ostream& out) {
    GrBlockCode::EmitIncludeHeaders(out);
    PanelCtrlComp* pcComp = GetPanelCtrlComp();
    const char* tool = pcComp->GetToolName()->GetName();
    SubclassNameVar* snamer = pcComp->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("uctrls")) {
        _namelist->Append("uctrls");
        out << "#include <Unidraw/uctrls.h> \n\n";
    }
    if (strcmp(snamer->GetName(), _classname) != 0) {
        if (!_namelist->Search("ctrlinfo")) {
            _namelist->Append("ctrlinfo");
            out << "#include <Unidraw/ctrlinfo.h> \n";
        }
        if (!_namelist->Search("grcomp")) {
            _namelist->Append("grcomp");
            out << "#include <Unidraw/Components/grcomp.h>\n";
        }
        if (!_namelist->Search("grcomptool")) {
            _namelist->Append("grcomptool");
            out << "#include <Unidraw/Tools/grcomptool.h>\n";
        }
        
        if (strcmp(tool, "ConnectTool") == 0) {
            if (!_namelist->Search("connect")) {
                _namelist->Append("connect");
                out << "#include <Unidraw/Tools/connect.h>\n";
            }
        }    
        if (strcmp(tool, "MagnifyTool") == 0) {
            if (!_namelist->Search("magnify")) {
                _namelist->Append("magnify");
                out << "#include <Unidraw/Tools/magnify.h>\n";
            }
        }    
        if (strcmp(tool, "MoveTool") == 0) {
            if (!_namelist->Search("move")) {
                _namelist->Append("move");
                out << "#include <Unidraw/Tools/move.h>\n";
            }
        }    
        if (strcmp(tool, "ReshapeTool") == 0) {
            if (!_namelist->Search("reshape")) {
                _namelist->Append("reshape");
                out << "#include <Unidraw/Tools/reshape.h>\n";
            }
        }    
        if (strcmp(tool, "RotateTool") == 0) {
            if (!_namelist->Search("rotate")) {
                _namelist->Append("rotate");
                out << "#include <Unidraw/Tools/rotate.h>\n";
            }
        }    
        if (strcmp(tool, "ScaleTool") == 0){
            if (!_namelist->Search("scale")) {
                _namelist->Append("scale");
                out << "#include <Unidraw/Tools/scale.h>\n";
            }
        }    
        if (strcmp(tool, "SelectTool") == 0) {
            if (!_namelist->Search("select")) {
                _namelist->Append("select");
                out << "#include <Unidraw/Tools/select.h>\n";
            }
        }    
        if (strcmp(tool, "StretchTool") == 0) {
            if (!_namelist->Search("stretch")) {
                _namelist->Append("stretch");
                out << "#include <Unidraw/Tools/stretch.h>\n";
            }
        }    
    }
    return out.good();
}

/*****************************************************************************/

PanelCtrlGraphic::PanelCtrlGraphic (
    const char* name, Orientation orient, CanvasVar* c, Graphic* g
) : IBViewerGraphic(c, g) {
    _name = strnew(name);
    _orient = orient; 
}

PanelCtrlGraphic::~PanelCtrlGraphic () { delete _name; }

Graphic* PanelCtrlGraphic::Copy () {
    Graphic* copy = new PanelCtrlGraphic(_name, _orient, nil, this);
    return copy;
}

ClassId PanelCtrlGraphic::GetClassId () { return PANELCONTROL_GRAPHIC; }
boolean PanelCtrlGraphic::IsA (ClassId id) {
    return PANELCONTROL_GRAPHIC == id; 
}

void PanelCtrlGraphic::Read (istream& in) {
    Catalog* catalog = unidraw->GetCatalog();
    IBViewerGraphic::Read(in);
    
    delete _name;
    _name = catalog->ReadString(in);
    in >> _orient;
}

void PanelCtrlGraphic::Write (ostream& out) {
    Catalog* catalog = unidraw->GetCatalog();

    IBViewerGraphic::Write(out);
    catalog->WriteString(_name, out);
    out << " " << _orient << " ";
}


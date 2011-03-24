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
 * Implementation of EditorComp
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibeditor.c,v 1.2 91/09/27 14:08:11 tang Exp $
 */

#include "ibclasses.h"
#include "ibbutton.h"
#include "ibcmds.h"
#include "ibdialogs.h"
#include "ibed.h"
#include "ibgraphic.h"
#include "ibeditor.h"
#include "ibpanelctrl.h"
#include "ibvars.h"
#include "ibvarviews.h"

#include <Unidraw/catalog.h>
#include <Unidraw/clipboard.h>
#include <Unidraw/editor.h>
#include <Unidraw/iterator.h>
#include <Unidraw/manips.h>
#include <Unidraw/selection.h>
#include <Unidraw/viewer.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/ulist.h>

#include <stream.h>
#include <string.h>

/*****************************************************************************/

EditorComp::EditorComp (IBGraphic* gr) : MonoSceneClass(gr) { 
    _viewerVar = nil;
    _curCtrlVar = nil;
    _keymap = nil;
    _selection = nil;
    _compname = nil;

    if (gr != nil) {
        SubclassNameVar* subclass = GetClassNameVar();
        subclass->SetName("Editor");
        subclass->SetBaseClass("Editor");
        subclass->SetMachGen(true);
        subclass->GenNewName();
        subclass->SetAbstract(true);
    }
}

EditorComp::~EditorComp () {
    delete _viewerVar;
    delete _curCtrlVar;
    delete _keymap;
    delete _selection;
    delete _compname;
}

ClassId EditorComp::GetClassId () { return EDITOR_COMP; }

boolean EditorComp::IsA (ClassId id) {
    return EDITOR_COMP == id || MonoSceneClass::IsA(id);
}

void EditorComp::Instantiate () {
    MonoSceneClass::Instantiate();
    if (_viewerVar == nil) {
        _viewerVar = new MemberNameVar("", false, false);
    }
    if (_keymap == nil) {
        _keymap = new MemberNameVar("keymap");
        _keymap->GenNewName();
    }
    if (_selection == nil) {
        _selection = new MemberNameVar("selection");
        _selection->GenNewName();
    }
    if (_compname == nil) {
        _compname = new MemberNameVar("grcomp");
        _compname->GenNewName();
    }
    if (_curCtrlVar == nil) {
        _curCtrlVar = new ButtonStateVar("curCtrl");
        _curCtrlVar->GenNewName();
        _curCtrlVar->HideSetting();
    }
}

void EditorComp::Interpret (Command* cmd) {
    if (cmd->IsA(GETCONFLICT_CMD)) {
        GetConflictCmd* gcmd = (GetConflictCmd*) cmd;
        if (gcmd->IsGlobal()) {
            GetExtraConflict(gcmd);
            MonoSceneComp::Interpret(cmd);
        } else if (!gcmd->GetScope()) {
            gcmd->SetScope(true);
            GetExtraConflict(gcmd);
            MonoSceneComp::Interpret(cmd);
        }
        
    } else {
        MonoSceneClass::Interpret(cmd);
    }
}

void EditorComp::GetExtraConflict(GetConflictCmd* gcmd) {
    const char* cname = gcmd->GetCName();
    UList* conflictlist = gcmd->GetConflict();
    const char* curCtrl = _curCtrlVar->GetName();
    const char* key = _keymap->GetName();
    const char* select = _selection->GetName();
    const char* comp = _compname->GetName();
    
    if (strcmp(curCtrl, cname) == 0) {
        conflictlist->Append(new UList(_curCtrlVar->GetButtonSharedName()));
    }
    if (strcmp(key, cname) == 0) {
        conflictlist->Append(new UList(_keymap));
    }
    if (strcmp(select, cname) == 0) {
        conflictlist->Append(new UList(_selection));
    }
    if (strcmp(comp, cname) == 0) {
        conflictlist->Append(new UList(_compname));
    }
}

void EditorComp::SetState(const char* name, StateVar* stateVar) { 
    if (
        strcmp(name, "ViewerVar") == 0 || strcmp(name, "RelatedVar") == 0
    ) {
        MemberNameVar* memberVar = (MemberNameVar*) stateVar;
        *_viewerVar = *memberVar;

    } else if (strcmp(name, "Keymap") == 0) {
        _keymap = (MemberNameVar*) stateVar;

    } else if (strcmp(name, "Selection") == 0) {
        _selection = (MemberNameVar*) stateVar;

    } else if (strcmp(name, "CompName") == 0) {
        _compname = (MemberNameVar*) stateVar;

    } else if (strcmp(name, "CurCtrlVar") == 0) {
        _curCtrlVar = (ButtonStateVar*) stateVar;

    } else {
        MonoSceneClass::SetState(name, stateVar);
    }
}

StateVar* EditorComp::GetState (const char* name) {
    StateVar* stateVar = nil;

    if (strcmp(name, "ViewerVar") == 0) {
        stateVar = _viewerVar;

    } else if (strcmp(name, "Keymap") == 0) {
        stateVar = _keymap;

    } else if (strcmp(name, "Selection") == 0) {
        stateVar = _selection;

    } else if (strcmp(name, "CompName") == 0) {
        stateVar = _compname;

    } else if (strcmp(name, "CurCtrlVar") == 0) {
        stateVar = _curCtrlVar;

    } else {
        stateVar = MonoSceneClass::GetState(name);
    }

    return stateVar;
}

InteractorComp& EditorComp::operator = (InteractorComp& comp) {
    if (comp.IsA(IBVIEWER_COMP)) {
        MemberNameVar* viewerVar = comp.GetMemberNameVar();

        *_viewerVar = *comp.GetMemberNameVar();
        comp.SetState("EditorVar", GetMemberNameVar());

    } else if (comp.IsA(EDITOR_COMP)) {
        EditorComp* edcomp = (EditorComp*) &comp;
        
        MemberNameVar* viewerVar = edcomp->GetViewerVar();
        ButtonStateVar* curCtrlVar = edcomp->GetButtonStateVar();
        MemberNameVar* keymap = edcomp->GetKeyMap();
        MemberNameVar* selection = edcomp->GetSelection();
        MemberNameVar* compname = edcomp->GetCompName();
        
        
        *_viewerVar = *viewerVar;
        *_curCtrlVar = *curCtrlVar;
        *_keymap = *keymap;
        *_selection = *selection;
        *_compname = *compname;

    } else if (comp.IsA(PANELCONTROL_COMP)) {
        ButtonStateVar* curCtrlVar = comp.GetButtonStateVar();
        *_curCtrlVar = *curCtrlVar;
        comp.SetState("EditorVar", GetMemberNameVar());

    }
    return *this;
}

boolean EditorComp::IsRelatableTo (InteractorComp* comp) {
    boolean ok = false;
    if (
        comp->IsA(IBVIEWER_COMP) || comp->IsA(EDITOR_COMP) ||
        comp->IsA(PANELCONTROL_COMP) || comp->IsA(COMMANDCONTROL_COMP)
    ) {
        ok = true;
    }
    return ok;
}

void EditorComp::Read (istream& in) {
    Catalog* catalog = unidraw->GetCatalog();

    delete _curCtrlVar;
    delete _keymap;
    delete _selection;
    delete _compname;
    delete _viewerVar;

    _viewerVar = (MemberNameVar*) catalog->ReadStateVar(in);
    _keymap = (MemberNameVar*) catalog->ReadStateVar(in);
    _selection = (MemberNameVar*) catalog->ReadStateVar(in);
    _compname = (MemberNameVar*) catalog->ReadStateVar(in);
    _curCtrlVar = (ButtonStateVar*) catalog->ReadStateVar(in);

    MonoSceneClass::Read(in);
}

void EditorComp::Write (ostream& out) {
    Catalog* catalog = unidraw->GetCatalog();
    
    catalog->WriteStateVar(_viewerVar, out);
    catalog->WriteStateVar(_keymap, out);
    catalog->WriteStateVar(_selection, out);
    catalog->WriteStateVar(_compname, out);
    catalog->WriteStateVar(_curCtrlVar, out);
    MonoSceneClass::Write(out);
}

/*****************************************************************************/

EditorView::EditorView (EditorComp* subj) : MonoSceneClassView(subj) { }


EditorComp* EditorView::GetEditorComp () {
    return (EditorComp*) GetSubject();
}

ClassId EditorView::GetClassId () { return EDITOR_VIEW; }

boolean EditorView::IsA (ClassId id) {
    return EDITOR_VIEW == id || MonoSceneClassView::IsA(id);
}

InfoDialog* EditorView::GetInfoDialog () {
    IBEditor* ibed = (IBEditor*) GetViewer()->GetEditor();
    InfoDialog* info = MonoSceneClassView::GetInfoDialog();
    ButtonState* state = info->GetState();
    EditorComp* edcomp = GetEditorComp();

    MemberNameVar* viewerVar = edcomp->GetViewerVar();
    ButtonStateVar* curCtrlVar = edcomp->GetButtonStateVar();
    MemberNameVar* keymap = edcomp->GetKeyMap();
    MemberNameVar* selection = edcomp->GetSelection();
    MemberNameVar* compname = edcomp->GetCompName();

    info->Include(new RelatedVarView(
        viewerVar, state, edcomp, "Viewer Name: ")
    );
    info->Include(new MemberNameVarView(
        keymap, state, edcomp, "KeyMap: ")
    );
    info->Include(new MemberNameVarView(
        selection, state, edcomp, "Selection: ")
    );
    info->Include(new MemberNameVarView(
        compname, state, edcomp, "GraphicComp: ")
    );
    info->Include(new CtrlStateVarView(curCtrlVar, state, edcomp, ibed));
    return info;
}

/*****************************************************************************/

EditorCode::EditorCode (EditorComp* subj) : MonoSceneClassCode(subj) { }

EditorComp* EditorCode::GetEditorComp() {
    return (EditorComp*) GetSubject();
}

ClassId EditorCode::GetClassId () { return EDITOR_CODE; }

boolean EditorCode::IsA(ClassId id) {
    return EDITOR_CODE == id || MonoSceneClassCode::IsA(id);
}

boolean EditorCode::Definition (ostream& out) {
    char coreclass[CHARBUFSIZE];
    boolean ok = true;

    EditorComp* edcomp = GetEditorComp();
    SubclassNameVar* snamer = edcomp->GetClassNameVar();
    MemberNameVar* mnamer = edcomp->GetMemberNameVar();

    const char* subclass = snamer->GetName();
    const char* baseclass = snamer->GetBaseClass();
    const char* mname = mnamer->GetName();

    MemberNameVar* viewerVar = edcomp->GetViewerVar();
    const char* vname = viewerVar->GetName();

    GetCoreClassName(coreclass);
    CodeView* kidview = GetKidView();
    MemberNameVar* kidname = kidview->GetIntComp()->GetMemberNameVar();
    InteractorComp* dummy;

    if (*vname == '\0') {
        strcat(_errbuf, mname);
        strcat(_errbuf, " has undefined Viewer.\n");
        return false;

    } else if (!Search(viewerVar, dummy)) {
        strcat(_errbuf, mname);
        strcat(
            _errbuf, "'s Viewer is not in the same hierachy.\n"
        );
        return false;
    }
    if (_emitForward) {
        if (strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("GraphicComp")) {
                _namelist->Append("GraphicComp");
                out << "class GraphicComp;\n";
            }
            if (!_namelist->Search("ControlState")) {
                _namelist->Append("ControlState");
                out << "class ControlState;\n";
            }
        }
        ok = ok && MonoSceneClassCode::Definition(out);

    } else if (_emitBSDecls || _emitBSInits) {
        if (strcmp(subclass, _classname) == 0) {
            ButtonStateVar* ctrlvar = edcomp->GetButtonStateVar();
            const char* ctrl = ctrlvar->GetName();
            if (!_bsdeclslist->Search(ctrl)) {
                _bsdeclslist->Append(ctrl);
            }
            if (!_bsinitslist->Search(ctrl)) {
                _bsinitslist->Append(ctrl);
            }
            ok = ok && Iterate(out);
        }

    } else if (_emitExpHeader) {
        if (strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("editor")) {
                _namelist->Append("editor");
                out << "#include <Unidraw/editor.h>\n";
                out << "#include <Unidraw/keymap.h>\n";
                out << "#include <Unidraw/Components/grcomp.h>\n";
                out << "#include <Unidraw/uctrl.h>\n";
                out << "#include <Unidraw/selection.h>\n";
                out << "#include <Unidraw/Tools/tool.h>\n";
            }
            if (!_namelist->Search("control")) {
                _namelist->Append("control");
                out << "#include <InterViews/control.h>\n";
            }
        }
        ok = ok && CodeView::Definition(out);

    } else if (_emitCorehHeader) {
        if (strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("editor")) {
                _namelist->Append("editor");
                out << "#include <Unidraw/editor.h>\n";
            }
        } else {
            ok = ok && kidview->Definition(out);
        }

    } else {
        ok = ok && MonoSceneClassCode::Definition(out);
    }
    return ok && out.good();
}

boolean EditorCode::CoreConstDecls(ostream& out) { 
    boolean ok = true;
    EditorComp* edcomp = GetEditorComp();

    MemberNameVar* compname = edcomp->GetCompName();
    MemberNameVar* selection = edcomp->GetSelection();
    MemberNameVar* keymap = edcomp->GetKeyMap();
    MemberNameVar* vnamer = edcomp->GetViewerVar();

    ButtonStateVar* ctrlvar = edcomp->GetButtonStateVar();

    const char* ctrl = ctrlvar->GetName();
    const char* comp = compname->GetName();
    const char* select = selection->GetName();
    const char* key = keymap->GetName();
    const char* vname = vnamer->GetName();

    CodeView* kidview = GetKidView();

    out << "(const char*);\n";
    out << "    virtual Component* GetComponent();\n";
    out << "    virtual Viewer* GetViewer(int = 0);\n";
    out << "    virtual KeyMap* GetKeyMap();\n";
    out << "    virtual Tool* GetCurTool();\n";
    out << "    virtual Selection* GetSelection();\n\n";

    out << "    virtual void SetComponent(Component*);\n";
    out << "    virtual void SetViewer(Viewer*, int = 0);\n";
    out << "    virtual void SetKeyMap(KeyMap*);\n";
    out << "    virtual void SetSelection(Selection*);\n\n";

    ok = ok && EmitFunctionDecls(kidview, out);
    out << "protected:\n";
    out << "    Interactor* Interior();\n";
    out << "protected:\n";
    if (compname->GetExport()) {
        out << "    GraphicComp* " << comp << ";\n";
    }
    if (selection->GetExport()) {
        out << "    Selection* " << select << ";\n";
    }
    if (keymap->GetExport()) {
        out << "    KeyMap* " << key << ";\n";
    }
    if (ctrlvar->GetExport()) {
        out << "    ControlState* " << ctrl << ";\n";
    }
    _emitExport = true;
    ok = ok && EmitBSDecls(this, out);
    ok = ok && EmitInstanceDecls(kidview, out);
    _emitExport = false;
    out << "private:\n";
    if (!compname->GetExport()) {
        out << "    GraphicComp* " << comp << ";\n";
    }
    if (!selection->GetExport()) {
        out << "    Selection* " << select << ";\n";
    }
    if (!keymap->GetExport()) {
        out << "    KeyMap* " << key << ";\n";
    }
    if (!ctrlvar->GetExport()) {
        out << "    ControlState* " << ctrl << ";\n";
    }
    if (!vnamer->GetExport()) {
        out << "    Viewer* " << vname << ";\n";
    }
    
    return out.good();
}

boolean EditorCode::CoreConstInits(ostream& out) {
    boolean ok = true;
    EditorComp* edcomp = GetEditorComp();
    SubclassNameVar* snamer = edcomp->GetClassNameVar();
    MemberNameVar* mnamer = edcomp->GetMemberNameVar();
    MemberNameVar* compname = edcomp->GetCompName();
    MemberNameVar* selection = edcomp->GetSelection();
    MemberNameVar* keymap = edcomp->GetKeyMap();
    ButtonStateVar* ctrlvar = edcomp->GetButtonStateVar();

    const char* subclass = snamer->GetName();
    const char* baseclass = snamer->GetBaseClass();
    const char* mname = mnamer->GetName();
    const char* ctrl = ctrlvar->GetName();
    const char* comp = compname->GetName();
    const char* select = selection->GetName();
    const char* key = keymap->GetName();

    MemberNameVar* viewerVar = edcomp->GetViewerVar();
    const char* vname = viewerVar->GetName();

    CodeView* kidview = GetKidView();
    MemberNameVar* kidname = kidview->GetIntComp()->GetMemberNameVar();
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(const char* name) {\n";
    out << "    ManagedWindow* window = new ApplicationWindow(this);\n";
    out << "    window->name(name);\n";
    out << "    window->icon_name(name);\n";
    out << "    SetWindow(window);\n";
    out << "    SetInstance(name);\n";
    out << "    perspective = new Perspective;\n";
    out << "    " << comp << " = new GraphicComps;\n";
    out << "    " << ctrl << " = new ControlState;\n";
    out << "    " << select << " = new Selection;\n";
    out << "    " << key << " = new KeyMap;\n";
    out << "    Insert(Interior());\n";
    out << "    Propagate(false);\n";
    out << "    " << key << "->Execute(CODE_SELECT);\n";
    out << "}\n\n";
    
    out << "Interactor* " << coreclass;
    out << "::Interior() {\n";
    ok = ok && EmitBSInits(this, out);
    ok = ok && EmitInstanceInits(kidview, out);
    out << "    return " << kidname->GetName() << ";\n};\n\n";

    out << "Component* " << subclass;
    out << "_core::GetComponent () { return " << comp << "; }\n";
    out << "Viewer* " << subclass;
    out << "_core::GetViewer (int id) { return (id == 0) ? ";
    out << vname << ": nil; }\n";
    out << "Selection* " << subclass;
    out << "_core::GetSelection () { return " << select << "; }\n";
    out << "KeyMap* " << subclass;
    out << "_core::GetKeyMap () { return " << key << "; }\n\n";
    out << "Tool* " << subclass <<  "_core::GetCurTool () {\n";
    out << "    Tool* tool = nil;\n";
    out << "    UControl* c = (UControl*) " << ctrl << "->Selection();\n";
    out << "    if (c != nil) {\n";
    out << "        tool = (Tool*) c->GetControlInfo()->GetOwner();\n";
    out << "    }\n";
    out << "    return tool;\n";
    out << "}\n\n";

    out << "void " << subclass << "_core::SetComponent(Component* comp) {\n";
    out << "    if (comp == nil || comp->IsA(GRAPHIC_COMP)) {\n";
    out << "        " << comp << " = (GraphicComp*) comp;\n";
    out << "    }\n";
    out << "}\n\n";
    out << "void " << subclass << "_core::SetViewer(Viewer* vname, int) {\n";
    out << "    " << vname << " = vname;\n";
    out << "}\n\n";
    out << "void " << subclass << "_core::SetKeyMap(KeyMap* key) {\n";
    out << "    " << key << " = key;\n";
    out << "}\n\n";
    out << "void " << subclass << "_core::SetSelection(Selection* select) {\n";
    out << "    " << select << " = select;\n";
    out << "}\n\n\n";
    

    ok = ok && EmitFunctionInits(kidview, out);

    return out.good();
}

boolean EditorCode::EmitIncludeHeaders(ostream& out) {
    if (!_namelist->Search("editor")) {
        _namelist->Append("editor");
        out << "#include <Unidraw/classes.h>\n";
        out << "#include <Unidraw/creator.h>\n";
        out << "#include <Unidraw/globals.h> \n";
        out << "#include <Unidraw/grid.h> \n";
        out << "#include <Unidraw/keymap.h>\n";
        out << "#include <Unidraw/kybd.h>\n";
        out << "#include <Unidraw/page.h>\n";
        out << "#include <Unidraw/selection.h>\n";
        out << "#include <Unidraw/uctrl.h>\n";
        out << "#include <Unidraw/unidraw.h>\n";
        out << "#include <Unidraw/Components/grview.h> \n";
        out << "#include <Unidraw/Tools/tool.h>\n";
    }
    if (!_namelist->Search("ctrlinfo")) {
        _namelist->Append("ctrlinfo");
        out << "#include <Unidraw/ctrlinfo.h> \n";
    }
    if (!_namelist->Search("grcomp")) {
        _namelist->Append("grcomp");
        out << "#include <Unidraw/Components/grcomp.h>\n";
    }
    if (!_namelist->Search("control")) {
        _namelist->Append("control");
        out << "#include <InterViews/control.h>\n";
    }
    if (!_namelist->Search("window")) {
        _namelist->Append("window");
        out << "#include <InterViews/window.h>\n";
    }
    return out.good();
}




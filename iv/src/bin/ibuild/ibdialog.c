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

#include "ibclasses.h"
#include "ibcmds.h"
#include "ibdialog.h"
#include "ibdialogs.h"
#include "ibvars.h"
#include "ibvarviews.h"

#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/viewer.h>
#include <Unidraw/catalog.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/

DialogClass::DialogClass (IBGraphic* gr) : MonoSceneClass(gr) {
    if (gr != nil) {
        GetClassNameVar()->SetName("Dialog");
        GetClassNameVar()->SetBaseClass("Dialog");
        GetClassNameVar()->GenNewName();
    }
    _bsVar = nil;
}

DialogClass::~DialogClass () {
    delete _bsVar;
}

void DialogClass::SetState (const char* name, StateVar* stateVar) {
    if (strcmp(name, "ButtonStateVar") == 0) {
        ButtonStateVar* bsVar = (ButtonStateVar*) stateVar;
        *_bsVar = *bsVar;

    } else {
        MonoSceneClass::SetState(name, stateVar);
    }
}

StateVar* DialogClass::GetState (const char* name) {
    StateVar* stateVar = nil;

    if (strcmp(name, "ButtonStateVar") == 0) {
        stateVar = _bsVar;
    } else {
        stateVar = MonoSceneClass::GetState(name);
    }
    return stateVar;
}

InteractorComp& DialogClass::operator = (InteractorComp& comp) {
    StateVar* state = comp.GetButtonStateVar();
    if (state != nil) {
	ButtonStateVar* bsvar = (ButtonStateVar*) state;
	if (_bsVar != nil) {
	    *_bsVar = *bsvar;
	}
    }
    return *this;
}

boolean DialogClass::IsRelatableTo (InteractorComp* comp) {
    boolean ok = false;
    if (comp->GetButtonStateVar() != nil) {
        ok = true;
    }
    return ok;
}

void DialogClass::Relate (InteractorComp* comp) {
    *this = *comp;
    int setting = _bsVar->GetSetting();
    _bsVar->SetSetting(++setting);
}

void DialogClass::Instantiate () {
    MonoSceneClass::Instantiate();
    if (_bsVar == nil) {
        _bsVar = new ButtonStateVar();
	_bsVar->HideSetting();
        _bsVar->GenNewName();
    }
}

void DialogClass::Interpret(Command* cmd) {
    if (cmd->IsA(GETCONFLICT_CMD)) {
        GetConflictCmd* gcmd = (GetConflictCmd*) cmd;
        const char* cname = gcmd->GetCName();
        MonoSceneClass::Interpret(cmd);
        UList* conflictlist = gcmd->GetConflict();
        ButtonSharedName* bsnamer = _bsVar->GetButtonSharedName();
        const char* buttonname = bsnamer->GetName();
        const char* funcname = bsnamer->GetFuncName();
        if (strcmp(buttonname, cname) == 0 || strcmp(funcname, cname) == 0) {
            conflictlist->Append(new UList(bsnamer));
        }

    } else {
        MonoSceneClass::Interpret(cmd);
    }
}

void DialogClass::Read (istream& in) {
    delete _bsVar;
    _bsVar = (ButtonStateVar*) unidraw->GetCatalog()->ReadStateVar(in);
    MonoSceneClass::Read(in);
}

void DialogClass::Write (ostream& out) {
    unidraw->GetCatalog()->WriteStateVar(_bsVar, out);
    MonoSceneClass::Write(out);
}

ClassId DialogClass::GetClassId () { return DIALOG_CLASS; }

boolean DialogClass::IsA (ClassId id) {
    return DIALOG_CLASS == id || MonoSceneClass::IsA(id);
}

/*****************************************************************************/

DialogClassView::DialogClassView (
    DialogClass* subj
) : MonoSceneClassView(subj) { }


DialogClass* DialogClassView::GetDialogClass () {
    return (DialogClass*) GetSubject();
}

ClassId DialogClassView::GetClassId () { return MONOSCENECLASS_VIEW; }

boolean DialogClassView::IsA (ClassId id) {
    return MONOSCENECLASS_VIEW == id || MonoSceneClassView::IsA(id);
}

InfoDialog* DialogClassView::GetInfoDialog () {
    IBEditor* ibed = (IBEditor*) GetViewer()->GetEditor();
    InfoDialog* info = MonoSceneClassView::GetInfoDialog();
    ButtonState* state = info->GetState();

    DialogClass* dclass = GetDialogClass();
    ButtonStateVar* bsVar = dclass->GetButtonStateVar();

    info->Include(new ButtonStateVarView(bsVar, state, dclass, ibed));
    return info;
}

/*****************************************************************************/

DialogClassCode::DialogClassCode (
    DialogClass* subj
) : MonoSceneClassCode(subj) { }

DialogClass* DialogClassCode::GetDialogClass(){
    return (DialogClass*) GetSubject();
}

ClassId DialogClassCode::GetClassId () { return DIALOGCLASS_CODE; }

boolean DialogClassCode::IsA(ClassId id) {
    return DIALOGCLASS_CODE == id || MonoSceneClassCode::IsA(id);
}

boolean DialogClassCode::Definition (ostream& out) {
    char coreclass[CHARBUFSIZE];
    boolean ok = true;

    DialogClass* dclass = GetDialogClass();
    SubclassNameVar* snamer = dclass->GetClassNameVar();
    MemberNameVar* iname = dclass->GetMemberNameVar();
    const char* subclass = snamer->GetName();
    const char* baseclass = snamer->GetBaseClass();
    const char* instance = iname->GetName();

    GetCoreClassName(coreclass);
    ButtonStateVar* bsVar = dclass->GetButtonStateVar();

    MemberNameVar* kidname;
    CodeView* kidview = GetKidView();
    if (kidview != nil) {
        kidname = kidview->GetIntComp()->GetMemberNameVar();
    }

    if (
        _emitInstanceInits || _emitClassHeaders || _emitHeaders ||
        _emitProperty || _emitInstanceDecls || _emitExpHeader
    ) {
        ok = ok && MonoSceneClassCode::Definition(out);

    } else if (
	_emitFunctionDecls || _emitFunctionInits || 
	_emitBSDecls || _emitBSInits
    ) {
        if (strcmp(subclass, _classname) == 0) {
            ok = ok && CodeView::Definition(out);
            ok = ok && kidview->Definition(out);
	}

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("dialog")) {
                _namelist->Append("dialog");
                out << "#include <InterViews/dialog.h>\n";
            }
        } else {
            if (kidview != nil) {
                ok = ok && kidview->Definition(out); 
            }
        }

    } else if (_emitForward) {
        if (_scope) {
            ok = ok && CodeView::Definition(out);
            if (
                bsVar->GetExport() &&
                !_bsdeclslist->Search("ButtonState")
            ) {
                _bsdeclslist->Append("ButtonState");
                out << "class ButtonState;\n";
            }
        } else {
            if (strcmp(subclass, _classname) == 0) {
                _scope = true;
                if (kidview != nil) {
                    ok = ok && kidview->Definition(out); 
                }
                _scope = false;
            } else {
                if (kidview != nil) {
                    ok = ok && kidview->Definition(out); 
                }
            }
        }

    } else if (
        _emitCoreDecls || _emitCoreInits || _emitClassDecls || _emitClassInits
    ) {
        ok = ok && MonoSceneClassCode::Definition(out);

    }
    return ok && out.good();
}

boolean DialogClassCode::CoreConstDecls(ostream& out) { 
    boolean ok = true;
    CodeView* kidview = GetKidView();

    out << "(const char*);\n";
    ok = ok && EmitFunctionDecls(this, out);
    out << "protected:\n";
    out << "    Interactor* Interior();\n";
    out << "protected:\n";
    
    _emitExport = true;
    ok = ok && EmitBSDecls(this, out);
    if (kidview != nil) {
        ok = ok && EmitInstanceDecls(kidview, out);
    }
    _emitExport = false;
    
    return out.good();
}

boolean DialogClassCode::CoreConstInits(ostream& out) {
    boolean ok = true;
    DialogClass* dclass = GetDialogClass();
    ButtonStateVar* bsVar = dclass->GetButtonStateVar();
    SubclassNameVar* snamer = dclass->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();
    
    char ButtonClass[CHARBUFSIZE];
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);
    _bsinitslist->Append(bsVar->GetName());
    
    strcpy(ButtonClass, coreclass);
    strcat(ButtonClass, "_Button");
    const char* proc = bsVar->GetFuncName();
    
    boolean export = bsVar->GetExport();
    CodeView* kidview = GetKidView();
    MemberNameVar* kidname;
    if (kidview != nil) {
        kidname = kidview->GetIntComp()->GetMemberNameVar();
    }

    out << " (const char* name) : " << baseclass << "(\n";
    out << "    name, nil, nil\n) {\n";
    out << "    perspective = new Perspective;\n";
    out << "    if (input != nil) {\n";
    out << "        input->Unreference();\n";
    out << "    }\n";
    out << "    input = allEvents;\n";
    out << "    input->Reference();\n";

    out << "    state = new " << ButtonClass << "(";
    out << bsVar->GetInitial() << ", this";
    if (proc != nil && *proc != '\0') {
        out << ", " << "&" << coreclass << "::" << proc << ");\n";
    } else {
        out << ", nil);\n";
    }
    out << "    Insert(Interior());\n}\n\n";

    out << "Interactor*" << coreclass;
    out << "::Interior() {\n";
    if (export) {
        out << "    " << bsVar->GetName() << " = state;\n";
    } else {
        out << "    ButtonState* ";
        out << bsVar->GetName() << " = state;\n";
    }
    ok = ok && EmitBSInits(this, out);
    if (kidview != nil) {
        ok = ok && EmitInstanceInits(kidview, out);
        out << "    return " << kidname->GetName() << ";\n};\n\n";
        ok = ok && EmitFunctionInits(kidview, out);
    }

    return out.good();
}

boolean DialogClassCode::ConstDecls(ostream& out) {
    boolean ok = true;
    out << "(const char*);\n\n";
    ok = ok && EmitFunctionDecls(this, out);
    
    return out.good();
}

boolean DialogClassCode::ConstInits(ostream& out) {
    boolean ok = true;
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(const char* name) : " << coreclass << "(name) {}\n\n";
    ok = ok && EmitFunctionInits(this, out);
            
    return out.good();
}




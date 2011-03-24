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
 * StrEdit component definitions.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibstred.c,v 1.2 91/09/27 14:12:02 tang Exp $
 */

#include "ibclasses.h"
#include "ibstred.h"
#include "ibvars.h"

#include <Unidraw/ulist.h>
#include <Unidraw/viewer.h>
#include <Unidraw/Tools/tool.h>
#include <InterViews/shape.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/

static void ReverseColors (Graphic* g) {
    PSColor* fg = g->GetFgColor();
    PSColor* bg = g->GetBgColor();
    g->SetColors(bg, fg);
}

/*****************************************************************************/

StrEditComp::StrEditComp (MessageGraphic* g) : ButtonComp(g) { }
ClassId StrEditComp::GetClassId () { return STREDIT_COMP; }

boolean StrEditComp::IsA (ClassId id) {
    return STREDIT_COMP == id || ButtonComp::IsA(id);
}

StrEditGraphic* StrEditComp::GetStrEditGraphic () {
    return (StrEditGraphic*) GetGraphic();
}

void StrEditComp::Reconfig () {
    Shape* shape = GetShapeVar()->GetShape();
    int w, h;
    GetMessageGraphic()->Natural(w, h);
    shape->Rect(w, h);
    shape->Rigid(hfil, hfil);
    GetShapeVar()->Notify();
}

void StrEditComp::Relate (InteractorComp* comp) {
    InteractorComp::Relate(comp);
}

void StrEditComp::Instantiate() {
    if (_instanceNameVar == nil) {
        ButtonComp::Instantiate();
        GetMemberNameVar()->SetExport(true);
        GetButtonStateVar()->HideSetting();
    } else {
        ButtonComp::Instantiate();
    }
}

/*****************************************************************************/

StrEditView::StrEditView (StrEditComp* subj) : ButtonView(subj) { }
StrEditComp* StrEditView::GetStrEditComp(){return (StrEditComp*) GetSubject();}
ClassId StrEditView::GetClassId () { return STREDIT_VIEW; }

boolean StrEditView::IsA (ClassId id) {
    return STREDIT_VIEW == id || ButtonView::IsA(id);
}

Manipulator* StrEditView::CreateManipulator (
    Viewer* v, Event& e, Transformer* rel, Tool* tool
) {
    if (tool->IsA(RESHAPE_TOOL)) {
        Graphic* g = GetGraphic();
        ReverseColors(g);
        IncurDamage(g);
        GetViewer()->Update();
        ReverseColors(g);
    }
    return MessageView::CreateManipulator(v, e, rel, tool);
}

Command* StrEditView::InterpretManipulator (Manipulator* m) {
    return MessageView::InterpretManipulator(m);
}

/*****************************************************************************/

StrEditCode::StrEditCode (StrEditComp* subj) : CodeView(subj) { }
StrEditComp* StrEditCode::GetStrEditComp(){return (StrEditComp*) GetSubject();}
ClassId StrEditCode::GetClassId () { return STREDIT_CODE; }

boolean StrEditCode::IsA(ClassId id) {
    return STREDIT_CODE == id || CodeView::IsA(id);
}

boolean StrEditCode::Definition (ostream& out) {
    boolean ok = true;
    if (
	_emitProperty || _emitInstanceDecls ||
	_emitBSDecls || _emitBSInits || _emitHeaders ||
	_emitFunctionDecls || _emitFunctionInits || _emitClassHeaders
    ) {
        return CodeView::Definition(out);

    } else if (_emitForward) {
        if (_scope) {
            ok = ok && CodeView::Definition(out);
            ButtonStateVar* bsVar = GetStrEditComp()->GetButtonStateVar();
            if (
                bsVar->GetExport() &&
                !_bsdeclslist->Search("ButtonState")
            ) {
                _bsdeclslist->Append("ButtonState");
                out << "class ButtonState;\n";
            }
        }
    } else if (_emitExpHeader) {
	InteractorComp* icomp = GetIntComp();
	MemberNameVar* mnamer = icomp->GetMemberNameVar();
        SubclassNameVar* snamer = icomp->GetClassNameVar();
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("streditor")
            ) {
                _namelist->Append("streditor");
                out << "#include <InterViews/streditor.h>\n";
            }
            ButtonStateVar* bvar = icomp->GetButtonStateVar();
            if (_scope && bvar->GetExport() && !_namelist->Search("button")) {
                _namelist->Append("button");
                out << "#include <InterViews/button.h>\n";
            }
        } else {
            ok = ok && CodeView::Definition(out);
        }
    } else if (_emitCorehHeader) {
	InteractorComp* icomp = GetIntComp();
        SubclassNameVar* snamer = icomp->GetClassNameVar();
        const char* subclass = snamer->GetName();
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("streditor")) {
                _namelist->Append("streditor");
                out << "#include <InterViews/streditor.h>\n";
            }
        }
    } else if (_emitInstanceInits) {
        InteractorComp* icomp = GetIntComp();
        const char* mname = icomp->GetMemberNameVar()->GetName();

        if (!_instancelist->Find((void*) mname)) {
            _instancelist->Append(new UList((void*)mname));
            StrEditComp* stred = GetStrEditComp();

            BeginInstantiate(out);
            const char* text = stred->GetStrEditGraphic()->GetText();
            ButtonStateVar* bsVar = stred->GetButtonStateVar();

            out << "(";
            InstanceName(out);
            out << bsVar->GetName() << ", \"" << text << "\")";
            EndInstantiate(out);
            out << "    " << mname << "->Message(\"\");\n";
	}

    } else if (
        _emitCoreDecls || _emitCoreInits || _emitClassDecls || _emitClassInits
    ) {
	ok = ok && CodeView::Definition(out);
        
    } else if (_emitMain) {
	ok = ok && CodeView::Definition(out);
        
    }
    return out.good() && ok;
}

boolean StrEditCode::CoreConstDecls(ostream& out) { 
    out << "(const char*, ButtonState*, const char*);\n";
    return out.good();
}

boolean StrEditCode::CoreConstInits(ostream& out) {
    InteractorComp* icomp = GetIntComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    const char* name, ButtonState* bs, const char* sample\n)";
    out << " : " << baseclass;
    out << "(name, bs, sample) {\n";
    out << "    perspective = new Perspective;\n";
    out << "}\n\n";
    return out.good();
}

boolean StrEditCode::ConstDecls(ostream& out) {
    out << "(const char*, ButtonState*, const char*);\n";
    return out.good();
}

boolean StrEditCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    const char* name, ButtonState* bs, const char* sample\n)";
    out << " : " << coreclass;
    out << "(name, bs, sample) {}\n\n";
    return out.good();
}

boolean StrEditCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIntComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("streditor")) {
        _namelist->Append("streditor");
        out << "#include <InterViews/streditor.h> \n";
    }
    if (!_namelist->Search("button")) {
        _namelist->Append("button");
        out << "#include <InterViews/button.h> \n";
    }
    return out.good();
}

/*****************************************************************************/

StrEditGraphic::StrEditGraphic (
    const char* text, CanvasVar* c, Graphic* g
) : MessageGraphic(text, c, g, Left) { }

Graphic* StrEditGraphic::Copy(){return new StrEditGraphic(GetText(),nil,this);}
const char* StrEditGraphic::GetClassName () { return "StringEditor"; }
ClassId StrEditGraphic::GetClassId () { return STREDIT_GRAPHIC; }

void StrEditGraphic::draw (Canvas* c, Graphic* gs) {
    ReverseColors(gs);
    MessageGraphic::draw(c, gs);
}

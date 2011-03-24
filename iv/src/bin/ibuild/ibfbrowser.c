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
 * FBrowser component definitions.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibfbrowser.c,v 1.2 91/09/27 14:10:24 tang Exp $
 */

#include "ibclasses.h"
#include "ibcmds.h"
#include "ibdialogs.h"
#include "ibed.h"
#include "ibgraphic.h"
#include "ibfbrowser.h"
#include "ibvars.h"
#include "ibvarviews.h"

#include <Unidraw/catalog.h>
#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/viewer.h>

#include <stream.h>
#include <string.h>

/*****************************************************************************/

FBrowserComp::FBrowserComp (StrBrowserGraphic* g) : StrBrowserComp(g) {
    _fbVar = nil;
}

FBrowserComp::~FBrowserComp () {
    delete _fbVar;
}

ClassId FBrowserComp::GetClassId () { return FBROWSER_COMP; }

boolean FBrowserComp::IsA (ClassId id) {
    return FBROWSER_COMP == id || StrBrowserComp::IsA(id);
}

void FBrowserComp::Instantiate () {
    StrBrowserComp::Instantiate();
    if (_fbVar == nil) {
        _fbVar = new FBrowserVar("./", "");
    }
}
   
StateVar* FBrowserComp::GetState (const char* name) {
    StateVar* stateVar = nil;

    if (strcmp(name, "FBrowserVar") == 0) {
        stateVar = _fbVar;
    } else {
        stateVar = StrBrowserComp::GetState(name);
    }

    return stateVar;
}

void FBrowserComp::SetState (const char* name, StateVar* stateVar) {
    if (strcmp(name, "FBrowserVar") == 0) {
        _fbVar = (FBrowserVar*) stateVar;
    } else {
        StrBrowserComp::SetState(name, stateVar);
    }

}

void FBrowserComp::Read (istream& in) {
    StrBrowserComp::Read(in);
    delete _fbVar;
    _fbVar = (FBrowserVar*) unidraw->GetCatalog()->ReadStateVar(in);
}

void FBrowserComp::Write (ostream& out) {
    StrBrowserComp::Write(out);
    unidraw->GetCatalog()->WriteStateVar(_fbVar, out);
}

/*****************************************************************************/

FBrowserComp* FBrowserView::GetFBrowserComp() {
    return (FBrowserComp*) GetSubject();
}

FBrowserView::FBrowserView (FBrowserComp* subj) : StrBrowserView(subj) { }
ClassId FBrowserView::GetClassId () { return FBROWSER_VIEW; }

boolean FBrowserView::IsA (ClassId id) {
    return FBROWSER_VIEW == id || StrBrowserView::IsA(id);
}

InfoDialog* FBrowserView::GetInfoDialog () {
    IBEditor* ibed = (IBEditor*) GetViewer()->GetEditor();
    InfoDialog* info = StrBrowserView::GetInfoDialog();
    ButtonState* state = info->GetState();
    FBrowserComp* fcomp = GetFBrowserComp();
    FBrowserVar* fbVar = fcomp->GetFBrowserVar();

    info->Include(new FBrowserVarView(fbVar, state));
    return info;
}

/*****************************************************************************/

boolean FBrowserCode::IsA (ClassId id) {
    return FBROWSER_CODE == id || CodeView::IsA(id);
}

ClassId FBrowserCode::GetClassId () { return FBROWSER_CODE; }
FBrowserCode::FBrowserCode (FBrowserComp* subj) : CodeView(subj) { }

FBrowserComp* FBrowserCode::GetFBrowserComp () {
    return (FBrowserComp*) GetSubject();
}

boolean FBrowserCode::Definition (ostream& out) {
    boolean ok = true;
    if (
	_emitProperty || _emitBSDecls ||
	_emitBSInits || _emitInstanceDecls || _emitHeaders ||
	_emitFunctionDecls || _emitFunctionInits || _emitClassHeaders
    ) {
        return CodeView::Definition(out);

    } else if (_emitForward) {
        if (_scope) {
            ok = ok && CodeView::Definition(out);
            ButtonStateVar* bsVar = GetFBrowserComp()->GetButtonStateVar();
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
                !_namelist->Search("filebrowser")
            ) {
                _namelist->Append("filebrowser");
                out << "#include <InterViews/filebrowser.h>\n";
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
            if (!_namelist->Search("filebrowser")) {
                _namelist->Append("filebrowser");
                out << "#include <InterViews/filebrowser.h>\n";
            }
        }
    } else if (_emitInstanceInits) {
        InteractorComp* icomp = GetIntComp();
        const char* mname = icomp->GetMemberNameVar()->GetName();

        if (!_instancelist->Find((void*) mname)) {
            _instancelist->Append(new UList((void*)mname));

            FBrowserComp* fb = GetFBrowserComp();
            int rows, cols;
            char dirName[CHARBUFSIZE];
    
            const char* dirname = fb->GetFBrowserVar()->GetDirName();
            int uniqueSel = (int) fb->GetUniqueSel()->GetBooleanState();
            if (*dirname != '\0') {
                strcpy(dirName, dirname);
            } else {
                strcpy(dirName, ".");
            }

            BeginInstantiate(out);
            StrBrowserGraphic* graphic = fb->GetStrBrowserGraphic();
            graphic->GetRowsCols(rows, cols);
            ButtonStateVar* bsVar = fb->GetButtonStateVar();

            out << "(";
            InstanceName(out);
            out << bsVar->GetName() << ", ";
            out << "\"" << dirname << "\", ";
            out << rows << ", " << cols << ", ";
            if (uniqueSel) {
                out << "true";
            } else {
                out << "false";
            }
            out << ")";
            EndInstantiate(out);
            
            const char* textfilter = fb->GetFBrowserVar()->GetTextFilter();
            if (*textfilter != '\0') {
                out << "    " << mname << "->SetTextFilter(";
                out << "\"" << textfilter << "\"" << ");\n";
            }
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

boolean FBrowserCode::CoreConstDecls(ostream& out) { 
    out << "(const char*, ButtonState*, const char*, int, int, boolean);\n";
    return out.good();
}

boolean FBrowserCode::CoreConstInits(ostream& out) {
    InteractorComp* icomp = GetIntComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    const char* name, ButtonState* bs,";
    out << "const char* dir, int r, int c, boolean u\n) : " << baseclass;
    out << "(name, bs, dir, r, c, u) {}\n\n";
    return out.good();
}

boolean FBrowserCode::ConstDecls(ostream& out) {
    out << "(const char*, ButtonState*, const char*, int, int, boolean);\n";
    return out.good();
}

boolean FBrowserCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    const char* name, ButtonState* bs,";
    out << "const char* dir, int r, int c, boolean u\n) : " << coreclass;
    out << "(name, bs, dir, r, c, u) {}\n\n";
    return out.good();
}

boolean FBrowserCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIntComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("filebrowser")) {
        _namelist->Append("filebrowser");
        out << "#include <InterViews/filebrowser.h> \n";
    }
    if (!_namelist->Search("button")) {
        _namelist->Append("button");
        out << "#include <InterViews/button.h> \n";
    }
    return out.good();
}



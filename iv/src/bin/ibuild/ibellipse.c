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
 * IEllipse component definitions.
 */


#include "ibellipse.h"
#include "ibclasses.h"
#include "ibvars.h"
#include <Unidraw/Graphic/ellipses.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/
static const char* ellcomp_delim = "%ellcomp_delim";
/*****************************************************************************/

IEllipseComp::IEllipseComp (SF_Ellipse* graphic) {
    GetClassNameVar()->SetName("SF_Ellipse");
    GetClassNameVar()->SetBaseClass("SF_Ellipse");
    if (!_release || graphic != nil) {
        _target = new EllipseComp(graphic);
        if (graphic != nil) {
            Append(_target);
        }
    }
}

ClassId IEllipseComp::GetSubstId(const char*& delim) {
    delim = ellcomp_delim;
    return ELLIPSE_COMP;
}

ClassId IEllipseComp::GetClassId () { return IELLIPSE_COMP; }

boolean IEllipseComp::IsA (ClassId id) {
    return IELLIPSE_COMP == id || IComp::IsA(id);
}


/*****************************************************************************/

ClassId EllipseCode::GetClassId () { return IELLIPSE_CODE; }

boolean EllipseCode::IsA(ClassId id) {
    return IELLIPSE_CODE == id || CodeView::IsA(id);
}

EllipseCode::EllipseCode (IEllipseComp* subj) : CodeView(subj) {}

void EllipseCode::Update () {
    CodeView::Update();
    GetIEllipseComp()->Bequeath();
}

IEllipseComp* EllipseCode::GetIEllipseComp () {
    return (IEllipseComp*) GetSubject(); 
}

boolean EllipseCode::Definition (ostream& out) {
    boolean ok = true;

    IEllipseComp* ellipsecomp = GetIEllipseComp();
    EllipseComp* target = (EllipseComp*) ellipsecomp->GetTarget();
    SF_Ellipse* ellipsegr= target->GetEllipse();
    SubclassNameVar* snamer = ellipsecomp->GetClassNameVar();
    MemberNameVar* mnamer = ellipsecomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(ellipsegr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    EllipseComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (_scope && mnamer->GetExport()&&!_namelist->Search("ellipses")){
                _namelist->Append("ellipses");
                out << "#include <Unidraw/Graphic/ellipses.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("ellipses")) {
                _namelist->Append("ellipses");
                out << "#include <Unidraw/Graphic/ellipses.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        Coord x0, y0;
        int r1, r2;
        ellipsegr->GetOriginal(x0, y0, r1, r2);

        out << "    {\n";
        out << "        " << mname << " = new " << subclass << "(";
        out << x0 << ", " << y0 << ", " << r1 << ", " << r2 << ");\n";
        ok = WriteGraphicInits(ellipsegr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new EllipseComp(";
            out << mname << ");\n";
        }
        out << "    }\n";

    } else if (
        _emitCoreDecls || _emitCoreInits || _emitClassDecls || _emitClassInits
    ) {
	ok = ok && CodeView::Definition(out);
    }
    return ok && out.good();
}

boolean EllipseCode::CoreConstDecls(ostream& out) { 
    out << "(Coord x0, Coord y0, int r1, int r2, Graphic* = nil);\n";
    return out.good();
}

boolean EllipseCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out <<"(\n    Coord x0, Coord y0, int r1, int r2, Graphic* gr\n) : ";
    out << baseclass << "(x0, y0, r1, r2, gr) {}\n\n";

    return out.good();
}

boolean EllipseCode::ConstDecls(ostream& out) {
    out << "(Coord x0, Coord y0, int r1, int r2, Graphic* = nil);\n";
    return out.good();
}

boolean EllipseCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out <<"(\n    Coord x0, Coord y0, int r1, int r2, Graphic* gr\n) : ";
    out << coreclass << "(x0, y0, r1, r2, gr) {}\n\n";

    return out.good();
}

boolean EllipseCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("ellipses")) {
        _namelist->Append("ellipses");
        out << "#include <Unidraw/Graphic/ellipses.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("ellipse") && _emitGraphicComp
    ) {
        _namelist->Append("ellipse");
        out << "#include <Unidraw/Components/ellipse.h> \n";
    }
    return out.good();
}


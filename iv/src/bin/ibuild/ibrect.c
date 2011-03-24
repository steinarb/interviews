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
 * IRect component definitions.
 */


#include "ibrect.h"
#include "ibclasses.h"
#include "ibvars.h"
#include <Unidraw/Graphic/polygons.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/
static const char* rectcomp_delim = "%rectcomp_delim";
/*****************************************************************************/

IRectComp::IRectComp (SF_Rect* graphic) {
    GetClassNameVar()->SetName("SF_Rect");
    GetClassNameVar()->SetBaseClass("SF_Rect");
    if (!_release || graphic != nil) {
        _target = new RectComp(graphic);
        if (graphic != nil) {
            Append(_target);
        }
    }
}

ClassId IRectComp::GetClassId () { return IRECT_COMP; }

ClassId IRectComp::GetSubstId(const char*& delim) {
    delim = rectcomp_delim;
    return RECT_COMP;
}

boolean IRectComp::IsA (ClassId id) {
    return IRECT_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId RectCode::GetClassId () { return IRECT_CODE; }

boolean RectCode::IsA(ClassId id) {
    return IRECT_CODE == id || CodeView::IsA(id);
}

RectCode::RectCode (IRectComp* subj) : CodeView(subj) {}

void RectCode::Update () {
    CodeView::Update();
    GetIRectComp()->Bequeath();
}

IRectComp* RectCode::GetIRectComp () {
    return (IRectComp*) GetSubject(); 
}

boolean RectCode::Definition (ostream& out) {
    boolean ok = true;

    IRectComp* rectcomp = GetIRectComp();
    RectComp* target = (RectComp*) rectcomp->GetTarget();
    SF_Rect* rectgr= target->GetRect();
    SubclassNameVar* snamer = rectcomp->GetClassNameVar();
    MemberNameVar* mnamer = rectcomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(rectgr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    RectComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("polygons")
            ) {
                _namelist->Append("polygons");
                out << "#include <Unidraw/Graphic/polygons.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("polygons")) {
                _namelist->Append("polygons");
                out << "#include <Unidraw/Graphic/polygons.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        Coord x0, y0, x1, y1;
        rectgr->GetOriginal(x0, y0, x1, y1);

        out << "    {\n";
        out << "        " << mname << " = new " << subclass << "(";
        out << x0 << ", " << y0 << ", " << x1 << ", " << y1 << ");\n";
        ok = WriteGraphicInits(rectgr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new RectComp(";
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

boolean RectCode::CoreConstDecls(ostream& out) { 
    out << "(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);\n";
    return out.good();
}

boolean RectCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out <<"(\n    Coord x0, Coord y0, Coord x1, Coord y1, Graphic* gr\n) : ";
    out << baseclass << "(x0, y0, x1, y1, gr) {}\n\n";

    return out.good();
}

boolean RectCode::ConstDecls(ostream& out) {
    out << "(Coord x0, Coord y0, Coord x1, Coord y1, Graphic* = nil);\n";
    return out.good();
}

boolean RectCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out <<"(\n    Coord x0, Coord y0, Coord x1, Coord y1, Graphic* gr\n) : ";
    out << coreclass << "(x0, y0, x1, y1, gr) {}\n\n";

    return out.good();
}

boolean RectCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("polygons")) {
        _namelist->Append("polygons");
        out << "#include <Unidraw/Graphic/polygons.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("rect") && _emitGraphicComp
    ) {
        _namelist->Append("rect");
        out << "#include <Unidraw/Components/rect.h> \n";
    }
    return out.good();
}


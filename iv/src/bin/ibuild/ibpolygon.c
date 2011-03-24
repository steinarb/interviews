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
 * IPolygon component definitions.
 */


#include "ibpolygon.h"
#include "ibclasses.h"
#include "ibvars.h"
#include <Unidraw/Graphic/polygons.h>
#include <stream.h>
#include <string.h>

/****************************************************************************/
static const char* polycomp_delim = "%polycomp_delim";
/*****************************************************************************/

IPolygonComp::IPolygonComp (SF_Polygon* graphic) {
    GetClassNameVar()->SetName("SF_Polygon");
    GetClassNameVar()->SetBaseClass("SF_Polygon");
    if (!_release || graphic != nil) {
        _target = new PolygonComp(graphic);
        if (graphic != nil) {
            Append(_target);
        }
    }
}

ClassId IPolygonComp::GetSubstId(const char*& delim) {
    delim = polycomp_delim;
    return POLYGON_COMP;
}

ClassId IPolygonComp::GetClassId () { return IPOLYGON_COMP; }

boolean IPolygonComp::IsA (ClassId id) {
    return IPOLYGON_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId PolygonCode::GetClassId () { return IPOLYGON_CODE; }

boolean PolygonCode::IsA(ClassId id) {
    return IPOLYGON_CODE == id || CodeView::IsA(id);
}

PolygonCode::PolygonCode (IPolygonComp* subj) : CodeView(subj) {}

void PolygonCode::Update () {
    CodeView::Update();
    GetIPolygonComp()->Bequeath();
}

IPolygonComp* PolygonCode::GetIPolygonComp () {
    return (IPolygonComp*) GetSubject(); 
}

boolean PolygonCode::Definition (ostream& out) {
    boolean ok = true;

    IPolygonComp* polygoncomp = GetIPolygonComp();
    PolygonComp* target = (PolygonComp*) polygoncomp->GetTarget();
    SF_Polygon* polygongr= target->GetPolygon();
    SubclassNameVar* snamer = polygoncomp->GetClassNameVar();
    MemberNameVar* mnamer = polygoncomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(polygongr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    PolygonComp* " << mname << "_comp;\n";
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
        Coord *x, *y;
        int count;

        count = polygongr->GetOriginal(x, y);

        out << "    {\n";
        out << "        Coord polyx[" << count << "];\n";
        out << "        Coord polyy[" << count << "];\n";

        for (int i = 0; i < count; i++) {
            out << "        polyx[" << i << "] = ";
            out << x[i] << ";\n";
            out << "        polyy[" << i << "] = ";
            out << y[i] << ";\n";
        }
        out << "        " << mname;
        out << " = new " << subclass << "(";
        out << "polyx, polyy, " << count;
        out << ");\n";
        ok = WriteGraphicInits(polygongr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new PolygonComp(";
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

boolean PolygonCode::CoreConstDecls(ostream& out) { 
    out << "(Coord* x, Coord* y, int count, Graphic* = nil);\n";
    return out.good();
}

boolean PolygonCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out <<"(\n    Coord* x, Coord* y, int count, Graphic* gr\n) : ";
    out << baseclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean PolygonCode::ConstDecls(ostream& out) {
    out << "(Coord* x, Coord* y, int count, Graphic* = nil);\n";
    return out.good();
}

boolean PolygonCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out <<"(\n    Coord* x, Coord* y, int count, Graphic* gr\n) : ";
    out << coreclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean PolygonCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("polygons")) {
        _namelist->Append("polygons");
        out << "#include <Unidraw/Graphic/polygons.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("polygon") && _emitGraphicComp
    ) {
        _namelist->Append("polygon");
        out << "#include <Unidraw/Components/polygon.h> \n";
    }
    return out.good();
}


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
 * Implementation of ISplineComp
 */

#include "ibspline.h"
#include "ibclasses.h"
#include "ibvars.h"
#include <Unidraw/Graphic/splines.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/
static const char* spcomp_delim = "%spcomp_delim";
/*****************************************************************************/

ISplineComp::ISplineComp (SFH_OpenBSpline* g) {
    GetClassNameVar()->SetName("SFH_OpenBSpline");
    GetClassNameVar()->SetBaseClass("SFH_OpenBSpline");
    if (!_release || g != nil) {
        _target = new SplineComp(g);
        if (g != nil) {
            Append(_target);
        }
    }
}

ClassId ISplineComp::GetSubstId(const char*& delim) {
    delim = spcomp_delim;
    return SPLINE_COMP;
}

ClassId ISplineComp::GetClassId () { return ISPLINE_COMP; }

boolean ISplineComp::IsA (ClassId id) {
    return ISPLINE_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId SplineCode::GetClassId () { return ISPLINE_CODE; }

boolean SplineCode::IsA(ClassId id) {
    return ISPLINE_CODE == id || CodeView::IsA(id);
}

SplineCode::SplineCode (
    ISplineComp* subj
) : CodeView(subj) {}

void SplineCode::Update () {
    CodeView::Update();
    GetISplineComp()->Bequeath();
}

ISplineComp* SplineCode::GetISplineComp () {
    return (ISplineComp*) GetSubject(); 
}

boolean SplineCode::Definition (ostream& out) {
    boolean ok = true;

    ISplineComp* splinecomp = GetISplineComp();
    SplineComp* target = (SplineComp*) splinecomp->GetTarget();
    SFH_OpenBSpline* splinegr= target->GetSpline();
    SubclassNameVar* snamer = splinecomp->GetClassNameVar();
    MemberNameVar* mnamer = splinecomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(splinegr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    SplineComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("splines")
            ) {
                _namelist->Append("splines");
                out << "#include <Unidraw/Graphic/splines.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("splines")) {
                _namelist->Append("splines");
                out << "#include <Unidraw/Graphic/splines.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        Coord *x, *y;
        int count;

        count = splinegr->GetOriginal(x, y);

        out << "    {\n";
        out << "        Coord sx[" << count << "];\n";
        out << "        Coord sy[" << count << "];\n";

        for (int i = 0; i < count; i++) {
            out << "        sx[" << i << "] = ";
            out << x[i] << ";\n";
            out << "        sy[" << i << "] = ";
            out << y[i] << ";\n";
        }
        out << "        " << mname;
        out << " = new " << subclass << "(";
        out << "sx, sy, " << count <<");\n";
        ok = WriteGraphicInits(splinegr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new SplineComp(";
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

boolean SplineCode::CoreConstDecls(ostream& out) { 
    out << "(\n";
    out << "        Coord* x, Coord* y, int count, ";
    out << "Graphic* gr = nil\n";
    out << "    );\n";
    return out.good();
}

boolean SplineCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    Coord* x, Coord* y, int count, ";
    out << "Graphic* gr\n) : ";
    out << baseclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean SplineCode::ConstDecls(ostream& out) {
    out << "(\n";
    out << "        Coord* x, Coord* y, int count, ";
    out << "Graphic* gr = nil\n";
    out << "    );\n";
    return out.good();
}

boolean SplineCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    Coord* x, Coord* y, int count, ";
    out << "Graphic* gr\n) : ";
    out << coreclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean SplineCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("splines")) {
        _namelist->Append("splines");
        out << "#include <Unidraw/Graphic/splines.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("spline") && _emitGraphicComp
    ) {
        _namelist->Append("spline");
        out << "#include <Unidraw/Components/spline.h> \n";
    }
    return out.good();
}

/*****************************************************************************/
static const char* cspcomp_delim = "%cspcomp_delim";
/*****************************************************************************/

IClosedSplineComp::IClosedSplineComp (SFH_ClosedBSpline* graphic) {
    GetClassNameVar()->SetName("SFH_ClosedBSpline");
    GetClassNameVar()->SetBaseClass("SFH_ClosedBSpline");
    if (!_release || graphic != nil) {
        _target = new ClosedSplineComp(graphic);
        if (graphic != nil) {
            Append(_target);
        }
    }
}

ClassId IClosedSplineComp::GetSubstId(const char*& delim) {
    delim = cspcomp_delim;
    return CLOSEDSPLINE_COMP;
}

ClassId IClosedSplineComp::GetClassId () { return ICLOSEDSPLINE_COMP; }

boolean IClosedSplineComp::IsA (ClassId id) {
    return ICLOSEDSPLINE_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId ClosedSplineCode::GetClassId () { return ICLOSEDSPLINE_CODE; }

boolean ClosedSplineCode::IsA(ClassId id) {
    return ICLOSEDSPLINE_CODE == id || CodeView::IsA(id);
}

ClosedSplineCode::ClosedSplineCode (
    IClosedSplineComp* subj
) : CodeView(subj) {}

void ClosedSplineCode::Update () {
    CodeView::Update();
    GetIClosedSplineComp()->Bequeath();
}

IClosedSplineComp* ClosedSplineCode::GetIClosedSplineComp () {
    return (IClosedSplineComp*) GetSubject(); 
}

boolean ClosedSplineCode::Definition (ostream& out) {
    boolean ok = true;

    IClosedSplineComp* csplinecomp = GetIClosedSplineComp();
    ClosedSplineComp* target = (ClosedSplineComp*) csplinecomp->GetTarget();
    SFH_ClosedBSpline* closedsplinegr= target->GetClosedSpline();
    SubclassNameVar* snamer = csplinecomp->GetClassNameVar();
    MemberNameVar* mnamer = csplinecomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(closedsplinegr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    ClosedSplineComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("splines")
            ) {
                _namelist->Append("splines");
                out << "#include <Unidraw/Graphic/splines.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("splines")) {
                _namelist->Append("splines");
                out << "#include <Unidraw/Graphic/splines.h> \n";
                if (_emitGraphicComp) {
                    out << "#include <Unidraw/Components/spline.h> \n";
                }
            }
        }

    } else if (_emitInstanceInits) {
        Coord *x, *y;
        int count;

        count = closedsplinegr->GetOriginal(x, y);

        out << "    {\n";
        out << "        Coord cbsx[" << count << "];\n";
        out << "        Coord cbsy[" << count << "];\n";

        for (int i = 0; i < count; i++) {
            out << "        cbsx[" << i << "] = ";
            out << x[i] << ";\n";
            out << "        cbsy[" << i << "] = ";
            out << y[i] << ";\n";
        }
        out << "        " << mname;
        out << " = new " << subclass << "(";
        out << "cbsx, cbsy, " << count;
        out << ");\n";
        ok = WriteGraphicInits(closedsplinegr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new ClosedSplineComp(";
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

boolean ClosedSplineCode::CoreConstDecls(ostream& out) { 
    out << "(Coord* x, Coord* y, int count, Graphic* = nil);\n";
    return out.good();
}

boolean ClosedSplineCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out <<"(\n    Coord* x, Coord* y, int count, Graphic* gr\n) : ";
    out << baseclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean ClosedSplineCode::ConstDecls(ostream& out) {
    out << "(Coord* x, Coord* y, int count, Graphic* = nil);\n";
    return out.good();
}

boolean ClosedSplineCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out <<"(\n    Coord* x, Coord* y, int count, Graphic* gr\n) : ";
    out << coreclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean ClosedSplineCode::EmitIncludeHeaders(ostream& out) {
    if (!_namelist->Search("splines")) {
        _namelist->Append("splines");
        out << "#include <Unidraw/Graphic/splines.h> \n";
    }
    return out.good();
}


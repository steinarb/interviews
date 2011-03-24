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
 * Implementation of ILineComp and IMultiLineComp
 */

#include "ibline.h"
#include "ibclasses.h"
#include "ibvars.h"
#include <Unidraw/Graphic/lines.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/
static const char* lcomp_delim = "%lcomp_delim";
/*****************************************************************************/

ILineComp::ILineComp (Line* g) {
    GetClassNameVar()->SetName("Line");
    GetClassNameVar()->SetBaseClass("Line");
    if (!_release || g != nil) {
        _target = new LineComp(g);
        if (g != nil) {
            Append(_target);
        }
    }
}

ClassId ILineComp::GetSubstId(const char*& delim) {
    delim = lcomp_delim;
    return LINE_COMP;
}

ClassId ILineComp::GetClassId () { return ILINE_COMP; }

boolean ILineComp::IsA (ClassId id) {
    return ILINE_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId LineCode::GetClassId () { return ILINE_CODE; }

boolean LineCode::IsA(ClassId id) {
    return ILINE_CODE == id || CodeView::IsA(id);
}

LineCode::LineCode (ILineComp* subj) : CodeView(subj) {}

void LineCode::Update () {
    CodeView::Update();
    GetILineComp()->Bequeath();
}

ILineComp* LineCode::GetILineComp () {
    return (ILineComp*) GetSubject(); 
}

boolean LineCode::Definition (ostream& out) {
    boolean ok = true;

    ILineComp* linecomp = GetILineComp();
    LineComp* target = (LineComp*) linecomp->GetTarget();
    Line* linegr= target->GetLine();
    SubclassNameVar* snamer = linecomp->GetClassNameVar();
    MemberNameVar* mnamer = linecomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(linegr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    LineComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("lines")
            ) {
                _namelist->Append("lines");
                out << "#include <Unidraw/Graphic/lines.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("lines")) {
                _namelist->Append("lines");
                out << "#include <Unidraw/Graphic/lines.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        Coord x0, y0, x1, y1;
        linegr->GetOriginal(x0, y0, x1, y1);

        out << "    {\n";
        out << "        " << mname << " = new " << subclass << "(";
        out << x0 << ", " << y0 << ", " << x1 << ", " << y1 << ");\n";
        ok = WriteGraphicInits(linegr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new LineComp(";
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

boolean LineCode::CoreConstDecls(ostream& out) { 
    out << "(\n";
    out << "        Coord x0, Coord y0, Coord x1, Coord y1,";
    out << " Graphic* gr = nil\n";
    out << "    );\n";
    return out.good();
}

boolean LineCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    Coord x0, Coord y0, Coord x1, Coord y1,";
    out << " Graphic* gr\n) : ";
    out << baseclass << "(x0, y0, x1, y1, gr) {}\n\n";

    return out.good();
}

boolean LineCode::ConstDecls(ostream& out) {
    out << "(\n";
    out << "        Coord x0, Coord y0, Coord x1, Coord y1,";
    out << " Graphic* gr = nil\n";
    out << "    );\n";
    return out.good();
}

boolean LineCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    Coord x0, Coord y0, Coord x1, Coord y1,";
    out << " Graphic* gr\n) : ";
    out << coreclass << "(x0, y0, x1, y1, gr) {}\n\n";

    return out.good();
}

boolean LineCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("lines")) {
        _namelist->Append("lines");
        out << "#include <Unidraw/Graphic/lines.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("line") && _emitGraphicComp
    ) {
        _namelist->Append("line");
        out << "#include <Unidraw/Components/line.h> \n";
    }
    return out.good();
}

/****************************************************************************/
static const char* mlcomp_delim = "%mlcomp_delim";
/*****************************************************************************/

IMultiLineComp::IMultiLineComp (SF_MultiLine* g) {
    GetClassNameVar()->SetName("SF_MultiLine");
    GetClassNameVar()->SetBaseClass("SF_MultiLine");
    if (!_release || g != nil) {
        _target = new MultiLineComp(g);
        if (g != nil) {
            Append(_target);
        }
    }
}

ClassId IMultiLineComp::GetSubstId(const char*& delim) {
    delim = mlcomp_delim;
    return MULTILINE_COMP;
}

ClassId IMultiLineComp::GetClassId () { return IMULTILINE_COMP; }

boolean IMultiLineComp::IsA (ClassId id) {
    return IMULTILINE_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId MultiLineCode::GetClassId () { return IMULTILINE_CODE; }

boolean MultiLineCode::IsA(ClassId id) {
    return IMULTILINE_CODE == id || CodeView::IsA(id);
}

MultiLineCode::MultiLineCode (
    IMultiLineComp* subj
) : CodeView(subj) {}

void MultiLineCode::Update () {
    CodeView::Update();
    GetIMultiLineComp()->Bequeath();
}

IMultiLineComp* MultiLineCode::GetIMultiLineComp () {
    return (IMultiLineComp*) GetSubject(); 
}

boolean MultiLineCode::Definition (ostream& out) {
    boolean ok = true;

    IMultiLineComp* mlinecomp = GetIMultiLineComp();
    MultiLineComp* target = (MultiLineComp*) mlinecomp->GetTarget();
    SF_MultiLine* Mlinegr= target->GetMultiLine();
    SubclassNameVar* snamer = mlinecomp->GetClassNameVar();
    MemberNameVar* mnamer = mlinecomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(Mlinegr, out);

    } else if (_emitInstanceDecls || _emitForward || _emitClassHeaders) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    MultiLineComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("lines")
            ) {
                _namelist->Append("lines");
                out << "#include <Unidraw/Graphic/lines.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("lines")) {
                _namelist->Append("lines");
                out << "#include <Unidraw/Graphic/lines.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        Coord *x, *y;
        int count;

        count = Mlinegr->GetOriginal(x, y);

        out << "    {\n";
        out << "        Coord mlx[" << count << "];\n";
        out << "        Coord mly[" << count << "];\n";

        for (int i = 0; i < count; i++) {
            out << "        mlx[" << i << "] = ";
            out << x[i] << ";\n";
            out << "        mly[" << i << "] = ";
            out << y[i] << ";\n";
        }
        out << "        " << mname;
        out << " = new " << subclass << "(";
        out << "mlx, mly, " << count << ");\n";
        ok = WriteGraphicInits(Mlinegr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new MultiLineComp(";
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

boolean MultiLineCode::CoreConstDecls(ostream& out) { 
    out << "(\n";
    out << "        Coord* x, Coord* y, int count, ";
    out << "Graphic* gr = nil\n";
    out << "    );\n";
    return out.good();
}

boolean MultiLineCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    Coord* x, Coord* y, int count, ";
    out << "Graphic* gr\n) : ";
    out << baseclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean MultiLineCode::ConstDecls(ostream& out) {
    out << "(\n";
    out << "        Coord* x, Coord* y, int count, ";
    out << "Graphic* gr = nil\n";
    out << "    );\n";
    return out.good();
}

boolean MultiLineCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    Coord* x, Coord* y, int count, ";
    out << "Graphic* gr\n) : ";
    out << coreclass << "(x, y, count, gr) {}\n\n";

    return out.good();
}

boolean MultiLineCode::EmitIncludeHeaders(ostream& out) {
    if (!_namelist->Search("lines")) {
        _namelist->Append("lines");
        out << "#include <Unidraw/Graphic/lines.h> \n";
    }
    if (!_namelist->Search("line") && _emitGraphicComp) {
        _namelist->Append("line");
        out << "#include <Unidraw/Components/line.h> \n";
    }
    return out.good();
}


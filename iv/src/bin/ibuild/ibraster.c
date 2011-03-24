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
 * IRaster component definitions.
 */

#include "ibraster.h"
#include "ibclasses.h"
#include "ibglobals.h"
#include "ibvars.h"
#include <Unidraw/statevars.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/catalog.h>
#include <Unidraw/Graphic/rasterrect.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/
static const char* rastercomp_delim = "%rastercomp_delim";
/*****************************************************************************/

IRasterComp::IRasterComp (RasterRect* graphic) {
    GetClassNameVar()->SetName("RasterRect");
    GetClassNameVar()->SetBaseClass("RasterRect");
    if (!_release || graphic != nil) {
        _target = new RasterComp(graphic);
        if (graphic != nil) {
            Append(_target);
        }
    }
}

ClassId IRasterComp::GetSubstId(const char*& delim) {
    delim = rastercomp_delim;
    return RASTER_COMP;
}

ClassId IRasterComp::GetClassId () { return IRASTER_COMP; }

boolean IRasterComp::IsA (ClassId id) {
    return IRASTER_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/

ClassId RasterCode::GetClassId () { return IRASTER_CODE; }

boolean RasterCode::IsA(ClassId id) {
    return IRASTER_CODE == id || CodeView::IsA(id);
}

RasterCode::RasterCode (
    IRasterComp* subj
) : CodeView(subj) {}

void RasterCode::Update () {
    CodeView::Update();
    GetIRasterComp()->Bequeath();
}

IRasterComp* RasterCode::GetIRasterComp () {
    return (IRasterComp*) GetSubject(); 
}

boolean RasterCode::Definition (ostream& out) {
    boolean ok = true;

    IRasterComp* ircomp = GetIRasterComp();
    RasterComp* target = (RasterComp*) ircomp->GetTarget();
    RasterRect* raster = target->GetRasterRect();
    SubclassNameVar* snamer = ircomp->GetClassNameVar();
    MemberNameVar* mnamer = ircomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(raster, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (
                _scope && mnamer->GetExport() && 
                !_namelist->Search("rasterrect")
            ) {
                _namelist->Append("rasterrect");
                out << "#include <Unidraw/Graphic/rasterrect.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("rasterrect")) {
                _namelist->Append("rasterrect");
                out << "#include <Unidraw/Graphic/rasterrect.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        char substName[CHARBUFSIZE];
        const char* sfile = target->GetFileName();
        Catalog* catalog = unidraw->GetCatalog();

        if (!catalog->Exists(sfile)) {
            char orig[CHARBUFSIZE];
            const char* name = catalog->GetName(ircomp->GetRoot());
            char* dir = GetDirName(name);
            char* index = strrchr(sfile, '/');
            if (index == nil) {
                strcpy(orig, sfile);
            } else {
                strcpy(orig, &index[1]);
            }
            strcpy(substName, dir);
            strcat(substName, orig);
            sfile = substName;
            if (!catalog->Exists(sfile)) {
                catalog->Save(target, sfile);
            }
        }
        out << "    {\n";
        out << "        GraphicComp* " << mname << "_comp = ";
        out << "ImportCmd::Import(\"" << sfile << "\");\n";
        out << "        " << mname << " = (" << subclass << "*) " << mname;
        out << "_comp->GetGraphic();\n";
        ok = WriteGraphicInits(raster, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new RasterComp(";
            out << mname << ");\n";
            out << "        " << mname << "_comp->Update();\n";
        }
        out << "    }\n";

    } else if (
        _emitCoreDecls || _emitCoreInits || _emitClassDecls || _emitClassInits
    ) {
	ok = true;
    }
    return ok && out.good();
}

boolean RasterCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("rasterrect")) {
        _namelist->Append("rasterrect");
        out << "#include <Unidraw/Graphic/rasterrect.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("import")
    ) {
        _namelist->Append("import");
        out << "#include <Unidraw/Components/grcomp.h> \n";
        out << "#include <Unidraw/Commands/import.h> \n";
    }
    if (
        strcmp(snamer->GetName(), _classname) != 0 && 
        !_namelist->Search("rastercomp") && _emitGraphicComp
    ) {
        _namelist->Append("rastercomp");
        out << "#include <Unidraw/Components/rastercomp.h> \n";
    }
    return out.good();
}


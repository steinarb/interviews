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
 * Implementation of CodeView and subclasses.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibcode.c,v 1.2 91/09/27 14:07:29 tang Exp $
 */

#include "ibclasses.h"
#include "ibcmds.h"
#include "ibcode.h"
#include "ibinteractor.h"
#include "ibcomp.h"
#include "ibscene.h"
#include "ibvars.h"

#include <Unidraw/catalog.h>
#include <Unidraw/iterator.h>
#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/Components/grcomp.h>
#include <Unidraw/Graphic/graphic.h>
#include <Unidraw/Graphic/pspaint.h>

#include <InterViews/shape.h>
#include <InterViews/transformer.h>

#include <osfcn.h>
#include <stdlib.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/

char* FilterName (const char* name) {
    char* copy = strnew(name);
    char* tmp = copy;
    
    for(tmp = strpbrk(tmp, ".*-#"); tmp != nil; tmp = strpbrk(++tmp, ".*-#")){
        *tmp = '_';
    }
    return copy;
}

/*****************************************************************************/

implementList(CharStringList,char*);

StringList::StringList () { }

StringList::~StringList () {
    for (int i = 0; i < list.count(); i++) {
	char* s = list.item(i);
	delete s;
    }
}

void StringList::Append (const char* s) {
    char* copy = strnew(s);
    list.append(copy);
}

boolean StringList::Search (const char* s) {
    for (int i = 0; i < list.count(); i++) {
	if (strcmp(list.item(i), s) == 0) {
	    return true;
	}
    }
    return false;
}

/*****************************************************************************/
static boolean ISearch(UList* ilist, const char* classname) {
    boolean retval = false;
    for (UList* i = ilist->First(); i != ilist->End(); i = i->Next()) {
        SubclassNameVar* subclass = (SubclassNameVar*) (*i) ();
        const char* iclass = subclass->GetName();
        if (strcmp(iclass, classname) == 0) {
            retval = true;
            break;
        }
    }
    return retval;
}
/*****************************************************************************/
RootCodeView::RootCodeView (GraphicComp* subj) : CodeView(subj) {
    _emitForward = false;
    _emitBSDecls = false;
    _emitBSInits = false;
    _emitExpHeader = false;
    _emitCorehHeader = false;
    _emitInstanceDecls = false;
    _emitInstanceInits = false;
    _emitCoreDecls = false;
    _emitCoreInits = false;
    _emitProperty = false;
    _emitClassDecls = false;
    _emitClassInits = false;
    _emitClassHeaders = false;
    _emitHeaders = false;
    _emitFunctionDecls = false;
    _emitFunctionInits = false;
    _icomplete = false;
    _emitExport = false;
    _emitMain = false;
    _scope = false;
    _emitGraphicState = false;
    _emitGraphicComp = false;

    _instancelist = new UList;
    _functionlist = new StringList;
    _bsdeclslist = new StringList;
    _bsinitslist = new StringList;
    _namelist = new StringList;
    _globallist = new StringList;

    _brushlist = new StringList;
    _colorlist = new StringList;
    _fontlist = new StringList;
    _patternlist = new StringList;

    _classname = nil;
    *_errbuf = '\0';
}

RootCodeView::~RootCodeView () {
    delete _instancelist;
    delete _functionlist;
    delete _bsdeclslist;
    delete _bsinitslist;
    delete _namelist;
    delete _globallist;

    delete _brushlist;
    delete _colorlist;
    delete _fontlist;
    delete _patternlist;

    _instancelist = nil;
    _functionlist = nil;
    _bsdeclslist = nil;
    _bsinitslist = nil;
    _namelist = nil;
    _globallist = nil;

    _brushlist = nil;
    _colorlist = nil;
    _fontlist = nil;
    _patternlist = nil;
}

/*****************************************************************************/

boolean CodeView::_emitForward;
boolean CodeView::_emitBSDecls;
boolean CodeView::_emitBSInits;
boolean CodeView::_emitExpHeader;
boolean CodeView::_emitCorehHeader;

boolean CodeView::_emitInstanceDecls;
boolean CodeView::_emitInstanceInits;
boolean CodeView::_emitCoreDecls;
boolean CodeView::_emitCoreInits;
boolean CodeView::_emitProperty;
boolean CodeView::_emitClassDecls;
boolean CodeView::_emitClassInits;
boolean CodeView::_emitClassHeaders;
boolean CodeView::_emitHeaders;

boolean CodeView::_emitFunctionDecls;
boolean CodeView::_emitFunctionInits;
boolean CodeView::_icomplete;
boolean CodeView::_emitExport;
boolean CodeView::_emitMain;
boolean CodeView::_scope;
boolean CodeView::_emitGraphicState;
boolean CodeView::_emitGraphicComp;

UList* CodeView::_instancelist;
StringList* CodeView::_functionlist;
StringList* CodeView::_bsdeclslist;
StringList* CodeView::_bsinitslist;
StringList* CodeView::_namelist;
StringList* CodeView::_globallist;

StringList* CodeView::_brushlist;
StringList* CodeView::_colorlist;
StringList* CodeView::_fontlist;
StringList* CodeView::_patternlist;

char CodeView::_errbuf[CHARBUFSIZE*10];
const char* CodeView::_classname;

CodeView::CodeView (GraphicComp* subj) : PreorderView(subj) {
    _views = new UList;
    _lock = false;
}

CodeView::~CodeView () { 
    DeleteViews(); 
    delete _views; 
}

const char* CodeView::GetErrors () { return _errbuf; }

boolean CodeView::Definition(ostream& out) {
    boolean ok = true;

    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    GraphicComp* grcomp = (GraphicComp*) GetSubject();
    GetNameVarsCmd nameVarsCmd(grcomp);
    nameVarsCmd.Execute();

    SubclassNameVar* snamer = nameVarsCmd.GetClassNameVar();
    MemberNameVar* mnamer = nameVarsCmd.GetMemberNameVar();
    InstanceNameVar* inamer = nameVarsCmd.GetInstanceNameVar();
    const char* subclass = snamer->GetName();
    const char* baseclass = snamer->GetBaseClass();
    const char* mname = mnamer->GetName();
    const char* iname = (inamer == nil) ? nil : inamer->GetName();

    if (_emitProperty) {
	Graphic* gr;
        InteractorComp* intcomp = GetIntComp();
	gr = intcomp->GetGraphic();
        char* props = strnew(intcomp->GetProps()->GetPropsText());

	PSFont* psfont = gr->GetFont();
	PSColor* fgcolor = gr->GetFgColor();
	PSColor* bgcolor = gr->GetBgColor();

	if (psfont != nil) {
            const char* ifont = psfont->GetName();
            out << "    ";
            out << "{ \"*" << iname << "*font\",";
            out << "    ";
            out << "\"" << ifont << "\" },\n";
        }

	if (fgcolor != nil) {
	    int r, g, b;
	    char color[CHARBUFSIZE];
	    fgcolor->Intensities(r, g, b);
	    sprintf(color, "%04x%04x%04x", r, g, b);
            out << "    ";
            out << "{ \"*" << iname << "*foreground\",";
            out << "    ";
            out << "\"#" << color << "\" },\n";
	}

	if (bgcolor != nil) {
	    int r, g, b;
	    char color[CHARBUFSIZE];
	    bgcolor->Intensities(r, g, b);
	    sprintf(color, "%04x%04x%04x", r, g, b);
            out << "    ";
            out << "{ \"*" << iname << "*background\",";
            out << "    ";
            out << "\"#" << color << "\" },\n";
	}
        char* text = props;
        while (true) {
            char copy;
            char* index;
            if ((index = strchr(text, ':')) != nil) {
                copy = ':';
                *index = '\0';
            } else if ((index = strchr(text, ' ')) != nil) {
                copy = ' ';
                *index = '\0';
            } else if ((index = strchr(text, '\t')) != nil) {
                copy = '\t';
                *index = '\0';
            } else if ((index = strchr(text, '\n')) != nil) {
                copy = '\n';
                *index = '\0';
            } else if (*text == '\0') {
                break;
            } else {
                copy = '\0';
            }
            out << "    { \"*" << iname << "*" << text << "\",";
            out << "    ";
            if (copy != '\0') {
                *index = copy;
            }
            text = index + 1;

            if ((index = strchr(text, '\n')) != nil) {
                *index = '\0';
                out << "\"" << text << "\" },\n";
                *index = '\n';
                text = index + 1;
            } else if (*text == '\0') {
                out << "\"\" },\n";
                break;
            } else {
                out << "\"" << text << "\" },\n";
                break;
            }
        }
        delete props;

    } else if (_emitBSDecls) {
        InteractorComp* intcomp = GetIntComp();
	ButtonStateVar* bsVar = intcomp->GetButtonStateVar();
	const char* name = bsVar->GetName();

	if (_emitExport) {
	    if (bsVar->GetExport() && !_bsdeclslist->Search(name)) {
                _bsdeclslist->Append(name);
                out << "    ButtonState* " << name << ";\n";
	    }

	} else {
            if (!bsVar->GetExport() && !_bsdeclslist->Search(name)) {
                _bsdeclslist->Append(name);
                out << "    ButtonState* " << name << ";\n";
            }
	}

    } else if (_emitBSInits) {
	char ButtonClass[CHARBUFSIZE];
	char coreclass[CHARBUFSIZE];

        GetCoreClassName(coreclass);
	strcpy(ButtonClass, coreclass);
        strcat(ButtonClass, "_Button");

        InteractorComp* intcomp = GetIntComp();
	ButtonStateVar* bsVar = intcomp->GetButtonStateVar();
	const char* bsname = bsVar->GetName();
	const char* proc = bsVar->GetFuncName();
        boolean export = bsVar->GetExport();

	if (!_bsinitslist->Search(bsname)) {
	    _bsinitslist->Append(bsname);
            if (_emitMain) {
                out << "    ButtonState* " << bsname;
                out << " = new ButtonState(";
                out << bsVar->GetInitial() << ");\n";
            } else {
                if (export) {
                    out << "    " << bsname;
                } else {
                    out << "    " << ButtonClass << "* " << bsname;
                }
                out << " = new " << ButtonClass;
                out << "(" << bsVar->GetInitial() << ", this";
                if (proc != nil && *proc != '\0') {
                    out << ", " << "&" << coreclass << "::" << proc << ");\n";
                } else {
                    out << ", nil);\n";
                }
            }
        }
    } else if (_emitInstanceDecls) {
	if (_emitExport) {
	    if (mnamer->GetExport()) {
                out << "    " << subclass << "* " << mname << ";\n";
	    }
	} else {
	    if (!mnamer->GetExport() || _emitMain) {
                out << "    " << subclass << "* " << mname << ";\n";
	    }
	}

    } else if (_emitForward) {
        if (_scope && strcmp(subclass, _classname) != 0) {
            if (
                *subclass != '\0' && 
                !_namelist->Search(subclass) &&
                mnamer->GetExport()
            ) {
                _namelist->Append(subclass);
                out << "class " <<  subclass << ";\n";
            }
        }            
    } else if (_emitFunctionDecls) {
        InteractorComp* intcomp = GetIntComp();
	ButtonStateVar* bvar = intcomp->GetButtonStateVar();
        const char* proc = bvar->GetFuncName();
        if (*proc != '\0' && !_functionlist->Search(proc)) {
            _functionlist->Append(proc);
            out << "    virtual void " << proc << "();\n";
        }

    } else if (_emitFunctionInits) {
	char coreclass[CHARBUFSIZE];

        InteractorComp* intcomp = GetIntComp();
	ButtonStateVar* bvar = intcomp->GetButtonStateVar();
        const char* proc = bvar->GetFuncName();
	GetCoreClassName(coreclass);

        if (*proc != '\0' && !_functionlist->Search(proc)) {
            _functionlist->Append(proc);
            if (_emitClassInits)  {
                out << "void " << _classname << "::" << proc;
		out << "() {\n    /* unimplemented */\n}\n\n";
            } else {
                out << "void " << coreclass << "::" << proc << "() {}\n";
            }
        }

    } else if (_emitExpHeader) {
        Iterator i;
        InteractorComp* intcomp = GetIntComp();
        if (intcomp != nil && intcomp->IsANewScope()) {
            if (strcmp(subclass, _classname) == 0) {
                _scope = true;
                ok = ok && Iterate(out);
                if (!_namelist->Search(subclass)) {
                    _namelist->Append(subclass);
                    out << "#include \"" << subclass << ".h\"\n";
                }
                _scope = false;
            } else if (_scope) {
                if (mnamer->GetExport() && !_namelist->Search(subclass)) {
                    _namelist->Append(subclass);
                    out << "#include \"" << subclass << ".h\"\n";
                }
            } else {
                ok = ok && Iterate(out);
            }
        } else if (
            (strcmp(subclass, _classname) == 0 || 
             _scope && mnamer->GetExport()) && 
             !_namelist->Search(subclass)
        ) {
            _namelist->Append(subclass);
            out << "#include \"" << subclass << ".h\"\n";
        }
    } else if (_emitCoreDecls || _emitCoreInits) {
        if (
            strcmp(subclass, _classname) == 0 &&
            !_globallist->Search(_classname)
        ) {
	    _globallist->Append(_classname);
            if (_emitCoreDecls) {
                out << "class " << coreclass;
                out << " : public " << baseclass << " {\n";
                out << "public:\n";
                out << "    " << coreclass;
                ok = ok && CoreConstDecls(out);

                out << "};\n\n";
                
            } else {
                out << coreclass << "::" << coreclass;
                ok = ok && CoreConstInits(out);
                
            }
        }

    } else if (_emitClassDecls) {
        if (
            strcmp(subclass, _classname) == 0 &&
            !_globallist->Search(_classname)
        ) {
	    _globallist->Append(_classname);
            out << "class " << subclass;
            out << " : public " << coreclass << " {\n";
            out << "public:\n";
            out << "    " << subclass;
            ok = ok && ConstDecls(out);
            out << "};\n\n";
        }

    } else if (_emitClassInits) {
        if (
            strcmp(subclass, _classname) == 0 &&
            !_globallist->Search(_classname)
        ) {
	    _globallist->Append(_classname);
            out << subclass << "::" << subclass;
            ok = ok && ConstInits(out);

        }
        
    } else if (_emitHeaders) {
        if (
            *_classname == '\0' || _scope || strcmp(subclass, _classname) == 0
        ) {
            ok = ok && EmitIncludeHeaders(out);
        }
    } else if (_emitClassHeaders) {
        if (snamer->IsSubclass()) {
            if (*_classname == '\0' || _scope) {
                if (!_namelist->Search("perspective")) {
                    _namelist->Append("perspective");
                    out << "#include <InterViews/perspective.h> \n";
                }
                if (!_namelist->Search(subclass)) {
                    _namelist->Append(subclass);
                    out << "#include \"" << subclass << ".h\"\n";
                }
            } else if (strcmp(subclass, _classname) == 0) {
                if (!_namelist->Search("perspective")) {
                    _namelist->Append("perspective");
                    out << "#include <InterViews/perspective.h> \n";
                }
                if (!_namelist->Search(subclass)) {
                    _namelist->Append(subclass);
                    out << "#include \"" << subclass << ".h\"\n";
                }
            }
        }
        
    } else if (_emitMain) {
        CleanUp();
        out << "static Interactor* " << iname << "() {\n";
        ok = ok && EmitBSInits(this, out);
        ok = ok && EmitInstanceInits(this, out);
        out << "    return " << mname << ";\n};\n\n";
    }
    return out.good() && ok;
}

void CodeView::CleanUp () {
    delete _instancelist;
    delete _bsdeclslist;
    delete _bsinitslist;
    delete _functionlist;
    delete _namelist;

    delete _brushlist;
    delete _colorlist;
    delete _fontlist;
    delete _patternlist;

    _instancelist = new UList;
    _bsdeclslist = new StringList;
    _bsinitslist = new StringList;
    _functionlist = new StringList;
    _namelist = new StringList;

    _brushlist = new StringList;
    _colorlist = new StringList;
    _fontlist = new StringList;
    _patternlist = new StringList;
}
    
static int CalcBitmap (float graylevel) {
    static const int SHADES = 17;
    static int shades[SHADES] = {
	0xffff, 0xefff, 0xefbf, 0xafbf,
	0xafaf, 0xadaf, 0xada7, 0xa5a7,
	0xa5a5, 0x85a5, 0x8525, 0x0525,
	0x0505, 0x0405, 0x0401, 0x0001,
	0x0000
    };
    return shades[round(graylevel * (SHADES - 1))];
}

boolean CodeView::WriteGraphicDecls(Graphic* gr, ostream& out) {
    PSBrush* brush = gr->GetBrush();
    PSColor* fgcolor = gr->GetFgColor();
    PSColor* bgcolor = gr->GetBgColor();
    PSFont* font = gr->GetFont();
    PSPattern* pattern = gr->GetPattern();
    
    if (brush != nil) {
        char brushname[CHARBUFSIZE];
        int p, w;
        boolean none = false;
        p = brush->GetLinePattern();
        w = brush->Width();
        if (brush->None()) {
            sprintf(brushname, "brush_none");
            none = true;
        } else {
            sprintf(brushname, "brush_%d_%d", p, w);
        }
        if (!_brushlist->Search(brushname)) {
            _brushlist->Append(brushname);
            if (none) {
                out << "    PSBrush* " << brushname << " = new PSBrush;\n";
            } else {
                out << "    PSBrush* " << brushname << " = new PSBrush(";
                out << p << ", " << w << ");\n";
            }
        }
    }
    if (fgcolor != nil) {
        const char* name = fgcolor->GetName();
	char* fname = FilterName(name);
        if (!_colorlist->Search(fname)) {
            _colorlist->Append(fname);
            if (
                strcmp(fname, "white") == 0 || 
                strcmp(fname, "White") == 0
            ) {
                out << "    PSColor* " << fname << " = new PSColor(";
                out << "1, 1, 1, \"" << fname << "\");\n";
            } else {
                int r, g, b;
                fgcolor->Intensities(r, g, b);
                float fr = float(r) / 0xffff;
                float fg = float(g) / 0xffff;
                float fb = float(b) / 0xffff;
                out << "    PSColor* " << fname << " = new PSColor(";
                out << fr << ", " << fg << ", " << fb << ", \"";
                out << fname << "\");\n";
            }
        }
	delete fname;
    }
    if (bgcolor != nil) {
        const char* name = bgcolor->GetName();
	char* fname = FilterName(name);
        if (!_colorlist->Search(fname)) {
            _colorlist->Append(fname);
            if (
                strcmp(fname, "white") == 0 || 
                strcmp(fname, "White") == 0
            ) {
                out << "    PSColor* " << fname << " = new PSColor(";
                out << "1, 1, 1, \"" << fname << "\");\n";
            } else {
                int r, g, b;
                bgcolor->Intensities(r, g, b);
                float fr = float(r) / 0xffff;
                float fg = float(g) / 0xffff;
                float fb = float(b) / 0xffff;
                out << "    PSColor* " << fname << " = new PSColor(";
                out << fr << ", " << fg << ", " << fb << ", \"";
                out << fname << "\");\n";
            }
        }
        delete fname;
    }
    if (font != nil) {
        char PF[CHARBUFSIZE];

        const char* name = font->GetName();
        const char* pf = font->GetPrintFont();
        const char* ps = font->GetPrintSize();

        sprintf(PF, "%s%s", pf, ps);
	char* font = FilterName(PF);
        if (!_fontlist->Search(font)) {
            _fontlist->Append(font);
            out << "    PSFont* " << font << " = new PSFont(\"";
            out << name << "\", \"" << font << "\", \"" << ps << "\");\n";
        }
        delete font;
    }
    if (pattern != nil) {
        char patternname[CHARBUFSIZE];
        char patternarray[CHARBUFSIZE];
        char buf[CHARBUFSIZE];
    
        if (pattern->None()) {
            sprintf(patternname, "pat_none");
            if (!_patternlist->Search(patternname)) {
                _patternlist->Append(patternname);
                out << "    PSPattern* " << patternname;
                out << " = new PSPattern;\n";
            }

        } else if (pattern->GetSize() > 0) {
            sprintf(patternname, "pat_");
            sprintf(patternarray, "patarray_");

            const int* data = pattern->GetData();
            int size = pattern->GetSize();

            if (size <= 8) {
                for (int i = 0; i < 8; i++) {
                    sprintf(buf, "%02x", data[i] & 0xff);
                    strcat(patternname, buf);
                    strcat(patternarray, buf);
                }
                
            } else {
                for (int i = 0; i < patternHeight; i++) {
                    sprintf(buf, "%0*x", patternWidth/4, data[i]);
                    strcat(patternname, buf);
                    strcat(patternarray, buf);
                }
            }
            char* filtername = FilterName(patternname);
            char* filterarray = FilterName(patternarray);
            if (!_patternlist->Search(filtername)) {
                _patternlist->Append(filtername);

                out << "    int " << filterarray << "[" << patternHeight;
                out << "];\n";
                for (int i = 0; i < patternHeight; i++) {
                    sprintf(buf, "%d", data[i]);
                    out << "    " << filterarray << "[" << i << "] = ";
                    out << buf << ";\n";
                }
                out << "    PSPattern* " << filtername << " = new PSPattern(";
                out << filterarray << ", " << size << ");\n";
            }
            delete filtername;
            delete filterarray;
        } else {
            float graylevel = pattern->GetGrayLevel();
            sprintf(patternname, "pat_%f", graylevel);
            char* filtername = FilterName(patternname);
            if (!_patternlist->Search(filtername)) {
                _patternlist->Append(filtername);
                int shade = CalcBitmap(graylevel);
                out << "    PSPattern* " << filtername << " = new PSPattern(";
                out << shade << ", (float)" << graylevel << ");\n";
            }
            delete filtername;
        }
    }
    return out.good();
}

boolean CodeView::WriteGraphicInits(Graphic* gr, ostream& out) {
    int fillbg = gr->BgFilled();
    PSColor* fgcolor = gr->GetFgColor();
    PSColor* bgcolor = gr->GetBgColor();
    PSPattern* pattern = gr->GetPattern();
    PSBrush* brush = gr->GetBrush();
    PSFont* font = gr->GetFont();
    Transformer* tr = gr->GetTransformer();
    const char* mname = GetIComp()->GetMemberNameVar()->GetName();

    if (fillbg >= 0) {
        out << "        " << mname << "->FillBg(" << fillbg << ");\n";
    }
    if (brush != nil) {
        char brushname[CHARBUFSIZE];
        if (brush->None()) {
            sprintf(brushname, "brush_none");
        } else {
            int p = brush->GetLinePattern();
            int w = brush->Width();
            sprintf(brushname, "brush_%d_%d", p, w);
        }
        out << "        " << mname << "->SetBrush(" << brushname;
        out << ");\n";
    }
    if (fgcolor != nil && bgcolor != nil) {
        const char* fname = fgcolor->GetName();
        const char* bname = bgcolor->GetName();
	char* fgname = FilterName(fname);
	char* bgname = FilterName(bname);
        out << "        " << mname << "->SetColors(" << fgname;
        out << ", " << bgname << ");\n";
        delete fgname;
        delete bgname;
    } else if (fgcolor != nil && bgcolor == nil) {
        const char* fname = fgcolor->GetName();
	char* fgname = FilterName(fname);
        out << "        " << mname << "->SetColors(" << fgname;
        out << ", nil);\n";
        delete fgname;
    } else if (fgcolor == nil && bgcolor != nil) {
        const char* bname = bgcolor->GetName();
	char* bgname = FilterName(bname);
        out << "        " << mname << "->SetColors(nil, " << bgname;
        out << ");\n";
        delete bgname;
    }
    if (font != nil) {
        char PF[CHARBUFSIZE];

        const char* pf = font->GetPrintFont();
        const char* ps = font->GetPrintSize();

        sprintf(PF, "%s%s", pf, ps);
	char* font = FilterName(PF);
        out << "        " << mname << "->SetFont(" << font << ");\n";
        delete font;
    }
    if (pattern != nil) {
        char patternname[CHARBUFSIZE];
        char buf[CHARBUFSIZE];
    
        if (pattern->None()) {
            out << "        " << mname << "->SetPattern(pat_none);\n";

        } else if (pattern->GetSize() > 0) {
            sprintf(patternname, "pat_");

            const int* data = pattern->GetData();
            int size = pattern->GetSize();

            if (size <= 8) {
                for (int i = 0; i < 8; i++) {
                    sprintf(buf, "%02x", data[i] & 0xff);
                    strcat(patternname, buf);
                }
            } else {
                for (int i = 0; i < patternHeight; i++) {
                    sprintf(buf, "%0*x", patternWidth/4, data[i]);
                    strcat(patternname, buf);
                }
            }
            char* filtername = FilterName(patternname);
            out << "        " << mname << "->SetPattern(";
            out << filtername << ");\n";
            delete filtername;
        } else {
            float graylevel = pattern->GetGrayLevel();
            sprintf(patternname, "pat_%f", graylevel);
            char* filtername = FilterName(patternname);
            out << "        " << mname << "->SetPattern(";
            out << filtername << ");\n";
            delete filtername;
        }
    }
    if (tr != nil) {
        float a00, a01, a10, a11, a20, a21;
        tr->GetEntries(a00, a01, a10, a11, a20, a21);
        out << "        " << mname;
        out << "->SetTransformer(new Transformer(";
        out << a00 << ", " << a01 << ", " << a10 << ", " << a11 << ", ";
        out << a20 << ", " << a21 << "));\n";
    }
    return out.good();
}

ClassId CodeView::GetClassId () { return CODE_VIEW; }

boolean CodeView::IsA (ClassId id) {
    return CODE_VIEW == id || PreorderView::IsA(id);
}

boolean CodeView::Search (MemberNameVar* mnamer, InteractorComp*& ctarget) {
    boolean ok = false;
    GetFirewallCmd firewallCmd((GraphicComp*) GetSubject());
    firewallCmd.Execute();
    InteractorComp* firewall = firewallCmd.GetFirewall();

    GetConflictCmd conflictCmd(firewall, mnamer->GetName());
    conflictCmd.Execute();
    if (!conflictCmd.GetConflict()->IsEmpty()) {
        ok = true;
        if (ctarget != nil) {
            ctarget = conflictCmd.GetCTarget();
        }
    }
    return ok;
}

void CodeView::GetClassList(UList* ulist) {
    Iterator i;
    GraphicComp* grcomp = (GraphicComp*) GetSubject();
    GetNameVarsCmd nameVarsCmd(grcomp);
    nameVarsCmd.Execute();
    SubclassNameVar* classname = nameVarsCmd.GetClassNameVar();
    
    if (classname->IsSubclass()) {
        const char* cname = classname->GetName();
        if (!ISearch(ulist, cname)) {
            ulist->Append(new UList(classname));
        }
    }
    for (First(i); !Done(i); Next(i)) {
        CodeView* kidview = (CodeView*) GetView(i);
        kidview->GetClassList(ulist);
    }
}

boolean CodeView::GenDothFile (const char* orig, ostream& out) {

    CleanUp();

    _classname = orig;
    delete _globallist;
    _globallist = new StringList;

    boolean ok = true;
    char* filtered = FilterName(orig);

    out << "#ifndef " << filtered << "_h\n";
    out << "#define " << filtered << "_h\n\n";
    out << "#include \"" << orig << "-core.h\"\n\n";

    ok = ok && EmitClassDecls(out);

    out << "#endif\n";
    delete filtered;
    ok = ok && out.good();
    if (!ok) {
        strcat(_errbuf, orig);
        strcat(_errbuf, ".h code generation failed.\n");
    }
    return ok && out.good();
}


boolean CodeView::GenCorecFile (const char* orig, ostream& out) {

    CleanUp();
    _classname = orig;
    delete _globallist;
    _globallist = new StringList;

    out << "#include <InterViews/canvas.h> \n";
    out << "#include <InterViews/painter.h> \n";
    out << "#include <InterViews/sensor.h> \n";
    boolean ok = EmitHeaders(out);
    ok = ok && EmitClassHeaders(out);
    out << "#include <InterViews/2.6/_enter.h>\n\n";
    out << "\n";
    if (Scan(BUTTON_COMP) || Scan(DIALOG_CLASS)) {
        ok = ok && EmitButtonState(out);
    }
    if (Scan(GLUE_COMP)) {
        ok = ok && EmitGlue(out);
    }
    if (Scan(MENUITEM_COMP)) {
        ok = ok && EmitMenu(out);
    }
    if (Scan(SHAPER_COMP)) {
        ok = ok && EmitShaper(out);
    }
    if (Scan(SLIDER_COMP)) {
        ok = ok && EmitSlider(out);
    }
    ok = ok && EmitCoreInits(out);

    if (!ok) {
	strcat(_errbuf, orig);
        strcat(_errbuf, "-core.c code generation failed.\n");
    }
    return ok;
}

boolean CodeView::GenCorehFile (const char* orig, ostream& out) {

    _classname = orig;
    delete _globallist;
    _globallist = new StringList;

    boolean ok = true;
    char* filtered = FilterName(orig);

    out << "#ifndef " << filtered << "_core_h\n";
    out << "#define " << filtered << "_core_h\n\n";

    ok = ok && EmitCorehHeader(out);
    ok = ok && EmitForwardDecls(out);
    ok = ok && EmitCoreDecls(out);

    out << "#endif\n";
    delete filtered;
    ok = ok && out.good();
    if (!ok) {
	strcat(_errbuf, orig);
        strcat(_errbuf, "-core.h code generation failed.\n");
    }
    return ok && out.good();
}

boolean CodeView::EmitClassDecls (ostream& out) {
    _emitClassDecls = true;
    CleanUp();
    boolean ok = Iterate(out);
    _emitClassDecls = false;
    if (!ok) {
        strcat(_errbuf, "Class declaration failed.\n");
    }
    return ok;
}

boolean CodeView::GenDotcFile (const char* orig, ostream& out) {

    CleanUp();
    _classname = orig;
    delete _globallist;
    _globallist = new StringList;

    boolean ok = EmitExpHeader(out);
    out << "#include <InterViews/2.6/_enter.h>\n\n";

    ok = ok && EmitClassInits(out);

    if (!ok) {
	strcat(_errbuf, orig);
        strcat(_errbuf, ".c code generation failed.\n");
    }
    return ok;
}

boolean CodeView::EmitClassInits (ostream& out) {
    _emitClassInits = true;
    CleanUp();
    boolean ok = Iterate(out);
    _emitClassInits = false;

    if (!ok) {
        strcat(_errbuf, "Class initialization failed.\n");
    }
    return ok;
}

boolean CodeView::EmitForwardDecls(ostream& out) {
    _emitForward = true;
    CleanUp();
    boolean ok = Iterate(out);
    out << "\n";
    _emitForward = false;

    return ok;
}

boolean CodeView::EmitSlider(ostream& out) {
    char coreclass[CHARBUFSIZE];
    char SliderClass[CHARBUFSIZE];

    GetCoreClassName(coreclass);
    strcpy(SliderClass, coreclass);
    strcat(SliderClass, "_Slider");

    out << "class " << SliderClass << " : public Slider {\n";
    out << "public:\n";
    out << "    " << SliderClass << "(const char*, Interactor*, int width);\n";
    out << "};\n\n";

    out << SliderClass << "::" << SliderClass;
    out << " (\n    const char* name, Interactor* i";
    out << ", int width) : Slider(name, i)\n";
    out << "{\n";
    out << "    Perspective* target = i->GetPerspective();\n";
    out << "    shape->width = width;\n";
    out << "    float aspect = float(target->height)";
    out << " / float(target->width);\n";
    out << "    shape->height = (int) (aspect * width);\n";
    out << "    shape->Rigid();\n";
    out << "}\n\n";
    if (!out.good()) {
        strcat(_errbuf, "Slider generation failed.\n");
    }
    return out.good();
}

boolean CodeView::EmitGlue(ostream& out) {
    char coreclass[CHARBUFSIZE];
    char HGlueClass[CHARBUFSIZE];
    char VGlueClass[CHARBUFSIZE];

    GetCoreClassName(coreclass);
    strcpy(HGlueClass, coreclass);
    strcat(HGlueClass, "_HGlue");
    strcpy(VGlueClass, coreclass);
    strcat(VGlueClass, "_VGlue");
    
    out << "class " << HGlueClass << " : public HGlue {\n";
    out << "public:\n";
    out << "    " << HGlueClass << "(const char*, int, int, int);\n";
    out << "protected:\n";
    out << "    virtual void Resize();\n";
    out << "};\n\n";

    out << "class " << VGlueClass << " : public VGlue {\n";
    out << "public:\n";
    out << "    " << VGlueClass << "(const char*, int, int, int);\n";
    out << "protected:\n";
    out << "    virtual void Resize();\n";
    out << "};\n\n";

    out << HGlueClass << "::" << HGlueClass << " (\n";
    out << "    const char* name, int nat, int shr, int str)";
    out << " : HGlue(name, nat, shr, str) {}\n\n";

    out << "void " << HGlueClass << "::Resize () {\n";
    out << "    HGlue::Resize();\n";
    out << "    canvas->SetBackground(output->GetBgColor());\n";
    out << "}\n\n";

    out << VGlueClass << "::" << VGlueClass << " (\n";
    out << "    const char* name, int nat, int shr, int str)";
    out << " : VGlue(name, nat, shr, str) {}\n\n";

    out << "void " << VGlueClass << "::Resize () {\n";
    out << "    VGlue::Resize();\n";
    out << "    canvas->SetBackground(output->GetBgColor());\n";
    out << "}\n\n";

    if (!out.good()) {
        strcat(_errbuf, "IBGlue generation failed.\n");
    }
    return out.good();
}

boolean CodeView::EmitShaper(ostream& out) {
    char coreclass[CHARBUFSIZE];
    char ShaperClass[CHARBUFSIZE];

    GetCoreClassName(coreclass);
    strcpy(ShaperClass, coreclass);
    strcat(ShaperClass, "_Shaper");

    out << "class " << ShaperClass << " : public MonoScene {\n";
    out << "public:\n";
    out << "    " << ShaperClass;
    out << "(const char*, int, int, int, int, int, int);\n";
    out << "    void Reconfig();\n";
    out << "private:\n";
    out << "    int _h, _v;\n";
    out << "    int _hstr, _vstr;\n";
    out << "    int _hshr, _vshr;\n";
    out << "};\n\n";

    out << ShaperClass << "::" << ShaperClass << " (\n";
    out << "    const char* name, int h, int v, ";
    out << "int hstr, int vstr, int hshr, int vshr\n) {\n";
    out << "    SetInstance(name);\n";
    out << "    _h = h, _v = v;\n";
    out << "    _hstr = hstr, _vstr = vstr;\n";
    out << "    _hshr = hshr, _vshr = vshr;\n";
    out << "}\n\n";

    out << "void " << ShaperClass << "::Reconfig () {\n";
    out << "    MonoScene::Reconfig();\n";
    out << "    shape->width = _h;\n";
    out << "    shape->height = _v;\n";
    out << "    shape->hstretch = _hstr;\n";
    out << "    shape->vstretch = _vstr;\n";
    out << "    shape->hshrink = _hshr;\n";
    out << "    shape->vshrink = _vshr;\n";
    out << "}\n\n";
   if (!out.good()) {
        strcat(_errbuf, "Shaper generation failed.\n");
    }
    return out.good();
}

boolean CodeView::EmitMenu(ostream& out) {
    char Func[CHARBUFSIZE];
    char MenuClass[CHARBUFSIZE];
    char coreclass[CHARBUFSIZE];
    
    GetCoreClassName(coreclass);
    strcpy(MenuClass, coreclass);
    strcat(MenuClass, "_Menu");
    strcpy(Func, coreclass);
    strcat(Func, "_Func");
    
    out << "#ifndef " << coreclass << "_func\n";
    out << "#define " << coreclass << "_func\n";
    out << "typedef void (" << coreclass << "::*" << Func << ")();\n";
    out << "#endif\n\n";
    out << "class " << MenuClass << " : public MenuItem {\n";
    out << "public:\n";
    out << "    " << MenuClass;
    out << "(const char*,  const char*, Alignment);\n";
    out << "    virtual void Do();\n";
    out << "    void SetCoreClass(" << coreclass << "*);\n";
    out << "    void SetCoreFunc(" << Func << ");\n";
    out << "private:\n";
    out << "    " << Func << " _func;\n";
    out << "    " << coreclass << "* _coreclass;\n";
    out << "};\n\n";

    out << MenuClass << "::" << MenuClass << "(\n";
    out << "    const char* instance, const char* str, Alignment al\n";
    out << ") : MenuItem(str, al) {\n";
    out << "    _func = nil;\n";
    out << "    _coreclass = nil;\n";
    out << "    SetInstance(instance);\n";
    out << "}\n\n";
    
    out << "void " << MenuClass << "::SetCoreClass(" << coreclass;
    out << "* core) {\n";
    out << "    _coreclass = core;\n";
    out << "}\n\n";

    out << "void " << MenuClass << "::SetCoreFunc(" << Func;
    out << " func) {\n";
    out << "    _func = func;\n";
    out << "}\n\n";

    out << "void " << MenuClass << "::Do() {\n";
    out << "    if (_func != nil) {\n";
    out << "        (_coreclass->*_func)();\n";
    out << "    }\n";
    out << "}\n\n";
    if (!out.good()) {
        strcat(_errbuf, "Menu generation failed.\n");
    }
    return out.good();
}

boolean CodeView::EmitButtonState(ostream& out) {
    char BFunc[CHARBUFSIZE];
    char ButtonClass[CHARBUFSIZE];
    char coreclass[CHARBUFSIZE];
    
    GetCoreClassName(coreclass);
    strcpy(BFunc, coreclass);
    strcat(BFunc, "_BFunc");
    strcpy(ButtonClass, coreclass);
    strcat(ButtonClass, "_Button");
    
    out << "typedef void (" << coreclass<< "::*" << BFunc << ")();\n\n";
    out << "class " << ButtonClass << " : public ButtonState {\n";
    out << "public:\n";
    out << "    " << ButtonClass;
    out << "(\n\tint, " << coreclass << "* = nil, ";
    out << BFunc << " = nil\n    );\n";
    out << "    virtual void Notify();\n";
    out << "private:\n";
    out << "    " << BFunc << " _func;\n";
    out << "    " << coreclass << "* _coreclass;\n";
    out << "};\n\n";
    out << ButtonClass << "::" << ButtonClass << "(\n";
    out << "    int i, ";
    out << coreclass << "* coreclass, " << BFunc;
    out << " f\n";
    out << ") : ButtonState(i) {\n";
    out << "    _func = f;\n";
    out << "    _coreclass = coreclass;\n";
    out << "}\n\n";
    
    out << "void " << ButtonClass << "::Notify() {\n";
    out << "    ButtonState::Notify();\n";
    out << "    if (_func != nil) {\n";
    out << "        (_coreclass->*_func)();\n";
    out << "    }\n";
    out << "}\n\n";

    if (!out.good()) {
        strcat(_errbuf, "ButtonState generation failed.\n");
    }
    return out.good();
}

boolean CodeView::GenPropFile (const char* orig, ostream& out) {
    _classname = orig;
    boolean ok = EmitPropertyData(out);
    return ok;
}

boolean CodeView::EmitIncludeHeaders(ostream&) { return true; }

boolean CodeView::EmitHeaders(ostream& out) {
    CleanUp();
    _emitHeaders = true;
    boolean ok = Iterate(out);
    _emitHeaders = false;
    if (!ok) {
        strcat(_errbuf, "Header file generation failed.\n");
    }
    return ok;
}

boolean CodeView::EmitClassHeaders(ostream& out) {
    CleanUp();
    _emitClassHeaders = true;
    boolean ok = Iterate(out);
    _emitClassHeaders = false;
    if (!ok) {
        strcat(_errbuf, "Class header file generation failed.\n");
    }
    return ok;
}

boolean CodeView::Iterate (ostream& out) {
    Iterator i;
    boolean ok = true;
    for(First(i); !Done(i); Next(i)) {
        CodeView* kid = (CodeView*) GetView(i);
        ok = kid->Definition(out) && ok;
    }
    return ok;
}
    
boolean CodeView::GenMainFile (const char* orig, ostream& out) {
    Iterator i;
    boolean ok = true;
    _emitMain = true;

    _classname = "";
    CleanUp();
    out << "#include <Unidraw/catalog.h> \n";
    out << "#include <Unidraw/unidraw.h> \n";
    out << "#include <Unidraw/creator.h> \n";

    out << "#include <InterViews/canvas.h> \n";
    out << "#include <InterViews/painter.h> \n";
    out << "#include <InterViews/sensor.h> \n";
    out << "#include <InterViews/world.h> \n";

    ok = ok && EmitHeaders(out);
    ok = ok && EmitClassHeaders(out);
    out << "#include <InterViews/2.6/_enter.h>\n\n";

    out << "static PropertyData properties[] = {\n";
    out << "#include \"" << orig << "-props\"\n";
    out << "    { nil }\n};\n\n";
    out << "static OptionDesc options[] = {\n";
    out << "    { nil }\n};\n\n";

    if (Scan(SLIDER_COMP)) {
        ok = ok && EmitSlider(out);
    }
    if (Scan(SHAPER_COMP)) {
        ok = ok && EmitShaper(out);
    }
    if (Scan(GLUE_COMP)) {
        ok = ok && EmitGlue(out);
    }

    
    ok = ok && Iterate(out);
    _emitMain = false;

    out << "int main (int argc, char** argv) {\n";

    out << "    Creator creator;\n";
    out << "    Unidraw* unidraw = new Unidraw(\n";
    out << "        new Catalog(\"/****/\", &creator), argc, argv";
    out << ", options, properties\n";
    out << "    );\n";
    out << "    World* w = unidraw->GetWorld();\n";
    
    for(First(i); !Done(i); Next(i)) {
        CodeView* kid = (CodeView*) GetView(i);
        InteractorComp* icomp = kid->GetIntComp();
        if (icomp->IsANewScope()) {
            MonoSceneClass* mkid = (MonoSceneClass*) icomp;
            MemberNameVar* mnamer = mkid->GetMemberNameVar();
            const char* mname = mnamer->GetName();
            const char* instance = mkid->GetInstanceNameVar()->GetName();

            const char* subclass = mkid->GetClassNameVar()->GetName();
            if (mnamer->GetExport()) {
                out << "    " << subclass << "* " << mname << " = new ";
                out << subclass << "(\"" << instance << "\");\n";
                if (mkid->IsA(EDITOR_COMP)) {
                    out << "    unidraw->Open(" << mname << ");\n";
                } else {
                    out << "    w->InsertApplication(" << mname << ");\n";
                }
            } else {
                if (mkid->IsA(EDITOR_COMP)) {
                    out << "    unidraw->Open(new " << subclass;
                    out << "(\"" << instance << "\"));\n";
                } else {
                    out << "    w->InsertApplication(new " << subclass;
                    out << "(\"" << instance << "\"));\n";
                }
            }
        } else {
            const char* iname = icomp->GetInstanceNameVar()->GetName();
            out << "    w->InsertApplication(" << iname << "());\n";
        }
    }
    out << "    unidraw->Run();\n";
    out << "    delete unidraw;\n";
    out << "    return 0;\n}\n";

    if (!out.good()) {
	strcat(_errbuf, orig);
        strcat(_errbuf, " Main code generation failed.\n");
    }
    return out.good() && ok;
}

boolean CodeView::Scan (ClassId id) {
    GraphicComp* grcomp = (GraphicComp*) GetSubject();
    ScanCmd scanCmd(grcomp, _classname, id);
    scanCmd.Execute();
    return scanCmd.Succeeded();
}

void CodeView::BeginInstantiate (ostream& out) {
    InteractorComp* icomp = GetIntComp();
    const char* instance = icomp->GetMemberNameVar()->GetName();
    boolean export = icomp->GetMemberNameVar()->GetExport();
    const char* classname = icomp->GetClassNameVar()->GetName();

    if (export && !_emitMain) {
        out << "    " << instance << " = new ";
    } else {
        out << "    " << classname << "* ";
        out << instance << " = new ";
    }
    out << classname;
}

void CodeView::EndInstantiate (ostream& out) {
    out << ";\n";
}

void CodeView::InstanceName (ostream& out, const char* separator) {
    const char* instanceName = GetIntComp()->GetInstanceNameVar()->GetName();

    if (*instanceName != '\0' && separator != '\0') {
        out << "\"" << instanceName << "\"" << separator;
    }
}

CodeView* CodeView::CreateCodeView (GraphicComp* icomp) {
    CodeView* cv = (CodeView*) icomp->Create(CODE_VIEW);

    if (cv != nil) {
        icomp->Attach(cv);
        cv->Update();
    }
    return cv;
}

void CodeView::Update () {
    DeleteViews();

    GraphicComp* icomp = (GraphicComp*) GetSubject();
    Iterator i;

    for (icomp->First(i); !icomp->Done(i); icomp->Next(i)) {
        GraphicComp* kid = (GraphicComp*) icomp->GetComp(i);
        CodeView* cv = CreateCodeView(kid);

        if (cv != nil) {
            _views->Append(new UList(cv));
        }
    }
}

InteractorComp* CodeView::GetIntComp () { 
    GraphicComp* grcomp = (GraphicComp*) GetSubject();
    if (grcomp->IsA(INTERACTOR_COMP)) {
        return (InteractorComp*) GetSubject();
    } else {
        return nil;
    }
}

IComp* CodeView::GetIComp () {
    GraphicComp* grcomp = (GraphicComp*) GetSubject();
    if (!grcomp->IsA(INTERACTOR_COMP)) {
        return (IComp*) GetSubject();
    } else {
        return nil;
    }
}

CodeView* CodeView::View (UList* r) { return (CodeView*) (*r)(); }
UList* CodeView::Elem (Iterator i) { return (UList*) i.GetValue(); }
void CodeView::First (Iterator& i) { i.SetValue(_views->First()); }
void CodeView::Last (Iterator& i) { i.SetValue(_views->Last()); }
void CodeView::Next (Iterator& i) { i.SetValue(Elem(i)->Next()); }
void CodeView::Prev (Iterator& i) { i.SetValue(Elem(i)->Prev()); }
boolean CodeView::Done (Iterator i) { return Elem(i) == _views->End(); }
ExternView* CodeView::GetView (Iterator i) { return View(Elem(i)); }

void CodeView::SetView (ExternView* ev, Iterator& i) {
    i.SetValue(_views->Find(ev));
}

void CodeView::DeleteView (Iterator& i) {
    UList* doomed = Elem(i);
    ExternView* view = GetView(i);

    Next(i);
    _views->Remove(doomed);
    SetParent(view, nil);
    delete doomed;
    delete view;
}

void CodeView::DeleteViews () {
    Iterator i;
    First(i);

    while (!Done(i)) {
        DeleteView(i);
    }
}    

boolean CodeView::EmitBSDecls (CodeView* kid, ostream& out) {
    boolean ok = true;
    _emitBSDecls = true;
    ok = ok && kid->Definition(out);
    _emitBSDecls = false;

    if (!ok) {
        strcat(_errbuf, "ButtonState declaration failed. \n");
    }
    return ok;
}

boolean CodeView::EmitBSInits (CodeView* kid, ostream& out) {
    boolean ok = true;
    _emitBSInits = true;
    ok = ok && kid->Definition(out);
    _emitBSInits = false;

    if (!ok) {
        strcat(_errbuf, "ButtonState initialization failed. \n");
    }
    return ok;
}

boolean CodeView::EmitInstanceDecls (CodeView* kid, ostream& out) {
    boolean ok = true;
    _emitInstanceDecls = true;
    ok = ok && kid->Definition(out);
    _emitInstanceDecls = false;
    if (!ok) {
        strcat(_errbuf, "Instance declaration failed. \n");
    }
    return ok;
}

boolean CodeView::EmitInstanceInits(CodeView* kid, ostream& out) {
    boolean ok = true;
    _emitInstanceInits = true;
    _icomplete = true;

    while (ok) {
	UList* track = _instancelist->Last();
        ok = ok && kid->Definition(out);

        if (track == _instancelist->Last()) {
	    break;
	}
    }

    _emitInstanceInits = false;
    ok = ok && _icomplete;
    
    if (!ok) {
        strcat(_errbuf, "Instance instantiation failed.\n");
    }

    return ok;
}

boolean CodeView::EmitPropertyData(ostream& out) {
    _emitProperty = true;
    boolean ok = Iterate(out);
    _emitProperty = false;
    if (!ok) {
        strcat(_errbuf, "Property generation failed.\n");
    }
    return ok;
}

boolean CodeView::AllKidsDefined() {
    boolean success = true;
    InteractorComp* icomp = GetIntComp();
    Iterator i;

    for (icomp->First(i); !icomp->Done(i); icomp->Next(i)) {
	InteractorComp* kid = (InteractorComp*) icomp->GetComp(i);
	const char* instance = kid->GetMemberNameVar()->GetName();

	if (!_instancelist->Find((void*) instance)) {
	    success = false;
	    break;
	}
    }
    return success;
}

boolean CodeView::EmitCoreDecls(ostream& out) {
    _emitCoreDecls = true;
    CleanUp();
    boolean ok = Iterate(out);
    _emitCoreDecls = false;
    if (!ok) {
        strcat(_errbuf, "Core class declaration failed.\n");
    }
    return ok;
}
    
boolean CodeView::EmitFunctionDecls(CodeView* kid, ostream& out) {
    boolean ok = true;

    CleanUp();
    _emitFunctionDecls = true;
    ok = ok && kid->Definition(out);
   _emitFunctionDecls = false;

    if (!ok) {
        strcat(_errbuf, "Class function declaration failed.\n");
    }
    return ok;
}
    
boolean CodeView::EmitFunctionInits(CodeView* kid, ostream& out) {
    Iterator i;
    boolean ok = true;

    CleanUp();
    _emitFunctionInits = true;
    ok = ok && kid->Definition(out);
    _emitFunctionInits = false;

    out << "\n";
    ok = ok && out.good();
    if (!ok) {
        strcat(_errbuf, "Class function initialization failed.\n");
    }
    return ok;
}
    
boolean CodeView::EmitCoreInits(ostream& out) {
    _emitCoreInits = true;
    CleanUp();
    boolean ok = Iterate(out);
   _emitCoreInits = false;
    if (!ok) {
        strcat(_errbuf, "Core class initialization failed.\n");
    }
    return ok;
}

boolean CodeView::EmitExpHeader (ostream& out) {
    _emitExpHeader = true;
    CleanUp();
    boolean ok = Iterate(out);
    _emitExpHeader = false;
    return ok;
}

boolean CodeView::EmitCorehHeader (ostream& out) {
    _emitCorehHeader = true;
    CleanUp();
    boolean ok = Iterate(out);
    _emitCorehHeader = false;
    return ok;
}

void CodeView::GetCoreClassName (char* coreclass) {
    if (_classname != nil && *_classname != '\0') {
        sprintf(coreclass, "%s_core", _classname);
    } else {
        *coreclass = '\0';
    }
}

boolean CodeView::CoreConstDecls(ostream&) { return true; }
boolean CodeView::CoreConstInits(ostream&) { return true; }
boolean CodeView::ConstDecls(ostream&) { return true; }
boolean CodeView::ConstInits(ostream&) { return true; }


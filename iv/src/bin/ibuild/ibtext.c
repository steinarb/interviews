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
 * IText component definitions.
 */

#include "ibtext.h"
#include "ibclasses.h"
#include "ibcmds.h"
#include "ibgrblock.h"
#include "ibvars.h"
#include <Unidraw/clipboard.h>
#include <Unidraw/editor.h>
#include <Unidraw/iterator.h>
#include <Unidraw/manips.h>
#include <Unidraw/viewer.h>
#include <Unidraw/Graphic/picture.h>
#include <Unidraw/Graphic/pspaint.h>
#include <Unidraw/Tools/tool.h>
#include <InterViews/transformer.h>
#include <InterViews/painter.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/
static const char* textcomp_delim = "%textcomp_delim";
/*****************************************************************************/

class ITextGraphic : public Picture {
public:
    ITextGraphic();
    void GetTotalGS(FullGraphic* gs);
};

ITextGraphic::ITextGraphic () {}

void ITextGraphic::GetTotalGS (FullGraphic* gs) {
    Iterator i;

    totalGS(*gs);
    First(i);
    Graphic* gr = GetGraphic(i);
    concatGraphic(gr, gr, gs, gs);
}

/*****************************************************************************/
ITextComp::ITextComp (TextGraphic* graphic) : IComp (new ITextGraphic) {
    GetClassNameVar()->SetName("TextGraphic");
    GetClassNameVar()->SetBaseClass("TextGraphic");
    if (!_release || graphic != nil) {
        _target = new TextComp(graphic);
        if (graphic != nil) {
            Append(_target);
        }
    }
}

void ITextComp::Interpret (Command* cmd) {
    if (cmd->IsA(EDIT_CMD)) {
        EditCmd* editcmd = (EditCmd*) cmd;

        GraphicComp* target = editcmd->SwapComp(GetTarget());
        Append(target);

        Graphic* src = _target->GetGraphic();
        Graphic* dest = target->GetGraphic();
        src->Align(TopLeft, dest, TopLeft);

        Remove(_target);
        _target = target;
        Notify();
        GetGrBlockComp()->Propagate(cmd);

    } else {
        IComp::Interpret(cmd);
    }
}

ClassId ITextComp::GetSubstId(const char*& delim) {
    delim = textcomp_delim;
    return TEXT_COMP;
}

ClassId ITextComp::GetClassId () { return ITEXT_COMP; }

boolean ITextComp::IsA (ClassId id) {
    return ITEXT_COMP == id || IComp::IsA(id);
}

/*****************************************************************************/
ITextView::ITextView (ITextComp* icomp) : IView(icomp) {}

ITextComp* ITextView::GetITextComp () { return (ITextComp*) GetSubject(); }

Graphic* ITextView::GetGraphic () {
    Graphic* g = GraphicView::GetGraphic();
    
    if (g == nil) {
        g = new ITextGraphic;
        Iterator i;

        for (First(i); !Done(i); Next(i)) {
            g->Append(GetView(i)->GetGraphic());
        }
        SetGraphic(g);
    }
    return g;
}

Manipulator* ITextView::CreateManipulator (
    Viewer* v, Event& e, Transformer* rel, Tool* tool
) {
    Manipulator* m = nil;
    Editor* ed = v->GetEditor();
    int tabWidth = round(.5*inch);

    if (tool->IsA(RESHAPE_TOOL)) {
        FullGraphic gs;

        ITextComp* itext = GetITextComp();
        ITextGraphic* ipic = (ITextGraphic*) itext->GetGraphic();

        ipic->GetTotalGS(&gs);
        
        TextGraphic* textgr = (TextGraphic*) GetKidView()->GetGraphic();

        Painter* painter = new Painter;
        int lineHt = textgr->GetLineHeight();
        float xpos, ypos;
        rel = new Transformer;
        const char* text = textgr->GetOriginal();
        int size = strlen(text);
        
        textgr->TotalTransformation(*rel);
        rel->transform(0.0, 0.0, xpos, ypos);

        painter->FillBg(true);
        painter->SetFont(gs.GetFont());
	painter->SetColors(gs.GetFgColor(), gs.GetBgColor());
        painter->SetTransformer(rel);
        Unref(rel);

        m = new TextManip(
            v, text, size, (Coord)xpos, (Coord)ypos, 
            painter, lineHt, tabWidth, tool
        );

    } else {
        m = IView::CreateManipulator(v, e, rel, tool);
    }
    return m;
}

Command* ITextView::InterpretManipulator (Manipulator* m) {
    Viewer* v = m->GetViewer();
    Editor* ed = v->GetEditor();
    Tool* tool = m->GetTool();
    Command* cmd = nil;

    if (tool->IsA(RESHAPE_TOOL)) {
        TextManip* tm = (TextManip*) m;
        int lineHt = tm->GetLineHeight();
        int size;
        const char* text = tm->GetText(size);
        Graphic* pg = GetITextComp()->GetTarget()->GetGraphic();
        TextGraphic* textgr = new TextGraphic(text, lineHt, pg);

        cmd = new EditCmd(
            ed, new Clipboard(GetITextComp()), new TextComp(textgr)
        );

    } else {
        cmd = IView::InterpretManipulator(m);
    }

    return cmd;
}

/*****************************************************************************/

ClassId TextCode::GetClassId () { return ITEXT_CODE; }

boolean TextCode::IsA(ClassId id) {
    return ITEXT_CODE == id || CodeView::IsA(id);
}

TextCode::TextCode (ITextComp* subj) : CodeView(subj) {}

void TextCode::Update () {
    CodeView::Update();
    GetITextComp()->Bequeath();
}

ITextComp* TextCode::GetITextComp () {
    return (ITextComp*) GetSubject(); 
}

boolean TextCode::Definition (ostream& out) {
    boolean ok = true;

    ITextComp* textcomp = GetITextComp();
    TextComp* target = (TextComp*) textcomp->GetTarget();
    TextGraphic* textgr= target->GetText();
    SubclassNameVar* snamer = textcomp->GetClassNameVar();
    MemberNameVar* mnamer = textcomp->GetMemberNameVar();
    const char* mname = mnamer->GetName();
    const char* subclass = snamer->GetName();

    if (_emitGraphicState) {
        ok = WriteGraphicDecls(textgr, out);

    } else if (
        _emitInstanceDecls || _emitForward || 
        _emitClassHeaders || _emitHeaders
    ) {
        ok = CodeView::Definition(out);
        if (_emitInstanceDecls && _emitGraphicComp && !_emitExport) {
            out << "    TextComp* " << mname << "_comp;\n";
        }

    } else if (_emitExpHeader) {
        if (!snamer->IsSubclass()) {
            if (_scope && mnamer->GetExport()&&!_namelist->Search("text")) {
                _namelist->Append("text");
                out << "#include <Unidraw/Components/text.h> \n";
            }
        } else {
            ok = CodeView::Definition(out);
        }

    } else if (_emitCorehHeader) {
        if (snamer->IsSubclass() && strcmp(subclass, _classname) == 0) {
            if (!_namelist->Search("text")) {
                _namelist->Append("text");
                out << "#include <Unidraw/Components/text.h> \n";
            }
        }

    } else if (_emitInstanceInits) {
        const char* text = textgr->GetOriginal();
        int h = textgr->GetLineHeight();

        out << "    {\n";
        out << "        " << mname << " = new " << subclass << "(\"";
        out << text << "\", " << h << ");\n";
        ok = WriteGraphicInits(textgr, out);
        if (_emitGraphicComp) {
            out << "        " << mname << "_comp = new TextComp(";
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

boolean TextCode::CoreConstDecls(ostream& out) { 
    out << "(const char*, int h, Graphic* = nil);\n";
    return out.good();
}

boolean TextCode::CoreConstInits(ostream& out) {
    IComp* icomp = GetIComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    const char* name, int h, Graphic* gr\n) : ";
    out << baseclass << "(name, h, gr) {}\n\n";

    return out.good();
}

boolean TextCode::ConstDecls(ostream& out) {
    out << "(const char*, int h, Graphic* = nil);\n";
    return out.good();
}

boolean TextCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    const char* name, int h, Graphic* gr\n) : ";
    out << coreclass << "(name, h, gr) {}\n\n";

    return out.good();
}

boolean TextCode::EmitIncludeHeaders(ostream& out) {
    SubclassNameVar* snamer = GetIComp()->GetClassNameVar();

    if (!snamer->IsSubclass() && !_namelist->Search("text")) {
        _namelist->Append("text");
        out << "#include <Unidraw/Components/text.h> \n";
    }
    return out.good();
}




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
 * Implementation of PanelCtrl
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibcommandctrl.c,v 1.2 91/09/27 14:07:34 tang Exp $
 */

#include "ibclasses.h"
#include "ibtext.h"
#include "ibdialogs.h"
#include "ibed.h"
#include "ibeditor.h"
#include "ibgrcomp.h"
#include "ibvars.h"
#include "ibvarviews.h"
#include "ibcommandctrl.h"

#include <Unidraw/iterator.h>
#include <Unidraw/page.h>
#include <Unidraw/statevars.h>
#include <Unidraw/selection.h>
#include <Unidraw/viewer.h>
#include <Unidraw/ulist.h>
#include <Unidraw/unidraw.h>
#include <Unidraw/Tools/tool.h>
#include <Unidraw/Components/text.h>
#include <Unidraw/Commands/command.h>
#include <Unidraw/Graphic/graphic.h>

#include <InterViews/brush.h>
#include <InterViews/shape.h>
#include <InterViews/transformer.h>

#include <math.h>
#include <stream.h>
#include <string.h>

/*****************************************************************************/

static const int HPAD = 4;          // horizontal padding around control labels
static const int VPAD = 1;          // vertical padding around control labels
static const int SEP = 8;           // separation between label & equiv
static const int MINHT = 15;        // minimum height of control labels
static const int MINWD = 15;        // minimum width of control labels

/*****************************************************************************/

CommandCtrlComp::CommandCtrlComp (PanelCtrlGraphic* gr) : PanelCtrlComp(gr) { 
    if (gr != nil) {
        SubclassNameVar* subclass = GetClassNameVar();
        subclass->SetName("CommandControl");
        subclass->SetBaseClass("CommandControl");
    }
}

ClassId CommandCtrlComp::GetClassId () { return COMMANDCONTROL_COMP; }

boolean CommandCtrlComp::IsA (ClassId id) {
    return COMMANDCONTROL_COMP == id || PanelCtrlComp::IsA(id);
}

void CommandCtrlComp::Instantiate () {
    if (_toolname == nil) {
        _toolname = new TrackNameVar("NOPCmd");
    }
    PanelCtrlComp::Instantiate();
}

void CommandCtrlComp::Reconfig () {
    Shape* shape = GetShapeVar()->GetShape();
    int x0, y0, x1, y1;
    TextComp* textcomp = (TextComp*) GetKeyLabel()->GetTarget();
    TextGraphic* textgr = textcomp->GetText();
    const char* kl = textgr->GetOriginal();

    GetTop()->GetGraphic()->GetBox(x0, y0, x1, y1);
    shape->width = 2*HPAD + x1 - x0;
    shape->height = max(2*VPAD + y1 - y0, MINHT);

    if (*kl != '\0') {
	PSFont* f = stdgraphic->GetFont();
	shape->width += f->Width(kl) + SEP;
	shape->height = max(shape->height, f->Height() + 2*VPAD);
    }
    shape->Rigid(shape->width, hfil, 0, 0);
}

void CommandCtrlComp::Resize () {
    Iterator i;

    PanelCtrlGraphic* pcgr = GetPanelCtrlGraphic();
    TextComp* textcomp = (TextComp*) GetKeyLabel()->GetTarget();
    TextGraphic* textgr = textcomp->GetText();
    const char* kl = textgr->GetOriginal();

    Picture* topgr = (Picture*) GetTop()->GetGraphic();
    pcgr->SetPattern(psclear);
    
    if (*kl == '\0') {
        if (!topgr->IsEmpty()) {
            topgr->Align(Center, pcgr, Center);
        }
    } else {
        if (!topgr->IsEmpty()) {
            topgr->Translate(-HPAD, 0);
            topgr->Align(CenterLeft, pcgr, CenterLeft);
            topgr->Translate(HPAD, 0);
            pcgr->Align(CenterRight, textgr, CenterRight);
            textgr->Translate(-HPAD, 0);
        }
    }
    textcomp->Notify();
    SubNotify();
}

void CommandCtrlComp::Interpret(Command* cmd) {
    if (!cmd->IsA(GETCONFLICT_CMD)) {
        PanelCtrlComp::Interpret(cmd);
    }
}

void CommandCtrlComp::Uninterpret(Command* cmd) {
    if (!cmd->IsA(GETCONFLICT_CMD)) {
        PanelCtrlComp::Uninterpret(cmd);
    }
}

boolean CommandCtrlComp::IsRelatableTo (InteractorComp* comp) {
    boolean ok = false;
    if (comp->IsA(EDITOR_COMP) || comp->IsA(COMMANDCONTROL_COMP)) {
        ok = true;
    }
    return ok;
}

/*****************************************************************************/

CommandCtrlView::CommandCtrlView (
    CommandCtrlComp* subj
) : PanelCtrlView(subj) { _keylabel = nil; }


CommandCtrlComp* CommandCtrlView::GetCommandCtrlComp () {
    return (CommandCtrlComp*) GetSubject();
}

ClassId CommandCtrlView::GetClassId () { return COMMANDCONTROL_VIEW; }

boolean CommandCtrlView::IsA (ClassId id) {
    return COMMANDCONTROL_VIEW == id || PanelCtrlView::IsA(id);
}

GraphicComp* CommandCtrlView::CreateProtoComp (
    Editor* ed, Coord l, Coord b, Coord r, Coord t
) {
    ColorVar* colVar = (ColorVar*) ed->GetState("ColorVar");
    FontVar* fontVar = (FontVar*) ed->GetState("FontVar");
    
    float mcx, mcy, cx, cy;
    CommandCtrlComp* comComp = (CommandCtrlComp*) GetCommandCtrlComp()->Copy();
    Graphic* topgr = comComp->GetTop()->GetGraphic();
    mcx = (r-l)/2.0; mcy = (t-b)/2.0;

    topgr->SetColors(colVar->GetFgColor(), colVar->GetBgColor());
    topgr->SetFont(fontVar->GetFont());
    topgr->GetCenter(cx, cy);
    topgr->Translate(mcx-cx, mcy-cy);
    topgr->Translate(HPAD, 0);
    return comComp;
}

InfoDialog* CommandCtrlView::GetInfoDialog () {
    IBEditor* ibed = (IBEditor*) GetViewer()->GetEditor();
    InfoDialog* info = InteractorView::GetInfoDialog();
    ButtonState* state = info->GetState();
    CommandCtrlComp* comComp = GetCommandCtrlComp();

    IBNameVar* comName = comComp->GetToolName();
    MemberNameVar* edVar = comComp->GetEditorVar();

    NameChooserView* comChooser = new NameChooserView(
        comName, state, ibed,
        "Library commands: ", "Command Name: "
    );

    comChooser->Append("AlignCmd Bottom Bottom");
    comChooser->Append("AlignCmd Bottom Top");
    comChooser->Append("AlignCmd Center Center");
    comChooser->Append("AlignCmd HorizCenter HorizCenter");
    comChooser->Append("AlignCmd Left Left");
    comChooser->Append("AlignCmd Left Right");
    comChooser->Append("AlignCmd Right Left");
    comChooser->Append("AlignCmd Right Right");
    comChooser->Append("AlignCmd Top Bottom");
    comChooser->Append("AlignCmd Top Top");
    comChooser->Append("AlignCmd VertCenter VertCenter");
    comChooser->Append("AlignToGridCmd");
    comChooser->Append("BackCmd");
    comChooser->Append("BrushCmd");
    comChooser->Append("CenterCmd");
    comChooser->Append("CloseEditorCmd");
    comChooser->Append("ColorCmd fg");
    comChooser->Append("ColorCmd bg");
    comChooser->Append("CopyCmd");
    comChooser->Append("CutCmd");
    comChooser->Append("DeleteCmd");
    comChooser->Append("DupCmd");
    comChooser->Append("FontCmd");
    comChooser->Append("FrontCmd");
    comChooser->Append("GravityCmd");
    comChooser->Append("GridCmd");
    comChooser->Append("GridSpacingCmd");
    comChooser->Append("GroupCmd");
    comChooser->Append("ImportCmd");

    comChooser->Append("NewCompCmd");
    comChooser->Append("NormSizeCmd");
    comChooser->Append("OrientationCmd");
    comChooser->Append("PatternCmd");
    comChooser->Append("PasteCmd");
    comChooser->Append("PrintCmd");
    comChooser->Append("QuitCmd");
    comChooser->Append("RedoCmd");
    comChooser->Append("RedToFitCmd");
    comChooser->Append("RevertCmd");
    comChooser->Append("RotateCmd 90.0");
    comChooser->Append("RotateCmd -90.0");
    comChooser->Append("SaveCompCmd");
    comChooser->Append("SaveCompAsCmd");
    comChooser->Append("ScaleCmd -1.0 1.0");
    comChooser->Append("ScaleCmd 1.0 -1.0");
    comChooser->Append("SlctAllCmd");

    comChooser->Append("UndoCmd");
    comChooser->Append("UngroupCmd");
    comChooser->Append("ViewCompCmd");

    info->Include(new RelatedVarView(
        edVar, state, comComp, "Editor Name: ")
        
    );
    info->Include(comChooser);
    return info;
}

/*****************************************************************************/

CommandCtrlCode::CommandCtrlCode (
    CommandCtrlComp* subj
) : PanelCtrlCode(subj) { }

CommandCtrlComp* CommandCtrlCode::GetCommandCtrlComp() {
    return (CommandCtrlComp*) GetSubject();
}

ClassId CommandCtrlCode::GetClassId () { return COMMANDCONTROL_CODE; }

boolean CommandCtrlCode::IsA(ClassId id) {
    return COMMANDCONTROL_CODE == id || PanelCtrlCode::IsA(id);
}

boolean CommandCtrlCode::Definition (ostream& out) {
    char coreclass[CHARBUFSIZE];
    char Keycode[16], Com[16], Arg1[16], Arg2[16];
    boolean ok = true;
    Iterator i;

    CommandCtrlComp* ctrlComp = GetCommandCtrlComp();
    SubclassNameVar* snamer = ctrlComp->GetClassNameVar();
    MemberNameVar* mnamer = ctrlComp->GetMemberNameVar();
    TextComp* textcomp = (TextComp*) ctrlComp->GetKeyLabel()->GetTarget();
    TextGraphic* textgr = textcomp->GetText();
    MemberNameVar* edVar = ctrlComp->GetEditorVar();
    const char* edname = edVar->GetName();

    const char* text = textgr->GetOriginal();
    const char* com = ctrlComp->GetToolName()->GetName();

    const char* subclass = snamer->GetName();
    const char* baseclass = snamer->GetBaseClass();
    const char* mname = mnamer->GetName();
    strcpy(Keycode, text);
    HashKeyCode(Keycode);
    InteractorComp* dummy;

    GetCoreClassName(coreclass);
    if (*edname == '\0') {
        strcat(_errbuf, mname);
        strcat(_errbuf, " has undefined Editor.\n");
        return false;

    } else if (!Search(edVar, dummy)) {
        strcat(_errbuf, mname);
        strcat(
            _errbuf, "'s Editor is not in the same hierachy.\n"
        );
        return false;
    }

    _emitGraphicComp = true;

    if (_emitInstanceInits) {
        if (!_instancelist->Find((void*)mname)) {
            _instancelist->Append(new UList((void*)mname));

            ok = ok && EmitGraphicState(out);

            for(First(i); !Done(i); Next(i)) {
                CodeView* kid = (CodeView*) GetView(i);
                ok = ok && EmitInstanceDecls(kid, out);
            }
            ok = ok && Iterate(out);

            const char* kidname = nil;

            if (!SingleKid()) {
                out << "    GraphicComps* " << mname;
                out << "_comp = new GraphicComps;\n";
                
                for(First(i); !Done(i); Next(i)) {
                    CodeView* kid = (CodeView*) GetView(i);
                    MemberNameVar* kmnamer=kid->GetIComp()->GetMemberNameVar();
                    const char* kmname = kmnamer->GetName();
                    out << "    " << mname << "_comp->Append(";
                    out << kmname << "_comp);\n";
                }
                kidname = mname;

            } else {
                First(i);
                CodeView* kidv = (CodeView*) GetView(i);
                MemberNameVar* kmnamer=kidv->GetIComp()->GetMemberNameVar();
                kidname = kmnamer->GetName();
            }
            out << "    ControlInfo* " << mname;
            out << "_info = new ControlInfo(";

            *Arg1 = '\0';
            *Arg2 = '\0';
	    /* some sscanfs need writable string */
	    char* writable_com = strnew(com);
            sscanf(writable_com, "%s%s%s", Com, Arg1, Arg2);
	    delete writable_com;
            
            out << kidname << "_comp, \"" << text << "\", \"";
            out << Keycode << "\");\n";
            
            out << "    Command* " << mname << "_com = new ";
            
            if (*com == '\0') {
                strcat(_errbuf, mname);
                strcat(_errbuf, " has undefined Command.\n");
                return false;
            }
            
            if (strcmp(Com, "NewCompCmd") == 0) {
                out << Com << "(" << mname << "_info, new GraphicComps);\n";

            } else if (
                strcmp(Com, "ScaleCmd") == 0 ||
                strcmp(Com, "AlignCmd") == 0
            ) {
                out << Com << "(" << mname << "_info, ";
                out << Arg1 << ", " << Arg2 << ");\n";

            } else if (strcmp(Com, "RotateCmd") == 0) {
                out << Com << "(" << mname << "_info, ";
                out << Arg1 << ");\n";

            } else if (strcmp(Com, "FontCmd") == 0) {
                out << Com << "(\n        " << mname << "_info, ";
                out << kidname << "_comp->GetGraphic()->GetFont()\n    );\n";

            } else if (strcmp(Com, "BrushCmd") == 0) {
                out << Com << "(\n        " << mname << "_info, ";
                out << kidname << "_comp->GetGraphic()->GetBrush()\n    );\n";

            } else if (strcmp(Com, "PatternCmd") == 0) {
                out << Com << "(\n        " << mname << "_info, ";
                out << kidname <<"_comp->GetGraphic()->GetPattern()\n    );\n";

            } else if (strcmp(Com, "ColorCmd") == 0) {
                out << Com << "(" << mname << "_info, ";
                if (strcmp(Arg1, "fg") == 0) {
                    out << kidname;
                    out << "_comp->GetGraphic()->GetFgColor(), nil\n    );\n";

                } else if (strcmp(Arg1, "bg") == 0) {
                    out << "nil, " << kidname;
                    out << "_comp->GetGraphic()->GetBgColor()\n    );\n";

                } else {
                    out << kidname << "_comp->GetGraphic()->GetFgColor(), ";
                    out << kidname;
                    out << "_comp->GetGraphic()->GetBgColor()\n    );\n";
                }

            } else {
                out << Com << "(" << mname << "_info);\n";
            }

            BeginInstantiate(out);
            out << "(";
            InstanceName(out);
            out << mname << "_info)";
            EndInstantiate(out);
            out << "    GetKeyMap()->Register(";
            out << mname << ");\n";
            out << "    " << mname << "_com->SetEditor(this);\n";
        }

    } else if (_emitBSDecls || _emitBSInits) {
        ok = true;

    } else {
	ok = ok && PanelCtrlCode::Definition(out);
    }
    _emitGraphicComp = true;

    return out.good() && ok;
}

boolean CommandCtrlCode::CoreConstDecls(ostream& out) { 
    out << "(const char*, ControlInfo*);\n";
    return out.good();
}

boolean CommandCtrlCode::CoreConstInits(ostream& out) {
    InteractorComp* icomp = GetIntComp();
    SubclassNameVar* snamer = icomp->GetClassNameVar();
    const char* baseclass = snamer->GetBaseClass();

    out << "(\n    const char* name, ControlInfo* info";
    out << "\n) : ";
    out << baseclass << "(name, info) {\n";
    out << "    perspective = new Perspective;\n";
    out << "}\n\n";

    return out.good();
}

boolean CommandCtrlCode::ConstDecls(ostream& out) {
    out << "(const char*, ControlInfo*);\n";
    return out.good();
}

boolean CommandCtrlCode::ConstInits(ostream& out) {
    char coreclass[CHARBUFSIZE];
    GetCoreClassName(coreclass);

    out << "(\n    const char* name, ControlInfo* info";
    out << "\n) : ";
    out << coreclass << "(name, info) {}\n\n";

    return out.good();
}

boolean CommandCtrlCode::EmitIncludeHeaders(ostream& out) {
    GrBlockCode::EmitIncludeHeaders(out);
    CommandCtrlComp* ctrlComp = GetCommandCtrlComp();
    const char* com = ctrlComp->GetToolName()->GetName();
    SubclassNameVar* snamer = ctrlComp->GetClassNameVar();


    if (!snamer->IsSubclass() && !_namelist->Search("uctrls")) {
        _namelist->Append("uctrls");
        out << "#include <Unidraw/uctrls.h> \n\n";
    }
    if (strcmp(snamer->GetName(), _classname) != 0) {
        if (!_namelist->Search("ctrlinfo")) {
            _namelist->Append("ctrlinfo");
            out << "#include <Unidraw/ctrlinfo.h> \n";
        }
        if (!_namelist->Search("grcomp")) {
            _namelist->Append("grcomp");
            out << "#include <Unidraw/Components/grcomp.h>\n";
        }
        if (!_namelist->Search("catcmds")) {
            _namelist->Append("catcmds");
            out << "#include <Unidraw/Commands/catcmds.h>\n";
        }
        if (!_namelist->Search("edit")) {
            _namelist->Append("edit");
            out << "#include <Unidraw/Commands/edit.h>\n";
        }
        if (!_namelist->Search("transforms")) {
            _namelist->Append("transforms");
            out << "#include <Unidraw/Commands/transforms.h>\n";
        }
        if (!_namelist->Search("viewcmds")) {
            _namelist->Append("viewcmds");
            out << "#include <Unidraw/Commands/viewcmds.h>\n";
        }
        if (strncmp(com, "ScaleCmd", 8) == 0 || strncmp(com, "RotateCmd", 9)== 0) {
            if (!_namelist->Search("struct")) {
                _namelist->Append("struct");
                out << "#include <Unidraw/Commands/struct.h>\n";
            }
        }  
        if (strcmp(com, "ImportCmd") == 0) {
            if (!_namelist->Search("import")) {
                _namelist->Append("import");
                out << "#include <Unidraw/Commands/import.h>\n";
            } 
        }
        if (strcmp(com, "FontCmd") == 0) {
            if (!_namelist->Search("font")) {
                _namelist->Append("font");
                out << "#include <Unidraw/Commands/font.h>\n";
            } 
        }
        if (strcmp(com, "BrushCmd") == 0) {
            if (!_namelist->Search("brushcmd")) {
                _namelist->Append("brushcmd");
                out << "#include <Unidraw/Commands/brushcmd.h>\n";
            } 
        }
        if (strcmp(com, "PatternCmd") == 0) {
            if (!_namelist->Search("patcmd")) {
                _namelist->Append("patcmd");
                out << "#include <Unidraw/Commands/patcmd.h>\n";
            } 
        }
        if (strncmp(com, "ColorCmd", 8) == 0) {
            if (!_namelist->Search("colorcmd")) {
                _namelist->Append("colorcmd");
                out << "#include <Unidraw/Commands/colorcmd.h>\n";
            } 
        }
        if (strcmp(com, "AlignToGridCmd") == 0 || strncmp(com, "AlignCmd", 8)==0) {
            if (!_namelist->Search("align")) {
                _namelist->Append("align");
                out << "#include <Unidraw/Commands/align.h>\n";
            } 
        }
    }

    return out.good();
}


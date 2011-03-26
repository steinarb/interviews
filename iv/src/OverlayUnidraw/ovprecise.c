/*
 * Copyright (c) 1998 Vectaport Inc.
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
 * precise commands
 */

#include <OverlayUnidraw/ovprecise.h>
#include <OverlayUnidraw/ovclasses.h>
#include <Unidraw/Commands/transforms.h>
#include <IVGlyph/enumform.h>
#include <IVGlyph/stredit.h>
#include <Unidraw/editor.h>
#include <InterViews/window.h>
#include <stdio.h>
#include <string.h>
#include <strstream.h>

/*****************************************************************************/

char* OvPreciseMoveCmd::_default_movestr = nil;
int OvPreciseMoveCmd::_default_enumval = 0;

ClassId OvPreciseMoveCmd::GetClassId () { return OVPRECISEMOVE_CMD; }

boolean OvPreciseMoveCmd::IsA (ClassId id) {
    return OVPRECISEMOVE_CMD == id || PreciseMoveCmd::IsA(id);
}

OvPreciseMoveCmd::OvPreciseMoveCmd (ControlInfo* c) : PreciseMoveCmd(c) {}
OvPreciseMoveCmd::OvPreciseMoveCmd (Editor* ed) : PreciseMoveCmd(ed) {}
OvPreciseMoveCmd::~OvPreciseMoveCmd () {}

Command* OvPreciseMoveCmd::Copy () {
    Command* copy = new OvPreciseMoveCmd(CopyControlInfo());
    InitCopy(copy);
    return copy;
}

Glyph* OvPreciseMoveCmd::unit_buttons() {
    StringList* list = new StringList;
    String* str_i;
    str_i = new String("Pixels");
    list->append(*str_i);
    str_i = new String("Points");
    list->append(*str_i);
    str_i = new String("Centimeters");
    list->append(*str_i);
    str_i = new String("Inches");
    list->append(*str_i);
    _unit_enum = new ObservableEnum(list);
    _unit_enum->setvalue(_default_enumval);
    return new RadioEnumEditor(_unit_enum, "Units", true, true);
}

void OvPreciseMoveCmd::Execute () {
    if (!_default_movestr) 
      _default_movestr = strdup("1.0 1.0");
    char* movestr = 
      StrEditDialog::post(GetEditor()->GetWindow(),
			  "Enter X and Y movement:",
			  _default_movestr, nil, unit_buttons());

    int cur_unit = _unit_enum->intvalue();
    _default_enumval = cur_unit;

    if (movestr) {
      istrstream in(movestr);
      float xmove, ymove;
      in >> xmove >> ymove;

      switch (cur_unit) {
      case 1:   xmove *= ivpoints; ymove *= ivpoints; break;
      case 2:   xmove *= ivcm; ymove *= ivcm; break;
      case 3:   xmove *= ivinches; ymove *= ivinches; break;
      }

      if (in.good() && (xmove!=0.0 || ymove!=0.0)) {
	MoveCmd* moveCmd = new MoveCmd(GetEditor(), xmove, ymove);
	moveCmd->Execute();
	moveCmd->Log();
      }
      delete _default_movestr;
      _default_movestr = movestr;
    }
}

/*****************************************************************************/

ClassId OvPreciseScaleCmd::GetClassId () { return OVPRECISESCALE_CMD; }

boolean OvPreciseScaleCmd::IsA (ClassId id) {
    return OVPRECISESCALE_CMD == id || PreciseScaleCmd::IsA(id);
}

OvPreciseScaleCmd::OvPreciseScaleCmd (ControlInfo* c) : PreciseScaleCmd(c) {}
OvPreciseScaleCmd::OvPreciseScaleCmd (Editor* ed) : PreciseScaleCmd(ed) {}
OvPreciseScaleCmd::~OvPreciseScaleCmd () {}

Command* OvPreciseScaleCmd::Copy () {
    Command* copy = new OvPreciseScaleCmd(CopyControlInfo());
    InitCopy(copy);
    return copy;
}

void OvPreciseScaleCmd::Execute () {
    static char* default_scalestr = strdup("1.0 1.0");
    char* scalestr = 
      StrEditDialog::post(GetEditor()->GetWindow(),
			  "Enter X and Y scaling:",
			  default_scalestr);
    if (scalestr) {
      istrstream in(scalestr);
      float xscale, yscale;
      in >> xscale >> yscale;
      if (in.good() && xscale !=0.0 && yscale != 0.0) {
	ScaleCmd* scaleCmd = new ScaleCmd(GetEditor(), xscale, yscale);
	scaleCmd->Execute();
	scaleCmd->Log();
      }
      delete default_scalestr;
      default_scalestr = scalestr;
    }
}

/*****************************************************************************/

ClassId OvPreciseRotateCmd::GetClassId () { return OVPRECISEROTATE_CMD; }

boolean OvPreciseRotateCmd::IsA (ClassId id) {
    return OVPRECISEROTATE_CMD == id || PreciseRotateCmd::IsA(id);
}

OvPreciseRotateCmd::OvPreciseRotateCmd (ControlInfo* c) : PreciseRotateCmd(c) {}
OvPreciseRotateCmd::OvPreciseRotateCmd (Editor* ed) : PreciseRotateCmd(ed) {}
OvPreciseRotateCmd::~OvPreciseRotateCmd () {}

Command* OvPreciseRotateCmd::Copy () {
    Command* copy = new OvPreciseRotateCmd(CopyControlInfo());
    InitCopy(copy);
    return copy;
}

void OvPreciseRotateCmd::Execute () {
    static char* default_rotatestr = strdup("45.0");
    char* rotatestr = 
      StrEditDialog::post(GetEditor()->GetWindow(),
			  "Enter rotation in degrees:",
			  default_rotatestr);
    if (rotatestr) {
      istrstream in(rotatestr);
      float angle;
      in >> angle;
      if (in.good() && angle!=0.0) {
	RotateCmd* rotateCmd = new RotateCmd(GetEditor(), angle);
	rotateCmd->Execute();
	rotateCmd->Log();
      }
      delete default_rotatestr;
      default_rotatestr = rotatestr;
    }
}


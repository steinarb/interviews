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
 * Implementation of user interface builder-specific state variable views.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibvarviews.c,v 1.2 91/09/27 14:12:22 tang Exp $
 */

#include "ibadjuster.h"
#include "ibbutton.h"
#include "ibclasses.h"
#include "ibcmds.h"
#include "ibcomp.h"
#include "ibdialogs.h"
#include "ibed.h"
#include "ibmenu.h"
#include "ibgrblock.h"
#include "ibvars.h"
#include "ibvarviews.h"

#include <Unidraw/iterator.h>
#include <Unidraw/ulist.h>

#include <InterViews/box.h>
#include <InterViews/border.h>
#include <InterViews/button.h>
#include <InterViews/deck.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/message.h>
#include <InterViews/matcheditor.h>
#include <InterViews/scene.h>
#include <InterViews/scroller.h>
#include <InterViews/scrollbar.h>
#include <InterViews/strbrowser.h>
#include <InterViews/strchooser.h>
#include <InterViews/tray.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************************/

static char buf[CHARBUFSIZE];

/*****************************************************************************/

static const char* IntToString (int i) {
    sprintf(buf, "%d", i);
    return strnew(buf);
}

static Message* IntToMsg (int i) {
    sprintf(buf, "%d", i);
    return new Message(buf);
}

inline Interactor* PaddedFrame (Interactor* i) {
    return new Frame(new MarginFrame(i, 2));
}

inline Interactor* RightJustified (Interactor* i) {
    return new HBox(new HGlue(0, 0, 500), i);
}

inline Interactor* LeftJustified (Interactor* i) {
    return new HBox(i, new HGlue);
}

/*****************************************************************************/

IBVarView::IBVarView (
    StateVar* stateVar, GraphicComp* icomp
) : StateVarView(stateVar) {
    _icomp = icomp;
}

boolean IBVarView::ChangedSubject(const char*& errors) {
    errors = nil;
    return true;
}

void IBVarView::SetAffectedStates(UList* ilist) {
    StateVar* subject = GetSubject();
    UList* target = ilist->Find(subject);
    if (target == nil) {
        ilist->Append(new UList(subject));
    }
}

/*****************************************************************************/

CanvasVarView::CanvasVarView (CanvasVar* canvasVar) : IBVarView(canvasVar){}

void CanvasVarView::Init () {
    CanvasVar* cvar = (CanvasVar*) _subject;
    const char* string = "Canvas Dimensions (width x height): %d x %d";

    sprintf(buf, string, cvar->Width(), cvar->Height());
    Insert(new HBox(new Message(buf, Left), new HGlue));
}

/*****************************************************************************/

class InfoButtonState : public ButtonState {
public:
    InfoButtonState(int, BSDialog* = nil, IBEditor* = nil);
    virtual void Notify ();
private:
    BSDialog* _dialog;
    IBEditor* _ed;
};

InfoButtonState::InfoButtonState (
    int i, BSDialog* dialog, IBEditor* ed
) : ButtonState(i) {
    _dialog = dialog;
    _ed = ed;
}

void InfoButtonState::Notify () {
    ButtonState::Notify();
    int value;
    GetValue(value);
    if (value == 1) {
        if (_dialog->Init()) {
            _ed->InsertDialog(_dialog);
            while(_dialog->Accept() && !_dialog->ChangeBS());
            _ed->RemoveDialog(_dialog);
        }
        SetValue(0);
    }
}       

/*****************************************************************************/
        
ButtonStateVarView::ButtonStateVarView (
    ButtonStateVar* bsv, ButtonState* bs, GraphicComp* icomp, IBEditor* ibed
) : IBVarView(bsv, icomp) {
    _showsetting = bsv->DisplaySetting();
    _bsdialog = new BSDialog(this);
    _ibed = ibed;
    _namechange = false;
    _bsname = (ButtonSharedName*) bsv->GetButtonSharedName()->Copy();
    Insert(Interior(bs));
}

ButtonStateVarView::~ButtonStateVarView () {
    delete _bsdialog;
    delete _bsname;
}

static ButtonSharedName* FindFirstConflict(UList* ulist, const char* name) {
    ButtonSharedName* bsname = nil;
    for (UList* i = ulist->First(); i != ulist->End(); i = i->Next()) {
        StateVar* state = (StateVar*) (*i)();
        if (state->IsA(BUTTONSHAREDNAME)) {
            ButtonSharedName* bstmp = (ButtonSharedName*) state;
            if (strcmp(bstmp->GetName(), name) == 0) {
                bsname = (ButtonSharedName*) state;
                break;
            }
        }
    }
    return bsname;
}

void ButtonStateVarView::SetAffectedStates(UList* ilist) {
    IBVarView::SetAffectedStates(ilist);
    ButtonStateVar* button = (ButtonStateVar*) GetSubject();
    ButtonSharedName* bsname = button->GetButtonSharedName();
    ilist->Append(new UList(bsname));
}

boolean ButtonStateVarView::ChangedSubject (const char*& errors) {
    errors = nil;

    if (*_bs->Text() != '\0') {
        ButtonStateVar* button = (ButtonStateVar*) GetSubject();
        ButtonSharedName* bsname = button->GetButtonSharedName();

        GetFirewallCmd firewallCmd(_icomp);
        firewallCmd.Execute();
        GetConflictCmd conflictCmd(firewallCmd.GetFirewall(), _bs->Text());
        conflictCmd.Execute();
        UList* ulist = conflictCmd.GetConflict();
        for (UList* i = ulist->First(); i != ulist->End(); i = i->Next()) {
	    StateVar* state = (StateVar*) (*i)();
	    if (state->IsA(BUTTONSHAREDNAME)) {
                ButtonSharedName* bstmp = (ButtonSharedName*) state;
                if (strcmp(bstmp->GetFuncName(), _bs->Text()) == 0) {
                    sprintf(
                        buf,"ButtonState name %s has been used", 
                        _bs->Text()
                    );
                    errors = buf;
                    return false;
                }
	    } else if (!state->IsA(INSTANCENAME_VAR)) {
		sprintf(
		    buf,"ButtonState name %s has been used", _bs->Text()
		);
        	errors = buf;
		return false;
            }
  	}
        if (_showsetting && _setting->Text() != '\0') {
            int setting = atoi(_setting->Text());
            button->SetSetting(setting);
        }
        if (strcmp(_bsname->GetName(), bsname->GetName()) == 0) {
            bsname->SetInitial(_bsname->GetInitial());
            bsname->SetExport(_bsname->GetExport());
            bsname->SetFuncName(_bsname->GetFuncName());
            if (strcmp(_bs->Text(), bsname->GetName()) != 0) {
                bsname = FindFirstConflict(ulist, _bs->Text());
                if (bsname != nil) {
                    button->SetButtonSharedName(bsname);
                } else {
                    bsname = new ButtonSharedName(_bs->Text(), "", false);
                    button->SetButtonSharedName(bsname);
                }
            }
	} else {
            if (strcmp(_bs->Text(), _bsname->GetName()) != 0) {
                if (_namechange) {
                    bsname->SetMachGen(false);
                    bsname->SetName(_bsname->GetName());
                    bsname->SetInitial(_bsname->GetInitial());
                    bsname->SetExport(_bsname->GetExport());
                    bsname->SetFuncName(_bsname->GetFuncName());
                }
                bsname = FindFirstConflict(ulist, _bs->Text());
                if (bsname != nil) {
                    button->SetButtonSharedName(bsname);
                } else {
                    bsname = new ButtonSharedName(_bs->Text(), "", false);
                    button->SetButtonSharedName(bsname);
                }
            } else {
                if (_namechange) {
                    bsname->SetMachGen(false);
                    bsname->SetName(_bsname->GetName());
                    bsname->SetInitial(_bsname->GetInitial());
                    bsname->SetExport(_bsname->GetExport());
                    bsname->SetFuncName(_bsname->GetFuncName());
                } else {
                    bsname = FindFirstConflict(ulist, _bs->Text());
                    if (bsname != nil) {
                        button->SetButtonSharedName(bsname);
                    } else {
                        bsname = new ButtonSharedName(_bs->Text(), "", false);
                        button->SetButtonSharedName(bsname);
                    }
                    bsname->SetMachGen(false);
                    bsname->SetInitial(_bsname->GetInitial());
                    bsname->SetExport(_bsname->GetExport());
                    bsname->SetFuncName(_bsname->GetFuncName());
                }
            }
        }
    
    } else {
	sprintf(buf, "Null ButtonState name is invalid");
        errors = buf;
    }
    return errors == nil;
}

void ButtonStateVarView::Init () {
    ButtonStateVar* subj = (ButtonStateVar*) GetSubject();
    int setting = subj->GetSetting();
    ButtonSharedName* bsname = subj->GetButtonSharedName();

    if (bsname != nil) { 
        _bs->Message(bsname->GetName());
    }
    if (_showsetting) {
        _setting->Message(IntToString(setting));
    }
}

Interactor* ButtonStateVarView::Interior (ButtonState* bs) {
    const char* sample = "999";
    const int gap = round(.1*cm);

    _bs = new MatchEditor(bs, "a rather long name");
    _bs->Match("%[_a-zA-Z]%[_a-zA-Z0-9_]", true);

    Message* nameMsg = new Message("ButtonState Name: ");

    PushButton* pbutton = new PushButton(
        "ButtonState...", new InfoButtonState(0, _bsdialog, _ibed), 1
    );
    Interactor* bsname= PaddedFrame(_bs);

    HBox* namer = new HBox(
        bsname,
        new HGlue(2*gap, 0, 0),
        pbutton
    );
    namer->Align(Center);

    Tray* t = new Tray;

    t->HBox(t, nameMsg, namer, t);
    if (_showsetting) {
        _setting = new MatchEditor(bs, sample);
        _setting->Match("%d", true);

        Interactor* setter = new Frame(new MarginFrame(_setting, 2));
        Message* settingMsg = new Message("Setting Value: ");

        t->HBox(
            t,settingMsg,new HGlue(2*gap,0,0),setter,new HGlue(0,0,100*hfil),t
        );
        t->Align(VertCenter, settingMsg, setter);
	t->Align(Left, nameMsg, settingMsg);
	t->Align(Left, namer, setter);
	t->VBox(t, namer, new VGlue(gap), setter, new VGlue(gap), t);
	t->VBox(
            t, new VGlue(gap), nameMsg, new VGlue(gap), 
            settingMsg, new VGlue(gap), t
        );
    } else {
        t->VBox(t, new VGlue(gap), nameMsg, new VGlue(gap), t);
    }

    return t;
}

/*****************************************************************************/

CtrlStateVarView::CtrlStateVarView (
    ButtonStateVar* bsv, ButtonState* bs, GraphicComp* icomp, IBEditor* ibed
) : ButtonStateVarView(bsv, bs, icomp, ibed) {
    delete _bsdialog;
    _bsdialog = new CtrlDialog(this);
    Insert(Interior(bs));
}

Interactor* CtrlStateVarView::Interior (ButtonState* bs) {
    const char* sample = "999";
    const int gap = round(.1*cm);

    _bs = new MatchEditor(bs, "a rather long name");
    _bs->Match("%[_a-zA-Z]%[_a-zA-Z0-9_]", true);

    Message* nameMsg = new Message("ControlState Name: ");

    PushButton* pbutton = new PushButton(
        "ControlState...", new InfoButtonState(0, _bsdialog, _ibed), 1
    );
    Interactor* bsname= PaddedFrame(_bs);

    HBox* namer = new HBox(
        bsname,
        new HGlue(2*gap, 0, 0),
        pbutton
    );
    namer->Align(Center);

    Tray* t = new Tray;

    t->HBox(t, nameMsg, namer, t);
    t->Align(VertCenter, nameMsg, namer);

    return t;
}

/*****************************************************************************/

InstanceNameVarView::InstanceNameVarView (
    InstanceNameVar* inv, ButtonState* bs, GraphicComp* icomp, 
    const char* msg
) : IBVarView(inv, icomp) {
    _msg = strnew(msg);
    Insert(Interior(bs));
}

InstanceNameVarView::~InstanceNameVarView () { delete _msg; }

boolean InstanceNameVarView::ChangedSubject (const char*&) {
    const char* name = _name->Text(); 
    InstanceNameVar* iname = (InstanceNameVar*) GetSubject();
    
    if (strcmp(iname->GetName(), name) != 0) {
        iname->SetName(name);
        iname->SetMachGen(false);
    }
    return true;
}

void InstanceNameVarView::Init () { 
    InstanceNameVar* iname = (InstanceNameVar*) GetSubject();
    const char* name = iname->GetName();
    if( name != nil) {
	_name->Message(name);
    }
}

Interactor* InstanceNameVarView::Interior (ButtonState* bs) {
    const int gap = round(.1*cm);

    HBox* hbox = new HBox(
        new Message(_msg),
        PaddedFrame(_name = new MatchEditor(bs, "a rather long name"))
    );
    hbox->Align(Center);
    _name->Match("%[_a-zA-Z]%[_a-zA-Z0-9_.]", true);
    return hbox;
}

/*****************************************************************************/

class NameChooserDialog : public StringChooser {
public:
    NameChooserDialog(
        ButtonState*, int rows, int cols, const char*, const char*
    );
    void Append(const char*);
private:
    Interactor* AddScroller(Interactor* i);
    Interactor* Interior(const char*);
};

NameChooserDialog::NameChooserDialog(
    ButtonState* bs, int rows, int cols, const char* sample, const char* title
) : StringChooser(bs, rows, cols, sample) {
    Insert(Interior(title));
}

void NameChooserDialog::Append(const char* string) {
    _browser->Append(string);
}

Interactor* NameChooserDialog::AddScroller(Interactor* i) {
    return new HBox(
        new MarginFrame(i, 2),
        new VBorder,
	new VScrollBar(i)
    );
}

Interactor* NameChooserDialog::Interior(const char* title) {
    const int space = round(.5*cm);
    HBox* titleblock = new HBox(
        new MarginFrame(new class Message(title)),
        new HGlue
    );

    return new MarginFrame(
        new VBox(
            titleblock,
            new VGlue(space, 0),
            new Frame(new MarginFrame(_sedit, 2)),
            new VGlue(space, 0),
            new Frame(AddScroller(_browser)),
            new VGlue(space, 0),
            new HBox(
                new VGlue(space, 0),
                new HGlue,
                new PushButton("Cancel", state, '\007'),
                new HGlue(space, 0),
                new PushButton("  OK  ", state, '\r')
            )
        ), space, space/2, 0
    );
}

/*****************************************************************************/

class NameChooserState : public ButtonState {
public:
    NameChooserState(int, NameChooserView*);
    virtual void Notify ();

private:
    NameChooserView* _chooserView;
};

NameChooserState::NameChooserState (
    int i, NameChooserView* chooserView
) : ButtonState(i) {
    _chooserView = chooserView;
}

void NameChooserState::Notify () {
    ButtonState::Notify();
    int value;
    GetValue(value);
    if (value == 1) {
        _chooserView->Accept();
        SetValue(0);
    }
}       

/*****************************************************************************/

NameChooserView::NameChooserView (
    IBNameVar* nvar, ButtonState* bs,
    IBEditor* ibed, const char* title, const char* msg 
) : IBVarView(nvar, nil) {
    _msg = strnew(msg);
    _chooser = new NameChooserDialog(
        new ButtonState, 10, 24, nvar->GetName(), title
    );
    _ibed = ibed;
    Insert(Interior(bs));
}
    
NameChooserView::~NameChooserView () { 
    delete _chooser;
    delete _msg;
}

void NameChooserView::Append(const char* msg) {
    _chooser->Append(msg);
}

void NameChooserView::Accept () {
    boolean ok;
    _ibed->InsertDialog(_chooser);
    ok = _chooser->Accept();
    _ibed->RemoveDialog(_chooser);
    if (ok) {
        _name->Message(_chooser->Choice());
    }
}

boolean NameChooserView::ChangedSubject (const char*&) {
    const char* name = _name->Text(); 
    IBNameVar* chooser = (IBNameVar*) GetSubject();

    if (name != nil) {
	chooser->SetName(name);
        chooser->SetMachGen(false);
    }
    return true;
}

void NameChooserView::Init () { 
    IBNameVar* chooser = (IBNameVar*) GetSubject();
    const char* name = chooser->GetName();
    if( name != nil) {
	_name->Message(name);
    }
}

Interactor* NameChooserView::Interior (ButtonState* bs) {
    const int gap = round(.1*cm);

    HBox* hbox = new HBox(
        new Message(_msg),
        PaddedFrame(_name = new MatchEditor(bs, "a rather long name")),
	new HGlue(2*gap, 0, 0),
        new PushButton("Library...", new NameChooserState(0, this), 1)
    );
    hbox->Align(Center);

    return hbox;
}

/*****************************************************************************/

MemberNameVarView::MemberNameVarView (
    MemberNameVar* inv, ButtonState* bs, GraphicComp* icomp, const char* msg
) : IBVarView(inv, icomp) {
    MemberNameVar* iname = (MemberNameVar*) GetSubject();
    _export = new ButtonState((int) iname->GetExport());
    _msg = strnew(msg);
    Insert(Interior(bs));
}

MemberNameVarView::~MemberNameVarView () { delete _msg; }

void MemberNameVarView::SetAffectedStates(UList* ilist) {
    IBVarView::SetAffectedStates(ilist);
    MemberNameVar* mnamer = (MemberNameVar*) GetSubject();
    MemberSharedName* msnamer = mnamer->GetMemberSharedName();
    ilist->Append(new UList(msnamer));
}

boolean MemberNameVarView::ChangedSubject (const char*& errors) {
    errors = nil;
    int value;
    const char* name = _name->Text(); 

    _export->GetValue(value);
    MemberNameVar* mnamer = (MemberNameVar*) GetSubject();
    MemberSharedName* msnamer = mnamer->GetMemberSharedName();
    if (name != nil && *name != '\0') {
        
        GetFirewallCmd firewallCmd(_icomp);
        firewallCmd.Execute();
        GetConflictCmd conflictCmd(firewallCmd.GetFirewall(), name);
        conflictCmd.Execute();
        UList* ulist = conflictCmd.GetConflict();
	for(UList* i = ulist->First(); i != ulist->End(); i = i->Next()) {
	    StateVar* state = (StateVar*) (*i)();

	    if (
                !state->IsA(INSTANCENAME_VAR) && state != msnamer
            ) {
                sprintf( 
                    buf,
                    "Member name \"%s\" already exists.",
                    name
                );
                errors = buf;
                return false;
	    }
	}
        msnamer->SetExport(value);
        if (strcmp(msnamer->GetName(), name) != 0) {
            msnamer->SetName(name);
            msnamer->SetMachGen(false);
        }
        errors = nil;

    } else {
	sprintf(buf, "Null member name is invalid");
	errors = buf;
    }
    return errors == nil;
}

void MemberNameVarView::Init () { 
    MemberNameVar* mnamer = (MemberNameVar*) GetSubject();
    const char* name = mnamer->GetName();
    if( name != nil) {
	_name->Message(name);
    }
}

Interactor* MemberNameVarView::Interior (ButtonState* bs) {
    const int gap = round(.1*cm);

    HBox* hbox = new HBox(
        new Message(_msg),
        PaddedFrame(_name = new MatchEditor(bs, "a rather long name")),
	new HGlue(2*gap, 0, 0),
	new CheckBox("Export", _export, 1, 0)
    );
    hbox->Align(Center);

    _name->Match("%[_a-zA-Z]%[_a-zA-Z0-9_]", true);
    return hbox;
}

/*****************************************************************************/

TrackNameVarView::TrackNameVarView (
    TrackNameVar* nameVar, ButtonState* bs, GraphicComp* icomp
) : IBVarView(nameVar, icomp) {
    Insert(Interior(bs));
}

boolean TrackNameVarView::ChangedSubject (const char*& errors) {
    errors = nil;
    const char* name = _name->Text(); 
    TrackNameVar* proc = (TrackNameVar*) GetSubject();

    if (name != nil) {
        GetFirewallCmd firewallCmd(_icomp);
        firewallCmd.Execute();
        GetConflictCmd conflictCmd(firewallCmd.GetFirewall(), name);
        conflictCmd.Execute();
        UList* cl = conflictCmd.GetConflict();
        for (UList* i = cl->First(); i != cl->End(); i = i->Next()) {
            StateVar* state = (StateVar*) (*i)();
	    if (state->IsA(BUTTONSHAREDNAME)) {
                ButtonSharedName* bstmp = (ButtonSharedName*) state;
                if (strcmp(bstmp->GetName(), name) == 0) {
                    sprintf(
                        buf, 
                        "A ButtonState name \"%s\" already exists.",
                        name
                    );
                    errors = buf;
                    return false;
                }
            } else if (
                !state->IsA(PROCNAME_VAR) && !state->IsA(INSTANCENAME_VAR)
            ) {
                sprintf(
                    buf, 
                    "A member or class name \"%s\" already exists.",
                    name
                );
                errors = buf;
                return false;

            }
        }
	proc->SetName(name);
        proc->SetMachGen(false);
    }
    return errors == nil;
}

void TrackNameVarView::Init () { 
    TrackNameVar* proc = (TrackNameVar*) GetSubject();
    const char* name = proc->GetName();
    if( name != nil) {
	_name->Message(name);
    }
}

Interactor* TrackNameVarView::Interior (ButtonState* bs) {
    TrackNameVar* proc = (TrackNameVar*) GetSubject();

    HBox* hbox = new HBox(
        new Message("Member function: "),
        PaddedFrame(_name = new MatchEditor(bs, "a rather long name"))
    );
    hbox->Align(Center);

    _name->Match("%[_a-zA-Z]%[_a-zA-Z0-9_]", true);
    return hbox;
}

/*****************************************************************************/

ShapeVarView::ShapeVarView (ShapeVar* s, ButtonState* bs) : IBVarView(s) {
    if (bs != nil) {
        Insert(Interior(bs));
    }
}

boolean ShapeVarView::ChangedSubject (const char*&) {
    ShapeVar* subj = (ShapeVar*) GetSubject();

    IBShape* subshape = subj->GetShape();

    subshape->width = 
	(subshape->hnat) ? atoi(_width->Text()) : subshape->width;
    subshape->height = 
	(subshape->vnat) ? atoi(_height->Text()) : subshape->height;
    subshape->hshrink = 
	(subshape->hshr) ? atoi(_hshrink->Text()) : subshape->hshrink;
    subshape->vshrink =
	(subshape->vshr) ? atoi(_vshrink->Text()) : subshape->vshrink;
    subshape->hstretch =
	(subshape->hstr) ? atoi(_hstretch->Text()) : subshape->hstretch;
    subshape->vstretch =
	(subshape->vstr) ? atoi(_vstretch->Text()) : subshape->vstretch;

    return true;
}

void ShapeVarView::Init () {
    ShapeVar* subj = (ShapeVar*) GetSubject();
    IBShape* ibshape = subj->GetShape();
    if (ibshape->hnat) {
        _width->Message(IntToString(ibshape->width));
    }
    if (ibshape->vnat) {
        _height->Message(IntToString(ibshape->height));
    }
    if (ibshape->hstr) {
        _hstretch->Message(IntToString(ibshape->hstretch));
    }
    if (ibshape->vstr) {
        _vstretch->Message(IntToString(ibshape->vstretch));
    }
    if (ibshape->hshr) {
        _hshrink->Message(IntToString(ibshape->hshrink));
    }
    if (ibshape->vshr) {
        _vshrink->Message(IntToString(ibshape->vshrink));
    }
}

Interactor* ShapeVarView::Interior (ButtonState* bs) {
    Interactor* interior = nil;

    Interactor* natw, *nath;
    Interactor* hshr, *vshr;
    Interactor* hstr, *vstr;

    const char* sample = "9999999999";
    ShapeVar* subj = (ShapeVar*) GetSubject();
    IBShape* ibshape = subj->GetShape();
    if (ibshape->hnat) {
        _width = new MatchEditor(bs, sample);
	_width->Match("%d", true);
	natw = PaddedFrame(_width);
    } else {
	natw = IntToMsg(ibshape->width);
    }
    if (ibshape->vnat) {
        _height = new MatchEditor(bs, sample);
	_height->Match("%d", true);
	nath = PaddedFrame(_height);
    } else {
	nath = IntToMsg(ibshape->height);
    }
    if (ibshape->hshr) {
        _hshrink = new MatchEditor(bs, sample);
	_hshrink->Match("%d", true);
	hshr = PaddedFrame(_hshrink);
    } else {
	hshr = IntToMsg(ibshape->hshrink);
    }
    if (ibshape->vshr) {
        _vshrink = new MatchEditor(bs, sample);
	_vshrink->Match("%d", true);
	vshr = PaddedFrame(_vshrink);
    } else {
	vshr = IntToMsg(ibshape->vshrink);
    }
    if (ibshape->hstr) {
        _hstretch = new MatchEditor(bs, sample);
	_hstretch->Match("%d", true);
	hstr = PaddedFrame(_hstretch);
    } else {
	hstr = IntToMsg(ibshape->hstretch);
    }
    if (ibshape->vstr) {
        _vstretch = new MatchEditor(bs, sample);
	_vstretch->Match("%d", true);
	vstr = PaddedFrame(_vstretch);
    } else {
	vstr = IntToMsg(ibshape->vstretch);
    }

    return Interior(natw, nath, hshr, vshr, hstr, vstr);
}

Interactor* ShapeVarView::Interior (
    Interactor* natw, Interactor* nath,
    Interactor* hshr, Interactor* vshr,
    Interactor* hstr, Interactor* vstr
) {
    natw = RightJustified(natw);        nath = LeftJustified(nath);	
    hshr = RightJustified(hshr);        vshr = LeftJustified(vshr);
    hstr = RightJustified(hstr);	vstr = LeftJustified(vstr);

    const int gap = round(.1*cm);
    Message* natSize, *shrink, *stretch;
    GetMessages(natSize, shrink, stretch);

    Message* xnat = new Message(" x ");
    Message* xshr = new Message(" x ");
    Message* xstr = new Message(" x ");

    Tray* t = new Tray;

    t->HBox(t, natSize, natw, xnat);   t->HBox(xnat, nath, new HGlue, t);
    t->HBox(t, shrink, hshr, xshr);    t->HBox(xshr, vshr, new HGlue, t);
    t->HBox(t, stretch, hstr, xstr);   t->HBox(xstr, vstr, new HGlue, t);

    t->Align(Left, natw, hshr, hstr);
    t->Align(Right, natw, hshr, hstr);
    
    t->Align(Left, nath, vshr, vstr);
    t->Align(Right, nath, vshr, vstr);

    t->Align(VertCenter, natSize, natw, xnat, nath);
    t->Align(VertCenter, shrink, hshr, xshr, vshr);
    t->Align(VertCenter, stretch, hstr, xstr, vstr);

    t->VBox(t, natw, new VGlue(gap), hshr, new VGlue(gap), hstr, t);
    t->VBox(t, nath, new VGlue(gap), vshr, new VGlue(gap), vstr, t);
    return new HBox(new VBox(t, new VGlue), new HGlue);
}

void ShapeVarView::GetMessages(Message*& m1, Message*& m2, Message*& m3) {
    m1 = new Message("Natural Size (width x height): ");
    m2 = new Message("Shrinkability (horiz x vert): ");
    m3 = new Message("Stretchability (horiz x vert): ");
}

/*****************************************************************************/

MarginVarView::MarginVarView (
    ShapeVar* s, ButtonState* bs
) : ShapeVarView(s, bs) {
    Insert(Interior(bs));
}

void MarginVarView::GetMessages(Message*& m1, Message*& m2, Message*& m3) {
    m1 = new Message("Natural Margins (horiz x vert): ");
    m2 = new Message("Shrinkability (horiz x vert): ");
    m3 = new Message("Stretchability (horiz x vert): ");
}

/*****************************************************************************/

RelatedVarView::RelatedVarView (
    MemberNameVar* sv, ButtonState* bs, GraphicComp* icomp, const char* msg
) : IBVarView(sv, icomp) {
    _msg = strnew(msg);
    Insert(Interior(bs));
}

RelatedVarView::~RelatedVarView () { delete _msg; }

boolean RelatedVarView::ChangedSubject (const char*& errors) {
    errors = nil;
    const char* name = _name->Text();
    if (name != nil && *name != '\0') {

        GetFirewallCmd firewallCmd(_icomp);
        firewallCmd.Execute();
        GetConflictCmd conflictCmd(firewallCmd.GetFirewall(), name);
        conflictCmd.Execute();
        UList* cl = conflictCmd.GetConflict();
        if (cl->IsEmpty()) {
            sprintf(
                buf, "No interactor member named \"%s\" exists.", name
            );
            errors = buf;
    
        } else {
            UList* i = cl->First();
	    StateVar* msnamer = (StateVar*) (*i)(); 
	    if (!msnamer->IsA(MEMBERSHAREDNAME)) {
		sprintf(
                    buf, "\"%s\" is not a defined member name.", name
                );
                errors = buf;

	    } else {
                MemberNameVar* mnamer = (MemberNameVar*) GetSubject();
                InteractorComp* icomp = (InteractorComp*) _icomp;
                InteractorComp* ctarget = conflictCmd.GetCTarget();
                if (ctarget != nil && icomp->IsRelatableTo(ctarget)) {
                    mnamer->SetMemberSharedName((MemberSharedName*) msnamer);

                } else {
                    const char* iname = icomp->GetMemberNameVar()->GetName();
                    sprintf(
                        buf, "\"%s\" is not relatable with \"%s\".",name,iname
                    );
                    errors = buf;
                }
	    }
	}
    } else if (*name == '\0') {
        MemberNameVar* mnamer = (MemberNameVar*) GetSubject();
        mnamer->SetMemberSharedName(new MemberSharedName("", false, false));
    }
    return errors == nil;
}

void RelatedVarView::Init () { 
    MemberNameVar* mnamer = (MemberNameVar*) GetSubject();
    const char* name = mnamer->GetName();
    if( name != nil) {
	_name->Message(name);
    }
}

Interactor* RelatedVarView::Interior (ButtonState* bs) {
    const int gap = round(.1*cm);

    HBox* hbox = new HBox(
        new Message(_msg),
        PaddedFrame(_name = new MatchEditor(bs, "a rather long name"))
    );
    _name->Match("%[_a-zA-Z]%[_a-zA-Z0-9_]", true);
    hbox->Align(Center);
    return hbox;
}

/*****************************************************************************/

SubclassNameVarView::SubclassNameVarView (
    SubclassNameVar* inv, ButtonState* bs, GraphicComp* icomp
) : IBVarView(inv, icomp) {
    Insert(Interior(bs));
}

boolean SubclassNameVarView::ChangedSubject (const char*& errors) {
    errors = nil;
    const char* subclass = _subclass->Text(); 

    SubclassNameVar* subvar = (SubclassNameVar*) GetSubject();
    const char* orig = subvar->GetName();
    const char* baseclass = subvar->GetBaseClass();
    if (strcmp(orig, subclass) != 0) {
        if (*subclass != '\0') {
            if (strcmp(subclass, baseclass) != 0) {
                GetConflictCmd conflictCmd(
                    (GraphicComp*) _icomp->GetRoot(), subclass, true
                );
                conflictCmd.Execute();
                UList* cl = conflictCmd.GetConflict();
                for(UList* i = cl->First(); i != cl->End(); i = i->Next()) {
                    StateVar* state = (StateVar*) (*i)();
                    if (state->IsA(SUBCLASSNAME_VAR) && state != subvar) {
                        SubclassNameVar* svar = (SubclassNameVar*) state;
                        if (strcmp(svar->GetBaseClass(), baseclass) != 0) {
                            sprintf(
                                buf, 
                                "\"%s\" has been used with a different base class.",
                                subclass
                            );
                            errors = buf;
                            return false;
                        }
                    }
                }
                GetFirewallCmd firewallCmd(_icomp);
                firewallCmd.Execute();
                GetConflictCmd sconflict(firewallCmd.GetFirewall(),subclass);
                sconflict.Execute();
                cl = sconflict.GetConflict();
                for(i = cl->First(); i != cl->End(); i = i->Next()) {
                    StateVar* state = (StateVar*) (*i)();
                    if (
                        !state->IsA(INSTANCENAME_VAR) && 
                        !state->IsA(SUBCLASSNAME_VAR) && 
                        state != subvar
                        
                    ) {
                        sprintf(
                            buf, 
                            "A member or procedure name \"%s\" already exists.",
                            subclass
                        );
                        errors = buf;
                        return false;
                    }
                }
            } else if (subvar->IsAbstract()) {
                sprintf(buf, "Base class %s is abstract!!", subclass);
                errors = buf;
                return false;
            }
            subvar->SetMachGen(false);
            subvar->SetName(subclass);
            errors = nil;

        } else {
            sprintf(buf, "Null subclass name is invalid");
            errors = buf;
        }
    }
    return errors == nil;
}

void SubclassNameVarView::Init () { 
    SubclassNameVar* subvar = (SubclassNameVar*) GetSubject();
    const char* subclass = subvar->GetName();
    if( subclass != nil) {
	_subclass->Message(subclass);
    }
}

Interactor* SubclassNameVarView::Interior (ButtonState* bs) {
    const int gap = round(.1*cm);
    SubclassNameVar* subvar = (SubclassNameVar*) GetSubject();
    const char* baseclass = subvar->GetBaseClass();

    HBox* hbox1 = new HBox(
	new Message("Class Name: "),
        PaddedFrame(
	    _subclass = new MatchEditor(bs, "a rather long name")
	)
    );
    HBox* hbox2 = new HBox(
	new Message("Base Class Name: "),
	new Message(baseclass),
	new HGlue
    );
    VBox* vbox = new VBox(hbox1, new VGlue(2, 0, 0), hbox2);
    hbox1->Align(Center);
    hbox2->Align(Center);

    _subclass->Match("%[_a-zA-Z]%[_a-zA-Z0-9_]", true);
    return vbox;
}
/*****************************************************************************/

BooleanStateVarView::BooleanStateVarView (
    BooleanStateVar* bstate, const char* message
) : IBVarView(bstate) {
    _bstate = new ButtonState(bstate->GetBooleanState());
    _message = strnew(message);
    Insert(Interior());
}

BooleanStateVarView::~BooleanStateVarView () {
    delete _message;
}
    
boolean BooleanStateVarView::ChangedSubject (const char*&) {
    int value;
    _bstate->GetValue(value);
    BooleanStateVar* bstate = (BooleanStateVar*) GetSubject();
    bstate->SetBooleanState(value);
    return true;
}

Interactor* BooleanStateVarView::Interior () {
    HBox* hbox = new HBox(
        new CheckBox(_message, _bstate, 1, 0),
        new HGlue
    );
    hbox->Align(Center);
    return hbox;
}

/*****************************************************************************/

FBrowserVarView::FBrowserVarView (
    FBrowserVar* fbVar, ButtonState* bs
) : IBVarView(fbVar) {
    Insert(Interior(bs));
}

FBrowserVarView::~FBrowserVarView () {
    delete _dir;
    delete _textfilter;
}

boolean FBrowserVarView::ChangedSubject (const char*&) {
    const char* dir = _dir->Text(); 
    const char* textfilter = _textfilter->Text(); 
    FBrowserVar* fbVar = (FBrowserVar*) GetSubject();

    fbVar->SetDirName(dir);
    fbVar->SetTextFilter(textfilter);
    return true;
}

void FBrowserVarView::Init () { 
    FBrowserVar* fbVar = (FBrowserVar*) GetSubject();
    _dir->Message(fbVar->GetDirName());
    _textfilter->Message(fbVar->GetTextFilter());
}

Interactor* FBrowserVarView::Interior (ButtonState* bs) {
    const int gap = round(.1*cm);

    FBrowserVar* fbVar = (FBrowserVar*) GetSubject();

    _dir = new MatchEditor(bs, "a rather long name");
    _textfilter = new MatchEditor(bs, "a rather long name");
    Tray* t = new Tray;

    Interactor* dir = PaddedFrame(_dir);
    Interactor* textfilter = PaddedFrame(_textfilter);

    Interactor* mdir = new Message("Directory: ");
    Interactor* mtextfilter = new Message("Text Filter: ");
    
    t->HBox(t, mdir, dir, t);
    t->HBox(t, mtextfilter, textfilter, t);
    t->VBox(
        t, new VGlue(gap),  mdir, new VGlue(gap), mtextfilter, 
        new VGlue(gap), t
    );
    t->VBox(
        t, new VGlue(gap),  dir, new VGlue(gap), textfilter, 
        new VGlue(gap), t
    );
    t->Align(VertCenter, mdir, dir);
    t->Align(VertCenter, mtextfilter, textfilter);
    t->Align(Left, dir, textfilter);
        
    return t;
}

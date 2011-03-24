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
 * EditorComp
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibeditor.h,v 1.2 91/09/27 14:13:49 tang Exp $
 */

#ifndef ibeditor_h
#define ibeditor_h

#include "ibcode.h"
#include "ibscene.h"

class MemberNameVar;
class ButtonStateVar;
class InfoDialog;
class GetConflictCmd;

class EditorComp : public MonoSceneClass {
public:
    EditorComp(IBGraphic* = nil);
    virtual ~EditorComp();

    virtual boolean IsRelatableTo(InteractorComp*);
    virtual boolean IsRelatable();
    virtual void Interpret(Command*);
    void GetExtraConflict(GetConflictCmd*);

    virtual void Instantiate();
    virtual boolean IsAScene();
    virtual void Read(istream&);
    virtual void Write(ostream&);
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual StateVar* GetState(const char*);
    virtual void SetState(const char*, StateVar*);
    virtual InteractorComp& operator = (InteractorComp&);
    virtual ButtonStateVar* GetButtonStateVar();

    MemberNameVar* GetViewerVar();
    MemberNameVar* GetKeyMap();
    MemberNameVar* GetSelection();
    MemberNameVar* GetCompName();
protected:
    MemberNameVar* _viewerVar;
    MemberNameVar* _keymap;
    MemberNameVar* _selection;
    MemberNameVar* _compname;
    ButtonStateVar* _curCtrlVar;
};

inline MemberNameVar* EditorComp::GetViewerVar() { return _viewerVar; }
inline MemberNameVar* EditorComp::GetKeyMap() { return _keymap; }
inline MemberNameVar* EditorComp::GetSelection() { return _selection; }
inline MemberNameVar* EditorComp::GetCompName() { return _compname; }
inline ButtonStateVar* EditorComp::GetButtonStateVar() { return _curCtrlVar; }
inline boolean EditorComp::IsRelatable () { return true; }
inline boolean EditorComp::IsAScene () { return false; }

class EditorView : public MonoSceneClassView {
public:
    EditorView(EditorComp* = nil);
    EditorComp* GetEditorComp();

    virtual InfoDialog* GetInfoDialog();
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
};

class EditorCode : public MonoSceneClassCode {
public:
    EditorCode(EditorComp* = nil);

    virtual boolean Definition(ostream&);
    EditorComp* GetEditorComp();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean CoreConstDecls(ostream&);
    virtual boolean CoreConstInits(ostream&);
    virtual boolean EmitIncludeHeaders(ostream&);
};

#endif


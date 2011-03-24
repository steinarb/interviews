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
 * Shared Interactor component declarations.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibcomp.h,v 1.2 91/09/27 14:13:21 tang Exp $
 */

#ifndef ibcomp_h
#define ibcomp_h

#include <Unidraw/Components/grcomp.h>
#include <Unidraw/Components/grview.h>

class InfoDialog;
class Graphic;
class GraphicComp;
class GrBlockComp;
class MemberNameVar;
class StateVar;
class SubclassNameVar;
class Selection;
class UList;
class istream;
class ostream;

class IComp : public GraphicComps {
public:
    virtual ~IComp();

    SubclassNameVar* GetClassNameVar();
    MemberNameVar* GetMemberNameVar();

    virtual StateVar* GetState(const char*);
    virtual void SetState(const char*, StateVar*);

    virtual void Interpret(Command*);
    virtual void Uninterpret(Command*);
    void Propagate(Command*);
    void Unpropagate(Command*);

    virtual void Read(istream&);
    virtual void Write(ostream&);
    virtual void Instantiate();
    virtual ClassId GetClassId();
    virtual ClassId GetSubstId(const char*& delim);
    virtual boolean IsA(ClassId);
    virtual GraphicComp* GetTarget();

    GrBlockComp* GetGrBlockComp();

    static void SetRelease(boolean);
protected:
    IComp(Graphic* = nil);
    void ReadStateVars(istream&);
    void WriteStateVars(ostream&);
protected:
    SubclassNameVar* _classNameVar;
    MemberNameVar* _memberVar;
    GraphicComp* _target;
    static boolean _release;
};

inline void IComp::SetRelease (boolean release) { _release = release; }
inline SubclassNameVar* IComp::GetClassNameVar () {
    return _classNameVar; 
}
inline MemberNameVar* IComp::GetMemberNameVar () {
    return _memberVar; 
}


class IView : public GraphicViews {
public:
    IView(IComp* = nil);

    GraphicView* GetKidView();

    virtual void DrawHandles();
    virtual void RedrawHandles();
    virtual void EraseHandles();
    virtual void InitHandles();

    virtual Selection* SelectAll();
    virtual Selection* ViewContaining(Coord, Coord);
    virtual Selection* ViewsContaining(Coord, Coord);
    virtual Selection* ViewIntersecting(Coord, Coord, Coord, Coord);
    virtual Selection* ViewsIntersecting(Coord, Coord, Coord, Coord);
    virtual Selection* ViewsWithin(Coord, Coord, Coord, Coord);

    virtual InfoDialog* GetInfoDialog();
    virtual Manipulator* CreateManipulator(Viewer*,Event&,Transformer*,Tool*);
    virtual Command* InterpretManipulator(Manipulator*);
    virtual void Interpret(Command*);

    IComp* GetIComp();
};

#endif


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
 * CodeView base class for generating a C++ external representation
 * and subclasses for interactor components.
 * $Header: /master/3.0/iv/src/bin/ibuild/RCS/ibcode.h,v 1.2 91/09/27 14:13:14 tang Exp $
 */

#ifndef ibcode_h
#define ibcode_h

#include <Unidraw/Components/externview.h>
#include <OS/list.h>

class IComp;
class InteractorComp;
class GraphicComp;
class UList;
class MemberNameVar;


declareList(CharStringList,char*);

class StringList {
public:
    StringList();
    ~StringList();

    void Append(const char*);
    boolean Search(const char*);
private:
    CharStringList list;
};

class CodeView : public PreorderView {
public:
    CodeView(GraphicComp* = nil);
    virtual ~CodeView();

    virtual boolean Definition(ostream&);
    virtual void Update();

    InteractorComp* GetIntComp();
    IComp* GetIComp();
    const char* GetErrors();
    void GetClassList(UList*);

    boolean GenDothFile(const char*, ostream&);
    boolean GenDotcFile(const char*, ostream&);
    boolean GenPropFile(const char*, ostream&);
    boolean GenMainFile(const char*, ostream&);
    boolean GenCorehFile(const char*, ostream&);
    boolean GenCorecFile(const char*, ostream&);

    virtual ExternView* GetView(Iterator);
    virtual void SetView(ExternView*, Iterator&);

    virtual void First(Iterator&);
    virtual void Last(Iterator&);
    virtual void Next(Iterator&);
    virtual void Prev(Iterator&);
    virtual boolean Done(Iterator);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    boolean Search(MemberNameVar*, InteractorComp*&);
    boolean Scan(ClassId);

    CodeView* CreateCodeView(GraphicComp*);

    void BeginInstantiate(ostream&);
    void EndInstantiate(ostream&);
    void InstanceName(ostream&, const char* = ", ");

    UList* Elem(Iterator);
    CodeView* View(UList*);
    void DeleteView(Iterator&);
    void DeleteViews();
    void CleanUp();

    boolean AllKidsDefined();
    boolean EmitCorehHeader(ostream&);
    boolean EmitExpHeader(ostream&);

    boolean EmitPropertyData(ostream&);
    boolean EmitForwardDecls(ostream&);
    boolean EmitCoreDecls(ostream&);
    boolean EmitCoreInits(ostream&);
    boolean EmitBSDecls(CodeView*, ostream&);
    boolean EmitBSInits(CodeView*, ostream&);
    boolean EmitInstanceDecls(CodeView*, ostream&);
    boolean EmitInstanceInits(CodeView*, ostream&);
    boolean EmitFunctionDecls(CodeView*, ostream&);
    boolean EmitFunctionInits(CodeView*, ostream&);
    boolean EmitClassDecls(ostream&);

    boolean EmitClassInits(ostream&);
    boolean EmitClassHeaders(ostream&);
    boolean EmitHeaders(ostream&);

    boolean EmitButtonState(ostream&);
    boolean EmitGlue(ostream&);
    boolean EmitMenu(ostream&);
    boolean EmitShaper(ostream&);
    boolean EmitSlider(ostream&);
    boolean Iterate(ostream&);

    void GetCoreClassName(char*);

    virtual boolean CoreConstDecls(ostream&);
    virtual boolean CoreConstInits(ostream&);
    virtual boolean ConstDecls(ostream&);
    virtual boolean ConstInits(ostream&);
    virtual boolean EmitIncludeHeaders(ostream&);

    boolean WriteGraphicDecls(Graphic*, ostream&);
    boolean WriteGraphicInits(Graphic*, ostream&);
protected:
    boolean _lock;
    UList* _views;
    
    static boolean _emitForward;
    static boolean _emitBSDecls;
    static boolean _emitBSInits;
    static boolean _emitExpHeader;
    static boolean _emitCorehHeader;

    static boolean _emitInstanceDecls;
    static boolean _emitInstanceInits;
    static boolean _emitCoreDecls;
    static boolean _emitCoreInits;
    static boolean _emitProperty;
    static boolean _emitClassDecls;
    static boolean _emitClassInits;
    static boolean _emitClassHeaders;
    static boolean _emitHeaders;

    static boolean _emitFunctionDecls;
    static boolean _emitFunctionInits;
    static boolean _icomplete;
    static boolean _emitExport;
    static boolean _emitMain;
    static boolean _scope;
    static boolean _emitGraphicState;
    static boolean _emitGraphicComp;
    
    static UList* _instancelist;
    static StringList* _functionlist;
    static StringList* _bsdeclslist;
    static StringList* _bsinitslist;
    static StringList* _namelist;
    static StringList* _globallist;
    
    static StringList* _brushlist;
    static StringList* _colorlist;
    static StringList* _fontlist;
    static StringList* _patternlist;

    static char _errbuf[CHARBUFSIZE*10];
    static const char* _classname;
};

class RootCodeView : public CodeView {
public:
    RootCodeView(GraphicComp* = nil);
    virtual ~RootCodeView();
};

#endif


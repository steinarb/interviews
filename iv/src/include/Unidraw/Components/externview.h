/*
 * Copyright (c) 1990, 1991 Stanford University
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
 * ExternView - external representation of component.
 */

#ifndef unidraw_components_externview_h
#define unidraw_components_externview_h

#include <Unidraw/Components/compview.h>

class ostream;

class ExternView : public ComponentView {
public:
    virtual boolean Emit(ostream&);
    virtual boolean Definition(ostream&);

    virtual ExternView* GetView(Iterator);
    virtual void SetView(ExternView*, Iterator&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    ExternView(Component* = nil);
};

class PreorderView : public ExternView {
public:
    virtual boolean Definition(ostream&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    PreorderView(Component* = nil);
};

class InorderView : public ExternView {
public:
    virtual boolean Definition(ostream&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    InorderView(Component* = nil);
};

class PostorderView : public ExternView {
public:
    virtual boolean Definition(ostream&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    PostorderView(Component* = nil);
};

#endif

/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
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
 * A subject is an object that has one or more views
 * that it wishes to notify when it changes.
 */

#ifndef subject_h
#define subject_h

#include <InterViews/resource.h>

class Interactor;

class ViewList {
    friend class Subject;
    friend class InteractorItr;

    Interactor* element;
    ViewList* next;
};

class Subject : public Resource {
public:
    Subject();
    ~Subject();

    virtual void Attach(Interactor*);
    virtual void Detach(Interactor*);

    void all(InteractorItr&);
    virtual void Notify();
    boolean IsView(Interactor*);
protected:
    ViewList* views;
private:
    friend class InteractorItr;
};

class InteractorItr {
public:
    InteractorItr() { cur = nil; }
    InteractorItr(Subject* s) { s->all(*this); }
protected:
    boolean more() { return cur != nil; }
    InteractorItr& next() { cur = cur->next; return *this; }
    Interactor* view() { return cur->element; }
private:
    friend class Subject;

    ViewList* cur;
};

#endif

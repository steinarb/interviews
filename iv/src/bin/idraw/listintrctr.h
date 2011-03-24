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

// $Header: listintrctr.h,v 1.9 89/10/09 14:48:51 linton Exp $
// declares classes InteractorNode and InteractorList.

#ifndef listintrctr_h
#define listintrctr_h

#include "list.h"

// Declare imported types.

class Interactor;

// An InteractorNode contains an Interactor pointer.

class InteractorNode : public BaseNode {
public:

    InteractorNode (Interactor* i) { interactor = i; }
    boolean SameValueAs (void* p) { return interactor == p; }
    Interactor* GetInteractor () { return interactor; }

protected:

    Interactor* interactor;	// points to a Interactor

};

// An InteractorList manages a list of InteractorNodes.

class InteractorList : public BaseList {
public:

    InteractorNode* First();
    InteractorNode* Last();
    InteractorNode* Prev();
    InteractorNode* Next();
    InteractorNode* GetCur();
    InteractorNode* Index(int);

};

// Cast these functions to return InteractorNodes instead of
// BaseNodes.

inline InteractorNode* InteractorList::First () {
    return (InteractorNode*) BaseList::First();
}

inline InteractorNode* InteractorList::Last () {
    return (InteractorNode*) BaseList::Last();
}

inline InteractorNode* InteractorList::Prev () {
    return (InteractorNode*) BaseList::Prev();
}

inline InteractorNode* InteractorList::Next () {
    return (InteractorNode*) BaseList::Next();
}

inline InteractorNode* InteractorList::GetCur () {
    return (InteractorNode*) BaseList::GetCur();
}

inline InteractorNode* InteractorList::Index (int index) {
    return (InteractorNode*) BaseList::Index(index);
}

#endif

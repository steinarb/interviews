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

// $Header: listselectn.h,v 1.9 89/10/09 14:48:56 linton Exp $
// declares classes SelectionNode and SelectionList.

#ifndef listselectn_h
#define listselectn_h

#include "list.h"

// Declare imported types.

class Selection;

// A SelectionNode stores a pointer to a Selection.

class SelectionNode : public BaseNode {
public:

    SelectionNode (Selection* s) { selection = s; }
    boolean SameValueAs (void* p) { return selection == p; }
    Selection* GetSelection () { return selection; }

protected:

    Selection* selection;	// points to a Selection

};

// A SelectionList manages a list of SelectionNodes.

class SelectionList : public BaseList {
public:

    SelectionNode* First();
    SelectionNode* Last();
    SelectionNode* Prev();
    SelectionNode* Next();
    SelectionNode* GetCur();
    SelectionNode* Index(int);

};

// Cast these functions to return SelectionNodes instead of BaseNodes.

inline SelectionNode* SelectionList::First () {
    return (SelectionNode*) BaseList::First();
}

inline SelectionNode* SelectionList::Last () {
    return (SelectionNode*) BaseList::Last();
}

inline SelectionNode* SelectionList::Prev () {
    return (SelectionNode*) BaseList::Prev();
}

inline SelectionNode* SelectionList::Next () {
    return (SelectionNode*) BaseList::Next();
}

inline SelectionNode* SelectionList::GetCur () {
    return (SelectionNode*) BaseList::GetCur();
}

inline SelectionNode* SelectionList::Index (int index) {
    return (SelectionNode*) BaseList::Index(index);
}

#endif

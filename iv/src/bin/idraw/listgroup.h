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

// $Header: listgroup.h,v 1.9 89/10/09 14:48:45 linton Exp $
// declares classes GroupNode and GroupList.

#ifndef listgroup_h
#define listgroup_h

#include "list.h"

// Declare imported types.

class PictSelection;
class SelectionList;

// A GroupNode contains a PictSelection pointer, a boolean value, and
// two SelectionList pointers.

class GroupNode : public BaseNode {
public:

    GroupNode(PictSelection*, boolean, SelectionList*);
    ~GroupNode();

    PictSelection* GetParent () { return parent; }
    boolean GetHasChildren () { return haschildren; }
    SelectionList* GetChildren () { return children; }
    SelectionList* GetChildrenGS () { return childrengs; }

protected:

    PictSelection* parent;	// contains a Selection which may have children
    boolean haschildren;	// true if this parent has children
    SelectionList* children;	// lists the parent's children, if any
    SelectionList* childrengs;	// stores the children's orig. graphic states

};

// A GroupList manages a list of GroupNodes.

class GroupList : public BaseList {
public:

    GroupNode* First();
    GroupNode* Last();
    GroupNode* Prev();
    GroupNode* Next();
    GroupNode* GetCur();
    GroupNode* Index(int);

};

// Cast these functions to return GroupNodes instead of BaseNodes.

inline GroupNode* GroupList::First () {
    return (GroupNode*) BaseList::First();
}

inline GroupNode* GroupList::Last () {
    return (GroupNode*) BaseList::Last();
}

inline GroupNode* GroupList::Prev () {
    return (GroupNode*) BaseList::Prev();
}

inline GroupNode* GroupList::Next () {
    return (GroupNode*) BaseList::Next();
}

inline GroupNode* GroupList::GetCur () {
    return (GroupNode*) BaseList::GetCur();
}

inline GroupNode* GroupList::Index (int index) {
    return (GroupNode*) BaseList::Index(index);
}

#endif

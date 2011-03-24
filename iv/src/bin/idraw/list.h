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

// $Header: list.h,v 1.9 89/10/09 14:48:35 linton Exp $
// declares classes BaseNode and BaseList.

#ifndef list_h
#define list_h

#include <InterViews/defs.h>

// A BaseNode contains links to its previous and next BaseNodes.
// BaseLists can link BaseNodes to each other to form lists of
// BaseNodes.

class BaseNode {

    friend class BaseList;

public:

    BaseNode();
    virtual ~BaseNode();

    virtual boolean SameValueAs(void*);

protected:

private:

    BaseNode* prev;		// points to previous BaseNode in list
    BaseNode* next;		// points to next BaseNode in list

};

// A BaseList maintains and iterates through a list of BaseNodes.

class BaseList {
public:

    BaseList();
    ~BaseList();

    int Size();
    boolean AtEnd();
    BaseNode* First();
    BaseNode* Last();
    BaseNode* Prev();
    BaseNode* Next();
    BaseNode* GetCur();
    BaseNode* Index(int);
    boolean Find(void*);

    void Append(BaseNode*);
    void Prepend(BaseNode*);
    void InsertAfterCur(BaseNode*);
    void InsertBeforeCur(BaseNode*);

    void RemoveCur();
    void DeleteCur();
    void DeleteAll();

private:

    BaseNode* head;		// points to dummy head of circular list
    BaseNode* cur;		// points to current node of circular list
    int size;			// stores number of non-dummy nodes in list

};

// Size returns the number of non-dummy nodes in a list.

inline int BaseList::Size () {
    return size;
}

// AtEnd returns true if the current node is the dummy node.

inline boolean BaseList::AtEnd () {
    return cur == head;
}

// First returns the first node in a list (or the head if it's empty)
// and sets the current node to that node.

inline BaseNode* BaseList::First () {
    cur = head->next;
    return cur;
}

// Last returns the last node in a list (or the head if it's empty)
// and sets the current node to that node.

inline BaseNode* BaseList::Last () {
    cur = head->prev;
    return cur;
}

// Prev returns the node before the current node and sets the current
// node to that node.

inline BaseNode* BaseList::Prev () {
    cur = cur->prev;
    return cur;
}

// Next returns the node after the current node and sets the current
// node to that node.

inline BaseNode* BaseList::Next () {
    cur = cur->next;
    return cur;
}

// GetCur returns the current node in a list.

inline BaseNode* BaseList::GetCur () {
    return cur;
}

#endif

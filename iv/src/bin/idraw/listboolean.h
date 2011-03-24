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

// $Header: listboolean.h,v 1.8 89/10/09 14:48:36 linton Exp $
// declares classes booleanNode and booleanList.

#ifndef listboolean_h
#define listboolean_h

#include "list.h"

// A booleanNode contains a boolean value.

class booleanNode : public BaseNode {
public:

    booleanNode (boolean v) { value = v; }
    boolean GetBoolean () { return value; }

protected:

    boolean value;		// contains a boolean value

};

// A booleanList manages a list of booleanNodes.

class booleanList : public BaseList {
public:

    booleanNode* First();
    booleanNode* Last();
    booleanNode* Prev();
    booleanNode* Next();
    booleanNode* GetCur();
    booleanNode* Index(int);

};

// Cast these functions to return booleanNodes instead of BaseNodes.

inline booleanNode* booleanList::First () {
    return (booleanNode*) BaseList::First();
}

inline booleanNode* booleanList::Last () {
    return (booleanNode*) BaseList::Last();
}

inline booleanNode* booleanList::Prev () {
    return (booleanNode*) BaseList::Prev();
}

inline booleanNode* booleanList::Next () {
    return (booleanNode*) BaseList::Next();
}

inline booleanNode* booleanList::GetCur () {
    return (booleanNode*) BaseList::GetCur();
}

inline booleanNode* booleanList::Index (int index) {
    return (booleanNode*) BaseList::Index(index);
}

#endif

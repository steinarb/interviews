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

// $Header: listcenter.h,v 1.8 89/10/09 14:48:38 linton Exp $
// declares classes CenterNode and CenterList.

#ifndef listcenter_h
#define listcenter_h

#include "list.h"

// A CenterNode contains two float values.

class CenterNode : public BaseNode {
public:

    CenterNode (float x, float y) { cx = x; cy = y; }
    float GetCx () { return cx; }
    float GetCy () { return cy; }

protected:

    float cx, cy;		// stores a position

};

// A CenterList manages a list of CenterNodes.

class CenterList : public BaseList {
public:

    CenterNode* First();
    CenterNode* Last();
    CenterNode* Prev();
    CenterNode* Next();
    CenterNode* GetCur();
    CenterNode* Index(int);

};

// Cast these functions to return CenterNodes instead of BaseNodes.

inline CenterNode* CenterList::First () {
    return (CenterNode*) BaseList::First();
}

inline CenterNode* CenterList::Last () {
    return (CenterNode*) BaseList::Last();
}

inline CenterNode* CenterList::Prev () {
    return (CenterNode*) BaseList::Prev();
}

inline CenterNode* CenterList::Next () {
    return (CenterNode*) BaseList::Next();
}

inline CenterNode* CenterList::GetCur () {
    return (CenterNode*) BaseList::GetCur();
}

inline CenterNode* CenterList::Index (int index) {
    return (CenterNode*) BaseList::Index(index);
}

#endif

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

// $Header: listipattern.h,v 1.9 89/10/09 14:48:53 linton Exp $
// declares classes IPatternNode and IPatternList.

#ifndef listipattern_h
#define listipattern_h

#include "list.h"

// Declare imported types.

class IPattern;

// An IPatternNode contains an IPattern pointer.

class IPatternNode : public BaseNode {
public:

    IPatternNode (IPattern* p) { pattern = p; }
    boolean SameValueAs (void* p) { return pattern == p; }
    IPattern* GetPattern () { return pattern; }

protected:

    IPattern* pattern;		// points to an IPattern

};

// An IPatternList manages a list of IPatternNodes.

class IPatternList : public BaseList {
public:

    IPatternNode* First();
    IPatternNode* Last();
    IPatternNode* Prev();
    IPatternNode* Next();
    IPatternNode* GetCur();
    IPatternNode* Index(int);

};

// Cast these functions to return IPatternNodes instead of BaseNodes.

inline IPatternNode* IPatternList::First () {
    return (IPatternNode*) BaseList::First();
}

inline IPatternNode* IPatternList::Last () {
    return (IPatternNode*) BaseList::Last();
}

inline IPatternNode* IPatternList::Prev () {
    return (IPatternNode*) BaseList::Prev();
}

inline IPatternNode* IPatternList::Next () {
    return (IPatternNode*) BaseList::Next();
}

inline IPatternNode* IPatternList::GetCur () {
    return (IPatternNode*) BaseList::GetCur();
}

inline IPatternNode* IPatternList::Index (int index) {
    return (IPatternNode*) BaseList::Index(index);
}

#endif

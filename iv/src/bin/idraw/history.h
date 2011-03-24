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

// $Header: history.h,v 1.7 89/10/09 14:48:07 linton Exp $
// declares class History.

#ifndef history_h
#define history_h

#include <InterViews/defs.h>

// Declare imported types.

class ChangeNode;
class ChangeList;
class Interactor;

// A History maintains a log of changes made to the drawing to permit
// changes to be undone.

class History {
public:

    History(Interactor*);
    ~History();

    boolean IsEmpty();
    void Clear();
    void Do(ChangeNode*);
    void Redo();
    void Undo();

protected:

    int maxhistory;		// maximum number of changes to store
    ChangeList* changelist;	// stores changes to drawing

};

#endif

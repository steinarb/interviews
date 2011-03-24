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

// $Header: listgroup.c,v 1.3 89/10/09 14:48:43 linton Exp $
// implements class GroupNode.

#include "listgroup.h"
#include "listselectn.h"
#include "selection.h"

// GroupNode stores the parent, boolean value, children, and the
// children's original graphic states.

GroupNode::GroupNode (PictSelection* p, boolean h, SelectionList* sl) {
    parent = p;
    haschildren = h;
    children = new SelectionList;
    childrengs = new SelectionList;
    for (sl->First(); !sl->AtEnd(); sl->Next()) {
	Selection* child = sl->GetCur()->GetSelection();
	Selection* childgs = new Selection(child);
	children->Append(new SelectionNode(child));
	childrengs->Append(new SelectionNode(childgs));
    }
}

// Free storage allocated to list the children and allocated
// for their graphic states.

GroupNode::~GroupNode () {
    delete children;
    for (childrengs->First(); !childrengs->AtEnd(); childrengs->Next()) {
	Selection* s = childrengs->GetCur()->GetSelection();
	delete s;
    }
    delete childrengs;
}

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

// $Header: list.c,v 1.9 89/10/09 14:48:32 linton Exp $
// implements classes BaseNode and BaseList.

#include "list.h"

// BaseNode zeroes its pointers.

BaseNode::BaseNode () {
    prev = nil;
    next = nil;
}

BaseNode::~BaseNode () {
    // no storage allocated by base class
}

// SameValueAs returns true if this class contains a data member of
// any pointer type that has the same value as the given pointer.

boolean BaseNode::SameValueAs (void*) {
    // base class contains no data members
    return false;
}

// BaseList starts with only a header node.

BaseList::BaseList () {
    cur = head = new BaseNode;
    head->prev = head->next = head;
    size = 0;
}

// ~BaseList destroys the list.

BaseList::~BaseList () {
    DeleteAll();
    delete head;
}

// Index returns the node that would be the indexed element if the
// list was a C array (0 = first, 1 = next, ..., size-1 = last) or nil
// if the index is out of bounds.  Index sets the current node to the
// indexed node if it's in the list; otherwise, the current node
// remains the same.

BaseNode* BaseList::Index (int index) {
    BaseNode* elem = nil;
    if (index >= 0 && index < size) {
	elem = head->next;
	for (int i = 0; i < index; i++) {
	    elem = elem->next;
	}
	cur = elem;
    }
    return elem;
}

// Find returns true if the list contains a node with a data member
// whose value is the same as the given pointer or false if there is
// no such node.  Find sets the current node to that node only if it
// finds such a node; otherwise, the current node remains the same.

boolean BaseList::Find (void* value) {
    for (BaseNode* elem = head->next; elem != head; elem = elem->next) {
	if (elem->SameValueAs(value)) {
	    cur = elem;
	    return true;
	}
    }
    return false;
}

// Append inserts a node after the last node of the list.  The current
// node remains the same.

void BaseList::Append (BaseNode* appendee) {
    BaseNode* last = head->prev;
    appendee->prev = last;
    appendee->next = head;
    head->prev = appendee;
    last->next = appendee;
    ++size;
}

// Prepend inserts a node before the first node of the list.  The
// current node remains the same.

void BaseList::Prepend (BaseNode* prependee) {
    BaseNode* first = head->next;
    prependee->prev = head;
    prependee->next = first;
    first->prev = prependee;
    head->next  = prependee;
    ++size;
}

// InsertAfterCur inserts a node after the current node of the list.
// The current node remains the same.

void BaseList::InsertAfterCur (BaseNode* prependee) {
    BaseNode* first = cur->next;
    prependee->prev = cur;
    prependee->next = first;
    first->prev = prependee;
    cur->next   = prependee;
    ++size;
}

// InsertBeforeCur inserts a node before the current node of the list.
// The current node remains the same.

void BaseList::InsertBeforeCur (BaseNode* appendee) {
    BaseNode* last = cur->prev;
    appendee->prev = last;
    appendee->next = cur;
    cur->prev  = appendee;
    last->next = appendee;
    ++size;
}

// RemoveCur removes the current node of the list (if it's a node and
// not the head) and sets the current node to the following node.

void BaseList::RemoveCur () {
    if (cur != head) {
	BaseNode* before = cur->prev;
	BaseNode* after  = cur->next;
	after->prev  = before;
	before->next = after;
	cur = after;
	--size;
    }
}

// DeleteCur deletes the current node of the list (if it's a node and
// not the head) and sets the current node to the following node.

void BaseList::DeleteCur () {
    if (cur != head) {
	BaseNode* before = cur->prev;
	BaseNode* after  = cur->next;
	after->prev  = before;
	before->next = after;
	delete cur;
	cur = after;
	--size;
    }
}

// DeleteAll deletes all the nodes of the list, making the list empty
// again, and sets the current node to the list's head.

void BaseList::DeleteAll () {
    BaseNode* after = nil;
    for (BaseNode* doomed = head->next; doomed != head; doomed = after) {
	after = doomed->next;
	delete doomed;
    }
    cur = head;
    head->prev = head->next = head;
    size = 0;
}

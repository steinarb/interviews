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

// $Header: history.c,v 1.8 89/10/09 14:48:05 linton Exp $
// implements class History.

#include "history.h"
#include "istring.h"
#include "listchange.h"
#include <InterViews/interactor.h>
#include <stdio.h>

// History sets its maximum number of changes and creates the history.

History::History (Interactor* i) {
    const char* def = i->GetAttribute("history");
    char* definition = strdup(def); // some sscanfs write to their format...
    if (sscanf(definition, "%d", &maxhistory) != 1) {
	maxhistory = 20;	// default if we can't parse definition
	fprintf(stderr, "can't parse attribute for history, ");
	fprintf(stderr, "value set to %d\n", maxhistory);
    }
    delete definition;
    changelist = new ChangeList;
}

// ~History frees storage allocated for the changes.

History::~History () {
    delete changelist;
}

// IsEmpty returns true if there are no changes stored in the list.

boolean History::IsEmpty () {
    return changelist->Size() == 0;
}

// Clear deletes all of the stored changes.  Calling Clear prevents
// dangling pointers from being referenced if the user tries to undo a
// stored change that was made before a New, Revert, or Open command.

void History::Clear () {
    changelist->DeleteAll();
}

// Do truncates the history at its current point, trims the history to
// its last maxhistory-1 changes, performs the change, appends the
// change, and leaves the current point at the end of the history.

void History::Do (ChangeNode* changenode) {
    while (!changelist->AtEnd()) {
	changelist->DeleteCur();
    }
    changelist->First();
    while (changelist->Size() >= maxhistory) {
	changelist->DeleteCur();
    }
    changenode->Do();
    changelist->Append(changenode);
    changelist->Last();
    changelist->Next();
}

// Redo redoes a stored change in the history and steps over it unless
// the current point is already at the end of the history.

void History::Redo () {
    if (!changelist->AtEnd()) {
	ChangeNode* changenode = changelist->GetCur();
	changenode->Do();
	changelist->Next();
    }
}

// Undo steps back in the history and undoes a stored change unless
// the current point is already at the beginning of the history.

void History::Undo () {
    changelist->Prev();
    if (!changelist->AtEnd()) {
	ChangeNode* changenode = changelist->GetCur();
	changenode->Undo();
    } else {
	changelist->Next();
    }
}

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

// $Header: errhandler.c,v 1.10 89/10/09 14:47:59 linton Exp $
// implements class ErrHandler.

#include "editor.h"
#include "errhandler.h"
#include <stdio.h>
#include <stdlib.h>

// Declare imported functions.

extern NewHandler* set_new_handler(NewHandler*);

// out_of_memory relays the out of memory exception to the current
// instance of ErrHandler.

void out_of_memory () {
    ErrHandler::_err_handler->OutOfMemory();
}

// ErrHandler doesn't have an Editor yet.

ErrHandler::ErrHandler () {
    editor = nil;
    olderr = nil;
    oldnew = nil;
}

// ~ErrHandler restores the previous handlers for out of memory exceptions.

ErrHandler::~ErrHandler () {
    SetErrHandler(olderr);
    set_new_handler(oldnew);
}

// SetEditor sets the Editor which ErrHandler calls upon.

void ErrHandler::SetEditor (Editor* e) {
    editor = e;
}

// Install installs this instance of ErrHandler as the handler for
// both X protocol request errors and out of memory exceptions.

ReqErr* ErrHandler::Install () {
    olderr = SetErrHandler(this);
    oldnew = set_new_handler(&out_of_memory);

    return ReqErr::Install();
}

// Error prints the X error, checkpoints the current drawing, and
// terminates the program's execution.

void ErrHandler::Error () {
    fprintf(stderr, "X Error: %s\n", message);
    fprintf(stderr, "         Request code: %d\n", request);
    fprintf(stderr, "         Request function: %d\n", detail);
    fprintf(stderr, "         Request window 0x%x\n", id);
    fprintf(stderr, "         Error Serial #%d\n", msgid);
    if (editor != nil) {
	editor->Checkpoint();
    }
    const int ERROR = 1;
    exit(ERROR);
}

// SetErrHandler updates the static variable read by out_of_memory

ErrHandler* ErrHandler::SetErrHandler (ErrHandler* n) {
    ErrHandler* o = _err_handler;
    _err_handler = n;
    return o;
}

// OutOfMemory aborts the program.  I wish it could checkpoint the
// drawing, but writing the drawing consumes a significant amount of
// memory.

void ErrHandler::OutOfMemory () {
    fprintf(stderr, "operator new failed: out of memory\n");
    abort();
}

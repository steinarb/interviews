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

// $Header: errhandler.h,v 1.8 89/10/09 14:48:01 linton Exp $
// declares class ErrHandler.

#ifndef errhandler_h
#define errhandler_h

#include <InterViews/reqerr.h>

// Declare imported types.

typedef void (NewHandler)();
class Editor;

// ErrHandler calls upon the Editor to save the current drawing if an
// X request error occurs.

class ErrHandler : public ReqErr {
public:

    ErrHandler();
    ~ErrHandler();

    void SetEditor(Editor*);
    ReqErr* Install();
    void Error();

protected:

    ErrHandler* SetErrHandler(ErrHandler*);
    void OutOfMemory();
    friend void out_of_memory();

    static ErrHandler* _err_handler; // used by out_of_memory

    Editor* editor;		     // handles drawing and editing operations
    ErrHandler* olderr;		     // used to restore previous handler
    NewHandler* oldnew;		     // used to restore previous handler

};

#endif

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

/*
 * X11-dependent request error handling
 */

#include <InterViews/defs.h>
#include <InterViews/reqerr.h>
#include <InterViews/X11/Xlib.h>
#include <InterViews/X11/Xutil.h>

static ReqErr* errhandler;

ReqErr::ReqErr() {
    /* no constructor code currently necessary */
}

ReqErr::~ReqErr() {
    if (errhandler == this) {
	errhandler = nil;
    }
}

void ReqErr::Error() {
    /* default is to do nothing */
}

static int DoXError(Display* errdisplay, XErrorEvent* e) {
    register ReqErr* r = errhandler;
    if (r != nil) {
	r->msgid = e->serial;
	r->code = e->error_code;
	r->request = e->request_code;
	r->detail = e->minor_code;
	r->id = (void*)e->resourceid;
	XGetErrorText(errdisplay, r->code, r->message, sizeof(r->message));
	r->Error();
    }
    return 0;
}

ReqErr* ReqErr::Install() {
    if (errhandler == nil) {
	XSetErrorHandler(DoXError);
    }
    ReqErr* r = errhandler;
    errhandler = this;
    return r;
}

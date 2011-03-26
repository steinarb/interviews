/*
 * Copyright (c) 1998 Vectaport Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

/*
 * collection of help functions
 */

#if !defined(_helpfunc_h)
#define _helpfunc_h

#include <ComTerp/comfunc.h>

class ComTerp;

class HelpFunc : public ComFunc {
public:
    HelpFunc(ComTerp*);
    virtual void execute();

    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "%s([command] [command...] :all) -- help for commands"; }
};

class SymIdFunc : public ComFunc {
public:
    SymIdFunc(ComTerp*);
    virtual void execute();

    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "%s(symbol [symbol...]) -- return id associated with symbol"; }
};

class SymValFunc : public ComFunc {
public:
    SymValFunc(ComTerp*);
    virtual void execute();

    virtual boolean post_eval() { return true; }
    virtual const char* docstring() { 
      return "%s(symid [symid...]) -- return symbol associated with id"; }
};

#endif /* !defined(_helpfunc_h) */

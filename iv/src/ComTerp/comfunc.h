/*
 * Copyright (c) 1994-1998 Vectaport Inc.
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
 * ComFunc is a simple action callback pointed to by a ComCom
 * (or possibly any other external function binding mechanism)
 */

#if !defined(_comfunc_h)
#define _comfunc_h

#include <stdlib.h>
#include <OS/types.h>
#include <ComTerp/comvalue.h>

class ComFuncState;
class ComTerp;
class ComTerpServ;

class ComFunc {
public:
    ComFunc(ComTerp*);
    virtual ~ComFunc() {}

    virtual void execute() = 0;

    int& nargs();
    int& nkeys();
    int& npops();
    int& pedepth();

    int nargsfixed() {return nargs() - nargskey();}
    int nargstotal() {return nargs() + nkeys();}
    int nargskey();
    int nargspost();
  
    ComFuncState* funcstate();
    void push_funcstate(int nargs, int nkeys, int pedepth=0);
    void pop_funcstate();

    ComTerp* comterp() { return _comterp; }
    ComTerpServ* comterpserv() { return (ComTerpServ*)_comterp; }

    ComValue& pop_stack();
    ComValue& pop_symbol();
    void push_stack(ComValue&);
    void push_stack(AttributeValue&);
    void reset_stack();

    ComValue& stack_arg(int n, boolean symbol=false, 
			ComValue& dflt=ComValue::nullval());

    ComValue& stack_key(int id, boolean symbol=false, 
			ComValue& dflt=ComValue::trueval(), 
			boolean use_dflt_for_no_key=false
			/* the antonym would be use_dflt_for_no_arg */);

    ComValue& stack_dotname(int n);

    ComValue& stack_arg_post_eval(int n, boolean symbol=false, 
				  ComValue& dflt=ComValue::nullval());

    ComValue& stack_key_post_eval(int id, boolean symbol=false, 
				  ComValue& dflt=ComValue::trueval(), 
				  boolean use_dflt_for_no_key=false);

    ComValue& stack_arg_post(int n, boolean symbol=false, 
			     ComValue& dflt=ComValue::nullval());

    ComValue& stack_key_post(int id, boolean symbol=false, 
			     ComValue& dflt=ComValue::trueval(), 
			     boolean use_dflt_for_no_key=false);

    ComValue& lookup_symval(ComValue&);
    void assign_symval(int id, ComValue*);

    virtual boolean post_eval() { return false; }
    virtual const char* docstring() { return "%s: no docstring method defined"; }
    static int bintest(const char* name);
    static boolean bincheck(const char* name);

protected:

    boolean skip_key_on_stack(int& stackptr, int& arglen);
    boolean skip_arg_on_stack(int& stackptr, int& arglen);
    boolean skip_key_in_expr(int& topptr, int& arglen);
    boolean skip_arg_in_expr(int& topptr, int& arglen);

    ComTerp* _comterp;

};

class ComFuncState {
public:
  ComFuncState(int nargs, int nkeys, int pedepth=0);
  ComFuncState(ComFuncState&);

  int& nargs() {return _nargs;}
  int& nkeys() {return _nkeys;}
  int& npops() { return _npops;}
  int& nargskey() { return _nargskey; }
  int& nargspost() { return _nargspost; }
  int& pedepth() { return _pedepth; }

protected:

  int _nargs;
  int _nkeys;
  int _npops;
  int _nargskey;
  int _nargspost;
  int _pedepth;
};
#endif /* !defined(_comfunc_h) */

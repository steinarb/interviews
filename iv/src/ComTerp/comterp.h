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
 * ComTerp is a command interpreter derived from the Parser
 */

#if !defined(_comterp_h)
#define _comterp_h

#include <ComTerp/parser.h>

#include <OS/table.h>
declareTable(ComValueTable,int,void*)


class AttributeList;
class AttributeValue;
class ComFunc;
class ComFuncState;
class ComValue;
class ostream;

class ComterpHandler;

class ComTerp : public Parser {
public:
    ComTerp();
    ComTerp(const char* path);
    ComTerp(void*, char*(*)(char*,int,void*), int(*)(void*), int(*)(void*));
    ~ComTerp();

    void init();

    boolean read_expr();
    boolean eof();

    virtual int eval_expr(boolean nested=false);
    virtual int post_eval_expr(int tokcnt, int offtop, int pedepth);

    int print_stack_top() const;
    int print_stack_top(ostream& out) const;
    int print_stack() const;
    int stack_height() { return _stack_top+1; }
    boolean brief() const;
    void brief(boolean flag) { _brief = flag; }

    int add_command(const char* name, ComFunc*);
    void list_commands(ostream& out, boolean sorted = false);
    int* get_commands(int &ncommands, boolean sorted = false);

    ComValue& pop_stack(boolean lookupsym=true);
    ComValue& lookup_symval(ComValue&);
    ComValue& stack_top(int n=0);
    ComValue& pop_symbol();
    void push_stack(ComValue&);
    void push_stack(AttributeValue&);
    void incr_stack();
    void incr_stack(int n);
    void decr_stack(int n=1);
    boolean stack_empty() { return _stack_top<0; }

    boolean skip_func(ComValue* topval, int& offset);
    boolean skip_key(ComValue* topval, int& offset, int& argcnt);
    boolean skip_arg(ComValue* topval, int& offset, int& argcnt);

    ComValue& expr_top(int n=0);

    static ComTerp& instance();

    void quit(boolean quitflag=true);
    boolean quitflag();
    virtual void exit(int status=0);

    virtual int run(boolean once=false);
    virtual int runfile(const char* filename);
    void add_defaults();

    ComValueTable* localtable() const { return _localtable; }
    ComValueTable* globaltable() const { return _globaltable; }
    ComValue* localvalue(int symid);
    ComValue* globalvalue(int symid);
    ComValue* eithervalue(int symid, boolean globalfirst=false);

    const char* errmsg() { return _errbuf; }

    void set_attributes(AttributeList*);
    AttributeList* get_attributes();

    void handler(ComterpHandler* h );
    ComterpHandler* handler();

    void disable_prompt();
    void enable_prompt();

    unsigned int& pfnum() { return _pfnum; }

protected:
    void push_stack(postfix_token*);
    void token_to_comvalue(postfix_token*, ComValue*);
    const ComValue* stack(unsigned int &top) const;
    void load_sub_expr();
    void load_postfix(postfix_token*, int toklen, int tokoff);
    void eval_expr_internals(int pedepth=0);

    ComFuncState* top_funcstate();
    void push_funcstate(ComFuncState& funcstate);
    void pop_funcstate();

protected:
    ComValue* _stack;
    int _stack_top;
    unsigned int _stack_siz;
    boolean _quitflag;
    char* _errbuf;
    int _pfoff;
    boolean _brief;
    boolean _just_reset;
    boolean _defaults_added;

    ComValueTable* _localtable;
    static ComValueTable* _globaltable;
    AttributeList* _alist;

    ComFuncState* _fsstack;
    int _fsstack_top;
    unsigned int _fsstack_siz;

    ComValue* _pfcomvals;

    static ComTerp* _instance;

    ComterpHandler* _handler;

    friend class ComFunc;
    friend class ComterpHandler;
    friend class ComTerpIOHandler;
};

#endif /* !defined(_comterp_h) */

/*
 * Copyright (c) 1994-1999 Vectaport Inc.
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
 * ComValue is the general object for interpreter values stored on the
 * stack and associated with global symbols.
 */

#if !defined(_comvalue_h)
#define _comvalue_h

#include <ComTerp/_comterp.h>
#include <Attribute/attrvalue.h>

class ComFunc;
class ComTerp;

//: AttributeValue with extensions for use with ComTerp.
// ComValue adds to AttributeValue a set of convenient static methods
// for returning constants useful in any application of ComTerp, as well
// as four integers (narg(), nkey(), nids(), pedepth()) used for storing
// necessary command and keyword state after code conversion and during 
// interpretation.  
class ComValue : public AttributeValue {
public:
    ComValue(ComValue&);
    // copy constructor.
    ComValue(AttributeValue&);
    // copy constructor from an AttributeValue.
    ComValue(postfix_token* token);
    // copy constructor from a postfix token generated by a Parser.
    ComValue(ValueType type);
    // construct of a given 'type' with no initial value.
    ComValue();
    // construct of UnknownType.

    ComValue(char val);
    // CharType constructor.
    ComValue(unsigned char val);
    // UCharType constructor.
    ComValue(short val);
    // ShortType constructor.
    ComValue(unsigned short val);
    // UShortType constructor.
    ComValue(int val, ValueType type=IntType);
    // IntType constructor or any other int-like value.
    ComValue(unsigned int val, ValueType type=IntType);
    // UIntType constructor or any other unsigned-int-like value including SymbolType.
    ComValue(unsigned int val, unsigned int, ValueType type=KeywordType);
    // KeywordType constructor (or can be used for ObjectType).
    ComValue(long val);
    // LongType constructor.
    ComValue(unsigned long val);
    // ULongType constructor.
    ComValue(float val);
    // FloatType constructor.
    ComValue(double val);
    // DoubleType constructor.
    ComValue(int class_symid, void* ptr);
    // ObjectType constructor.
    ComValue(AttributeValueList* listptr);
    // ArrayType constructor.
    ComValue(const char* val);
    // StringType constructor.
    ComValue(ComFunc* func);
    // CommandType constructor.

    void init();
    // initialize member variables.
    virtual ~ComValue();

    ComValue& operator= (const ComValue&);
    // assignment operator.

    void* geta(int type); 
    // return a pointer if ObjectType matches

    int narg() const;
    // number of arguments associated with this command or keyword.
    int nkey() const;
    // number of keywords associated with this command.
    int nids() const;
    // number of subordinate identifiers associated with this identifier (not used).
    int bquote() const;
    // return backquote flag
    void narg(int n) {_narg = n; }
    // set number of arguments associated with this command or keyword.
    void nkey(int n) {_nkey = n; }
    // set number of keywords associated with this command.
    void nids(int n) {_nids = n; }
    // set number of subordinate identifiers associated with this identifier (not used).
    void bquote(int flag) {_bquote = flag; }
    // set backquote flag

    int& pedepth() { return _pedepth; }
    // set/get depth of nesting in post-evaluated blocks of control commands.
    // Used by ComTerp to efficiently extract and execute post-evaluation commands.

    boolean unknown() { return ComValue::UnknownType == type(); }
    // return true if UnknownType.
    boolean null() { return unknown(); }
    // return true if UnknownType.
    boolean is_comfunc(int func_classid);
    // returns true if CommandType with ComFunc

    friend ostream& operator << (ostream& s, const ComValue&);
    // print contents to ostream, brief or not depending on
    // associated ComTerp brief flag.

    static void comterp(const ComTerp* comterp) { _comterp = comterp; }
    // set static pointer to ComTerp used to provide brief flag.
    static const ComTerp* comterp() { return _comterp; }
    // return static pointer to ComTerp used to provide brief flag.

    static ComValue& nullval();
    // returns reference to UnknownType ComValue.
    static ComValue& trueval();
    // returns reference to BooleanType ComValue with value of 1.
    static ComValue& falseval();
    // returns reference to BooleanType ComValue with value of 0.
    static ComValue& blankval();
    // returns reference to BlankType ComValue.  ComTerp places one of these
    // on the stack whenever the execution of a ComFunc places nothing on the stack.
    static ComValue& unkval();
    // returns reference to UnknownType ComValue.
    static ComValue& oneval();
    // returns reference to IntType ComValue with value of 1.
    static ComValue& zeroval();
    // returns reference to IntType ComValue with value of 0.
    static ComValue& minusoneval();
    // returns reference to IntType ComValue with value of -1.
protected:
    void zero_vals() { _narg = _nkey = _nids = _pedepth = _bquote = 0; }

    int _narg;
    int _nkey;
    int _nids;
    int _pedepth;
    int _bquote;

    static const ComTerp* _comterp;
    static ComValue _nullval;
    static ComValue _trueval;
    static ComValue _falseval;
    static ComValue _blankval;
    static ComValue _unkval;
    static ComValue _oneval;
    static ComValue _zeroval;
    static ComValue _minusoneval;
};

#endif /* !defined(_comvalue_h) */

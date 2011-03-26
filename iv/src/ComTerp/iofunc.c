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

#include <ComTerp/iofunc.h>
#include <ComTerp/comhandler.h>
#include <ComTerp/comvalue.h>
#include <ComTerp/comterp.h>
#include <Attribute/aliterator.h>
#include <Attribute/attrlist.h>
#include <iostream.h>
#include <strstream.h>

#define TITLE "IoFunc"

/*****************************************************************************/

PrintFunc::PrintFunc(ComTerp* comterp) : ComFunc(comterp) {
}

void PrintFunc::execute() {
  ComValue formatstr(stack_arg(0));
  ComValue printval(stack_arg(1));
  static int string_symid = symbol_add("string");
  ComValue stringflag(stack_key(string_symid));
  reset_stack();

  const char* fstr = formatstr.is_string() ? formatstr.string_ptr() : "nil";

  streambuf* strmbuf = nil;
  if (stringflag.is_false()) {
    filebuf * fbuf = new filebuf();
    strmbuf = fbuf;
    if (comterp()->handler()) {
      int fd = max(1, comterp()->handler()->get_handle());
      fbuf->attach(fd);
    } else
      fbuf->attach(fileno(stdout));
  } else {
    strmbuf = new strstreambuf();
  }
  ostream out(strmbuf);

  switch( printval.type() )
    {
    case ComValue::SymbolType:
    case ComValue::StringType:
      out.form(fstr, symbol_pntr( printval.symbol_ref()));
      break;
      
    case ComValue::BooleanType:
      out.form(fstr, printval.boolean_ref());
      break;
      
    case ComValue::CharType:
      out.form(fstr, printval.char_ref());
      break;	    
      
    case ComValue::UCharType:
      out.form(fstr, printval.uchar_ref());
      break;
	    
    case ComValue::IntType:
      out.form(fstr, printval.int_ref());
      break;
	    
    case ComValue::UIntType:
      out.form(fstr, printval.uint_ref());
      break;
	    
    case ComValue::LongType:
      out.form(fstr, printval.long_ref());
      break;
	    
    case ComValue::ULongType:
      out.form(fstr, printval.ulong_ref());
      break;
	    
    case ComValue::FloatType:
      out.form(fstr, printval.double_ref());
      break;
	    
    case ComValue::DoubleType:
      out.form(fstr, printval.double_ref());
      break;
	    
    case ComValue::ArrayType: 
      {
	ALIterator i;
	AttributeValueList* avl = printval.array_val();
	avl->First(i);
	boolean first = true;
	while (!avl->Done(i)) {
	  ComValue val(*avl->GetAttrVal(i));
	  push_stack(formatstr);
	  push_stack(val);
	  push_funcstate(2,0);
	  execute();
	  pop_funcstate();
	  avl->Next(i);
	  if (!avl->Done(i)) out << "\n";
	}
      }
      break;
	
    case ComValue::BlankType:
      out << "<blank>";
      break;

    case ComValue::UnknownType:
      out.form(fstr);
      break;
	    
    default:
      break;
    }


  if (stringflag.is_true()) {
    out << '\0';
    ComValue retval(((strstreambuf*)strmbuf)->str());
    push_stack(retval);
  }
  delete strmbuf;
    
}


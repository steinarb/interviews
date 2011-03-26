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

#if !defined(_comunidraw_nfunc_h)
#define _comunidraw_nfunc_h

#include <ComUnidraw/unifunc.h>

class NColsFunc : public UnidrawFunc {
public:
    NColsFunc(ComTerp*,Editor*);
    virtual void execute();
    virtual const char* docstring() { 
	return "%s() -- onscreen horizontal extent in pixels"; }

};

class NRowsFunc : public UnidrawFunc {
public:
    NRowsFunc(ComTerp*,Editor*);
    virtual void execute();
    virtual const char* docstring() { 
	return "%s() -- onscreen vertical extent in pixels"; }

};

class NFontsFunc : public UnidrawFunc {
public:
    NFontsFunc(ComTerp*,Editor*);
    virtual void execute();
    virtual const char* docstring() { 
	return "%s() -- return size of font menu"; }

};

class NBrushesFunc : public UnidrawFunc {
public:
    NBrushesFunc(ComTerp*,Editor*);
    virtual void execute();
    virtual const char* docstring() { 
	return "%s() -- return size of brush menu"; }

};

class NPatternsFunc : public UnidrawFunc {
public:
    NPatternsFunc(ComTerp*,Editor*);
    virtual void execute();
    virtual const char* docstring() { 
	return "%s() -- return size of pattern menu"; }

};

class NColorsFunc : public UnidrawFunc {
public:
    NColorsFunc(ComTerp*,Editor*);
    virtual void execute();
    virtual const char* docstring() { 
	return "%s() -- return size of colors menu"; }

};

#endif /* !defined(_comunidraw_nfunc_h) */

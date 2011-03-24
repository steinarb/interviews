/*
 * Copyright (c) 1991 Stanford University
 * Copyright (c) 1991 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Stanford and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Stanford and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

/*
 * FieldEditor -- simple editor for text fields
 */

#ifndef ivlook_field_h
#define ivlook_field_h

#include <InterViews/monoglyph.h>

class Action;
class FieldEditorImpl;
class String;
class Style;

class FieldEditor : public MonoGlyph {
public:
    FieldEditor(
	const char* sample, Style*, Action* ok = nil, Action* cancel = nil
    );
    FieldEditor(
	const String& sample, Style*, Action* ok = nil, Action* cancel = nil
    );
    virtual ~FieldEditor();

    void field(const char*);
    void field(const String&);

    void select(int pos);
    void select(int left, int right);

    void edit();
    void edit(const char*, int left, int right);
    void edit(const String&, int left, int right);

    const char* text() const;
    void text(String&) const;
private:
    FieldEditorImpl* impl_;
};

#endif

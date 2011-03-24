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

#include <IV-look/field.h>
#include <IV-look/kit.h>
#include <IV-look/2.6/InterViews/button.h>
#include <IV-look/2.6/InterViews/streditor.h>
#include <InterViews/action.h>
#include <OS/string.h>

class FieldButton : public ButtonState {
public:
    FieldButton(Action* ok, Action* cancel);
    virtual ~FieldButton();

    virtual void Notify();
private:
    Action* ok_;
    Action* cancel_;
};

class FieldEditorImpl {
private:
    friend class FieldEditor;

    StringEditor* editor_;
    FieldButton* bs_;
};

FieldEditor::FieldEditor(
    const char* sample, Style*, Action* ok, Action* cancel
) : MonoGlyph(nil) {
    impl_ = new FieldEditorImpl;
    impl_->bs_ = new FieldButton(ok, cancel);
    impl_->editor_ = new StringEditor(impl_->bs_, sample);
    body(impl_->editor_);
}

FieldEditor::FieldEditor(
    const String& sample, Style*, Action* ok, Action* cancel
) : MonoGlyph(nil) {
    impl_ = new FieldEditorImpl;
    impl_->bs_ = new FieldButton(ok, cancel);
    NullTerminatedString ns(sample);
    impl_->editor_ = new StringEditor(impl_->bs_, ns.string());
    body(impl_->editor_);
}

FieldEditor::~FieldEditor() {
    Resource::unref(impl_->bs_);
    delete impl_;
}

void FieldEditor::field(const char* str) {
    impl_->editor_->Message(str);
}

void FieldEditor::field(const String& s) {
    NullTerminatedString ns(s);
    impl_->editor_->Message(ns.string());
}

void FieldEditor::select(int pos) {
    impl_->editor_->Select(pos);
}

void FieldEditor::select(int l, int r) {
    impl_->editor_->Select(l, r);
}

void FieldEditor::edit() {
    impl_->editor_->Edit();
}

void FieldEditor::edit(const char* str, int left, int right) {
    impl_->editor_->Edit(str, left, right);
}

void FieldEditor::edit(const String& str, int left, int right) {
    NullTerminatedString ns(str);
    impl_->editor_->Edit(ns.string(), left, right);
}

const char* FieldEditor::text() const {
    return impl_->editor_->Text();
}

void FieldEditor::text(String& s) const {
    s = String(impl_->editor_->Text());
}

/** class FieldButton **/

FieldButton::FieldButton(Action* ok, Action* cancel) {
    ok_ = ok;
    Resource::ref(ok_);
    cancel_ = cancel;
    Resource::ref(cancel_);
}

FieldButton::~FieldButton() {
    Resource::unref(ok_);
    Resource::unref(cancel_);
}

void FieldButton::Notify() {
    int v;
    GetValue(v);
    if (v == '\r' && ok_ != nil) {
	ok_->execute();
    } else if (cancel_ != nil) {
	cancel_->execute();
    }
}

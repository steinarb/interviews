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
 * StringChooser implementation.
 */

#include <InterViews/button.h>
#include <InterViews/event.h>
#include <InterViews/sensor.h>
#include <InterViews/strbrowser.h>
#include <InterViews/strchooser.h>
#include <InterViews/streditor.h>

#include <string.h>

StringChooser::StringChooser (
    ButtonState* bs, int r, int c, const char* s, Alignment a
) : (bs, nil, a) { 
    Init(new StringEditor(bs, s), new StringBrowser(bs, r, c));
}

StringChooser::StringChooser (
    const char* name,
    ButtonState* bs, int r, int c, const char* s, Alignment a
) : (name, bs, nil, a) {
    Init(new StringEditor(bs, s), new StringBrowser(bs, r, c));
}

StringChooser::StringChooser (ButtonState* bs, Alignment a) : (bs, nil, a) { }

void StringChooser::Init (StringEditor* se, StringBrowser* sb) {
    input = new Sensor;
    input->Catch(KeyEvent);
    _sedit = se;
    _browser = sb;
}
    
void StringChooser::Select (int index) {
    if (index < 0) {
        _sedit->Select(strlen(_sedit->Text()));
    } else {
        _sedit->Select(index);
    }
}

void StringChooser::Select (int left, int right) { _sedit->Select(left, right);}

void StringChooser::SelectMessage () {
    _sedit->Select(0, strlen(_sedit->Text()));
}

void StringChooser::Message (const char* msg) { _sedit->Message(msg); }
const char* StringChooser::Choice () { return _sedit->Text(); }

void StringChooser::Forward (Event& e) {
    Coord x = e.x, y = e.y;

    e.target->GetRelative(x, y, this);
    if (x >= 0 && y >= 0 && x <= xmax && y <= ymax) {
        e.target->Handle(e);
    }
}

boolean StringChooser::Accept () {
    Event e;
    int v = 0;
    _focus = _sedit;
    state->SetValue(v);

    HandleFocus();
    state->GetValue(v);

    while (v == 0 || v == '\t') {
        if (v == '\t') {
            UpdateEditor();
            UpdateBrowser();
            SwitchFocus();
            HandleFocus();

        } else {
            Read(e);
            if (e.target != _focus && CanFocus(e.target)) {
                SwitchFocus();
            }
            Forward(e);
        }
        state->GetValue(v);
    }

    boolean accepted = v == '\r';

    if (accepted) {
        UpdateEditor();
        UpdateBrowser();
    }
    return accepted;
}

void StringChooser::Handle (Event& e) {
    _focus->Handle(e);
}

void StringChooser::SwitchFocus () {
    if (_focus == _sedit) {
        _focus = _browser;
        Select();
        state->SetValue(0);

    } else if (_focus == _browser) {
        _focus = _sedit;
        _browser->UnselectAll();
        state->SetValue(0);
    }
}

boolean StringChooser::CanFocus (Interactor* i) {
    return i == _sedit || i == _browser;
}

void StringChooser::HandleFocus () {
    if (_focus == _sedit) {
        _sedit->Edit();
    } else if (_focus == _browser) {
        _browser->Browse();
    }
}

void StringChooser::UpdateEditor () {
    int index = _browser->Selection();

    if (index >= 0) {
        _sedit->Message(_browser->String(index));
        SelectMessage();
    }
}

void StringChooser::UpdateBrowser () {
    _browser->UnselectAll();
}

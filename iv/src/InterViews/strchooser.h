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
 * StringChooser - a dialog box that provides a keyboard and mouse interface
 * to selecting from a list of strings.
 */

#ifndef strchooser_h
#define strchooser_h

#include <InterViews/dialog.h>

class StringBrowser;
class StringEditor;

class StringChooser : public Dialog {
public:
    StringChooser(
        ButtonState*, int rows, int cols, const char* sample, Alignment =Center
    );
    StringChooser(
        const char* name,
        ButtonState*, int, int, const char*, Alignment = Center
    );

    void Select(int = -1);
    void Select(int left, int right);
    void SelectMessage();
    void Message(const char*);

    virtual const char* Choice();
    virtual boolean Accept();
    virtual void Handle(Event&);
protected:
    StringChooser(ButtonState*, Alignment = Center);
    void Init(StringEditor*, StringBrowser*);

    virtual void SwitchFocus();
    virtual boolean CanFocus(Interactor*);
    virtual void HandleFocus();
    virtual void UpdateEditor();
    virtual void UpdateBrowser();
protected:
    StringEditor* _sedit;
    StringBrowser* _browser;
    Interactor* _focus;
private:
    void Forward(Event&);
};

#endif

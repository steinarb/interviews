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

// $Header: dialogbox.h,v 1.12 90/01/25 16:29:11 interran Exp $
// declares class DialogBox and DialogBox subclasses.

#ifndef dialogbox_h
#define dialogbox_h

#include <InterViews/filechooser.h>

// Declare imported types.

class ButtonState;
class IMessage;
class StringEditor;

// A DialogBox knows how to set its message and warning text and how
// to pop up itself over the underlying Interactor.

class DialogBox : public MonoScene {
public:

    void SetMessage(const char* = nil, const char* = nil);
    void SetWarning(const char* = nil, const char* = nil);
    void SetUnderlying(Interactor*);

protected:

    DialogBox(Interactor*, const char* = nil);

    void PopUp();
    void Disappear();

    IMessage* message;		// displays message text
    IMessage* warning;		// displays warning text
    Interactor* underlying;	// we'll insert ourselves into its parent

};

// A Messager displays a message until it's acknowledged.

class Messager : public DialogBox {
public:

    Messager(Interactor*, const char* = nil);
    ~Messager();

    void Display();

protected:

    void Init();
    void Reconfig();

    ButtonState* ok;		// stores status of "ok" button
    Interactor* okbutton;	// displays "ok" button

};

// A Confirmer displays a message until it's confirmed or cancelled.

class Confirmer : public DialogBox {
public:

    Confirmer(Interactor*, const char* = nil);
    ~Confirmer();

    char Confirm();

protected:

    void Init();
    void Reconfig();

    ButtonState* yes;		// stores status of "yes" button
    ButtonState* no;		// stores status of "no" button
    ButtonState* cancel;	// stores status of "cancel" button
    Interactor* yesbutton;	// displays "yes" button
    Interactor* nobutton;	// displays "no" button
    Interactor* cancelbutton;	// displays "cancel" button

};

// A Namer displays a string until it's edited or cancelled.

class Namer : public DialogBox {
public:

    Namer(Interactor*, const char* = nil);
    ~Namer();

    char* Edit(const char*);

protected:

    void Init();
    void Reconfig();

    ButtonState* accept;	// stores status of "accept" button
    ButtonState* cancel;	// stores status of "cancel" button
    Interactor* acceptbutton;	// displays "accept" button
    Interactor* cancelbutton;	// displays "cancel" button
    StringEditor* stringeditor;	// displays and edits a string

};

// A NamerNUnit displays a string until it's edited or cancelled and
// appends an unit to the string when returning the string.

class NamerNUnit : public DialogBox {
public:

    NamerNUnit(Interactor*, const char*, const char*, const char*);
    ~NamerNUnit();

    char* Edit(const char*);

protected:

    void Init();
    void Reconfig();

    ButtonState* accept;	// stores status of "accept" button
    ButtonState* cancel;	// stores status of "cancel" button
    ButtonState* unit;		// stores current unit
    Interactor* acceptbutton;	// displays "accept" button
    Interactor* cancelbutton;	// displays "cancel" button
    Interactor* unit1button;	// displays first unit button
    Interactor* unit2button;	// displays second unit button
    StringEditor* stringeditor;	// displays and edits a string

};

// A Finder browses the file system and returns a file name.

class Finder : public FileChooser {
public:

    Finder(Interactor*, const char*);

    const char* Find();

protected:

    Interactor* Interior();
    boolean Popup(Event&, boolean = true);

protected:

    Interactor* underlying;	// we'll insert ourselves into its parent

};

#endif

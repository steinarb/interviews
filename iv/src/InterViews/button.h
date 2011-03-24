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
 * A button is a view of some value that is normally set when
 * the button is pressed.
 */

#ifndef button_h
#define button_h

#include <InterViews/interactor.h>
#include <InterViews/subject.h>

class Button;
class ButtonList;
class Bitmap;

class ButtonState : public Subject {
public:
    ButtonState();
    ButtonState(int);
    ButtonState(void*);

    void operator=(ButtonState&);
    void SetValue(int);
    void SetValue(void*);
    void GetValue (int& v) { v = (int)value; }
    void GetValue (void*& v) { v = value; }
protected:
    void* value;

    void Modify(void*);
};

class Button : public Interactor {
public:
    void Attach(Button*);
    void Detach(Button*);

    void Enable();
    void Disable();

    void Choose();
    void UnChoose();

    virtual void Press();
    virtual void Refresh();

    void SetDimensions(int width, int height);

    virtual void Handle(Event&);
    virtual void Update();
protected:
    Button(ButtonState*, void*);
    Button(const char*, ButtonState*, void*);
    Button(Painter*, ButtonState*, void*);
    ~Button();

    void* value;		/* value associated with this button */
    ButtonState* subject;	/* set to this->value when pressed */
    ButtonList* associates;	/* enable/disable when chosen/unchosen */
    boolean enabled;		/* can be pressed */
    boolean chosen;		/* currently toggled on */
    boolean hit;		/* currently being pushed */
private:
    void Init(ButtonState*, void*);
};

class TextButton : public Button {
protected:
    const char* text;
    Painter* background;
    Painter* grayout;		/* for disabled buttons */

    TextButton(const char*, const char*, ButtonState*, void*);
    TextButton(const char*, ButtonState*, void*);
    TextButton(const char*, ButtonState*, void*, Painter*);
    ~TextButton();

    void MakeBackground();
    void MakeShape();
private:
    void Init(const char*);
};

class PushButton : public TextButton {
public:
    PushButton(const char*, ButtonState*, int);
    PushButton(const char*, ButtonState*, void*);
    PushButton(const char*, const char*, ButtonState*, int);
    PushButton(const char*, const char*, ButtonState*, void*);
    PushButton(const char*, ButtonState*, int, Painter*);
    PushButton(const char*, ButtonState*, void*, Painter*);

    virtual void Refresh();
protected:
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
private:
    void Init();
};

class RadioButton : public TextButton {
public:
    RadioButton(const char*, ButtonState*, int);
    RadioButton(const char*, ButtonState*, void*);
    RadioButton(const char*, const char*, ButtonState*, int);
    RadioButton(const char*, const char*, ButtonState*, void*);
    RadioButton(const char*, ButtonState*, int, Painter*);
    RadioButton(const char*, ButtonState*, void*, Painter*);

    virtual void Refresh();
protected:
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
private:
    void Init();
};

class CheckBox : public TextButton {
public:
    CheckBox(const char*, ButtonState*, int, int);
    CheckBox(const char*, ButtonState*, void*, void*);
    CheckBox(const char*, const char*, ButtonState*, int, int);
    CheckBox(const char*, const char*, ButtonState*, void*, void*);
    CheckBox(const char*, ButtonState*, int, int, Painter*);
    CheckBox(const char*, ButtonState*, void*, void*, Painter*);

    virtual void Press();
    virtual void Refresh();
    virtual void Update();
protected:
    virtual void Reconfig();
    virtual void Redraw(Coord, Coord, Coord, Coord);
private:
    void* offvalue;

    void Init(void*);
};

#endif

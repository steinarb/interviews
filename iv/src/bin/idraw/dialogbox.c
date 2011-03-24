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
 * $Header: dialogbox.c,v 1.18 90/01/25 16:29:08 interran Exp $
 * implements class DialogBox and DialogBox subclasses.
 */

#include "dialogbox.h"
#include "istring.h"
#include <InterViews/box.h>
#include <InterViews/button.h>
#include <InterViews/canvas.h>
#include <InterViews/event.h>
#include <InterViews/font.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/message.h>
#include <InterViews/painter.h>
#include <InterViews/sensor.h>
#include <InterViews/shape.h>
#include <InterViews/streditor.h>
#include <InterViews/world.h>

#include <os/fs.h>
#include <sys/param.h>

/*
 * An IMessage displays its own text, not somebody else's.
 */

class IMessage : public Message {
public:
    IMessage(const char* = nil, Alignment a = Center);
    ~IMessage();

    void SetText(const char* = nil, const char* = nil);
protected:
    char* buffer;		/* stores own copy of text */
};

/*
 * IMessage creates a buffer to store its own copy of the text.
 */

IMessage::IMessage (const char* msg, Alignment a) : (nil, a) {
    buffer = strdup(msg ? msg : "");
    text = buffer;
}

/*
 * Free storage allocated for the text buffer.
 */

IMessage::~IMessage () {
    delete buffer;
}

/*
 * SetText stores the new text and changes the IMessage's shape to fit
 * the new text's width.
 */

void IMessage::SetText (const char* beg, const char* end) {
    beg = beg ? beg : "";
    end = end ? end : "";
    delete buffer;
    buffer = new char[strlen(beg) + strlen(end) + 1];
    strcpy(buffer, beg);
    strcat(buffer, end);
    text = buffer;
    if (canvas != nil && canvas->Status() == CanvasMapped) {
	Reconfig();
	Parent()->Change(this);
    }
}

/*
 * DialogBox creates two IMessages to display a message and a warning
 * and stores its underlying Interactor.  DialogBox won't delete the
 * IMessages so its derived classes can put them in boxes which will
 * delete them when the boxes are deleted.
 */

DialogBox::DialogBox (Interactor* u, const char* msg) {
    SetCanvasType(CanvasSaveUnder); /* speed up expose redrawing if possible */
    input = allEvents;
    input->Reference();
    message = new IMessage(msg);
    warning = new IMessage;
    underlying = u;
}

/*
 * SetMessage sets the message's text.
 */

void DialogBox::SetMessage (const char* beg, const char* end) {
    message->SetText(beg, end);
}

/*
 * SetWarning sets the warning's text.
 */

void DialogBox::SetWarning (const char* beg, const char* end) {
    warning->SetText(beg, end);
}

/*
 * SetUnderlying sets the underlying Interactor over which the
 * DialogBox will pop up itself.
 */

void DialogBox::SetUnderlying (Interactor* u) {
    underlying = u;
}

/*
 * PopUp pops up the DialogBox centered over the underlying
 * Interactor's canvas.
 */

void DialogBox::PopUp () {
    World* world = underlying->GetWorld();
    Coord x, y;
    underlying->Align(Center, 0, 0, x, y);
    underlying->GetRelative(x, y, world);
    world->InsertTransient(this, underlying, x, y, Center);
}

/*
 * Disappear removes the DialogBox.  Since the user should see
 * warnings only once, Disappear clears the warning's text so the next
 * PopUp won't display it.
 */

void DialogBox::Disappear () {
    Parent()->Remove(this);
    SetWarning();
    Sync();
}

/*
 * Messager creates its button state and initializes its view.
 */

Messager::Messager (Interactor* u, const char* msg) : (u, msg) {
    ok = new ButtonState(false);
    okbutton = new PushButton("  OK  ", ok, true);
    Init();
}

/*
 * Free storage allocated for the message's button state.
 */

Messager::~Messager () {
    Unref(ok);
}

/*
 * Display pops up the Messager and removes it when the user
 * acknowledges the message.
 */

void Messager::Display () {
    ok->SetValue(false);

    PopUp();

    int okay = false;
    while (!okay) {
	Event e;
	Read(e);
	if (e.eventType == KeyEvent && e.len > 0) {
	    switch (e.keystring[0]) {
	    case '\r':		/* CR */
	    case '\007':	/* ^G */
		ok->SetValue(true);
		break;
	    default:
		break;
	    }
	} else if (e.target == okbutton) {
	    e.target->Handle(e);
	}
	ok->GetValue(okay);
    }

    Disappear();
}

/*
 * Init composes Messager's view with boxes, glue, and frames.
 */

void Messager::Init () {
    SetClassName("Messager");

    VBox* vbox = new VBox;
    vbox->Align(Center);
    vbox->Insert(new VGlue);
    vbox->Insert(warning);
    vbox->Insert(new VGlue);
    vbox->Insert(message);
    vbox->Insert(new VGlue);
    vbox->Insert(okbutton);
    vbox->Insert(new VGlue);

    Insert(new Frame(vbox, 2));
}

/*
 * Reconfig pads Messager's shape to make the view look less crowded.
 */

void Messager::Reconfig () {
    DialogBox::Reconfig();
    Font* font = output->GetFont();
    shape->width += 2 * font->Width("mmmm");
    shape->height += 4 * font->Height();
}

/*
 * Confirmer creates its button states and initializes its view.
 */

Confirmer::Confirmer (Interactor* u, const char* prompt) : (u, prompt) {
    yes		 = new ButtonState(false);
    no		 = new ButtonState(false);
    cancel	 = new ButtonState(false);
    yesbutton    = new PushButton(" Yes  ", yes, true);
    nobutton     = new PushButton("  No  ", no, true);
    cancelbutton = new PushButton("Cancel", cancel, true);
    Init();
}

/*
 * Free storage allocated for the button states.
 */

Confirmer::~Confirmer () {
    Unref(yes);
    Unref(no);
    Unref(cancel);
}

/*
 * Confirm pops up the Confirmer, lets the user confirm the message or
 * not, removes the Confirmer, and returns the confirmation.
 */

char Confirmer::Confirm () {
    yes->SetValue(false);
    no->SetValue(false);
    cancel->SetValue(false);

    PopUp();

    int confirmed = false;
    int denied = false;
    int cancelled = false;
    while (!confirmed && !denied && !cancelled) {
	Event e;
	Read(e);
	if (e.eventType == KeyEvent && e.len > 0) {
	    switch (e.keystring[0]) {
	    case 'y':
	    case 'Y':
		yes->SetValue(true);
		break;
	    case 'n':
	    case 'N':
		no->SetValue(true);
		break;
	    case '\r':		/* CR */
	    case '\007':	/* ^G */
		cancel->SetValue(true);
		break;
	    default:
		break;
	    }
	} else if (e.target == yesbutton || e.target == nobutton ||
		   e.target == cancelbutton)
	{
	    e.target->Handle(e);
	}
	yes->GetValue(confirmed);
	no->GetValue(denied);
	cancel->GetValue(cancelled);
    }

    Disappear();

    char answer = 'n';
    answer = confirmed ? 'y' : answer;
    answer = cancelled ? 'c' : answer;
    return answer;
}

/*
 * Init composes Confirmer's view with boxes, glue, and frames.
 */

void Confirmer::Init () {
    SetClassName("Confirmer");

    HBox* buttons = new HBox;
    buttons->Insert(new HGlue);
    buttons->Insert(yesbutton);
    buttons->Insert(new HGlue);
    buttons->Insert(nobutton);
    buttons->Insert(new HGlue);
    buttons->Insert(cancelbutton);
    buttons->Insert(new HGlue);

    VBox* vbox = new VBox;
    vbox->Align(Center);
    vbox->Insert(new VGlue);
    vbox->Insert(warning);
    vbox->Insert(new VGlue);
    vbox->Insert(message);
    vbox->Insert(new VGlue);
    vbox->Insert(buttons);
    vbox->Insert(new VGlue);

    Insert(new Frame(vbox, 2));
}

/*
 * Reconfig pads Confirmer's shape to make the view look less crowded.
 */

void Confirmer::Reconfig () {
    DialogBox::Reconfig();
    Font* font = output->GetFont();
    shape->width += 4 * font->Width("mmmm");
    shape->height += 4 * font->Height();
}

/*
 * Namer creates its button states and initializes its view.
 */

Namer::Namer (Interactor* u, const char* prompt) : (u, prompt) {
    accept = new ButtonState(false);
    cancel = new ButtonState(false);
    acceptbutton = new PushButton("  OK  ", accept, true);
    cancelbutton = new PushButton("Cancel", cancel, true);
    const char* sample = "                                                 ";
    stringeditor = new StringEditor(accept, sample, "\007\015");
    stringeditor->Message("");
    Init();
}

/*
 * Free storage allocated for the button states.
 */

Namer::~Namer () {
    Unref(accept);
    Unref(cancel);
}

/*
 * Edit pops up the Namer, lets the user edit the given string,
 * removes the Namer, and returns the edited string unless the user
 * cancelled it.
 */

char* Namer::Edit (const char* string) {
    accept->SetValue(false);
    cancel->SetValue(false);
    if (string != nil) {
	stringeditor->Message(string);
    }
    stringeditor->Select(0, strlen(stringeditor->Text()));

    PopUp();

    int accepted = false;
    int cancelled = false;
    while (!accepted && !cancelled) {
	stringeditor->Edit();
	accept->GetValue(accepted);
	if (accepted == '\007') {
	    accept->SetValue(false);
	    cancel->SetValue(true);
	} else if (accepted == '\015') {
	    accept->SetValue(true);
	    cancel->SetValue(false);
	} else {
	    Event e;
	    Read(e);
	    if (e.target == acceptbutton || e.target == cancelbutton) {
		e.target->Handle(e);
	    }
	}
	accept->GetValue(accepted);
	cancel->GetValue(cancelled);
    }

    Disappear();

    char* result = nil;
    if (accepted) {
	const char* text = stringeditor->Text();
	if (text[0] != '\0') {
	    result = strdup(text);
	}
    }
    return result;
}

/*
 * Init composes Namer's view with boxes, glue, and frames.
 */

void Namer::Init () {
    SetClassName("Namer");

    HBox* hboxedit = new HBox;
    hboxedit->Insert(new HGlue(5, 0, 0));
    hboxedit->Insert(stringeditor);
    hboxedit->Insert(new HGlue(5, 0, 0));

    VBox* vboxedit = new VBox;
    vboxedit->Insert(new VGlue(2, 0, 0));
    vboxedit->Insert(hboxedit);
    vboxedit->Insert(new VGlue(2, 0, 0));

    HBox* buttons = new HBox;
    buttons->Insert(new HGlue);
    buttons->Insert(acceptbutton);
    buttons->Insert(new HGlue);
    buttons->Insert(cancelbutton);
    buttons->Insert(new HGlue);

    VBox* vbox = new VBox;
    vbox->Align(Center);
    vbox->Insert(new VGlue);
    vbox->Insert(warning);
    vbox->Insert(new VGlue);
    vbox->Insert(message);
    vbox->Insert(new VGlue);
    vbox->Insert(new Frame(vboxedit, 1));
    vbox->Insert(new VGlue);
    vbox->Insert(buttons);
    vbox->Insert(new VGlue);
    vbox->Propagate(false);	/* for reshaping stringeditor w/o looping */

    Insert(new Frame(vbox, 2));
}

/*
 * Reconfig pads Namer's shape to make the view look less crowded.
 */

void Namer::Reconfig () {
    DialogBox::Reconfig();
    Shape s = *stringeditor->GetShape();
    s.Rigid();
    stringeditor->Reshape(s);
    Font* font = output->GetFont();
    shape->width += 2 * font->Width("mmmm");
    shape->height += 4 * font->Height();
}

/*
 * NamerNUnit creates its button states and initializes its view.
 */

NamerNUnit::NamerNUnit (
    Interactor* u, const char* prompt, const char* label1, const char* label2
) : (u, prompt) {
    accept = new ButtonState(false);
    cancel = new ButtonState(false);
    unit = new ButtonState((void*)label1);
    acceptbutton = new PushButton("  OK  ", accept, true);
    cancelbutton = new PushButton("Cancel", cancel, true);
    unit1button = new RadioButton(label1, unit, (void*)label1);
    unit2button = new RadioButton(label2, unit, (void*)label2);
    const char* sample = "                         ";
    stringeditor = new StringEditor(accept, sample, "\007\015");
    stringeditor->Message("");
    Init();
}

/*
 * Free storage allocated for the button states.
 */

NamerNUnit::~NamerNUnit () {
    Unref(accept);
    Unref(cancel);
    Unref(unit);
}

/*
 * Edit pops up the NamerNUnit, lets the user edit the given string,
 * removes the NamerNUnit, and returns the edited string with the selected
 * unit appended unless the user cancelled the string.
 */

char* NamerNUnit::Edit (const char* string) {
    accept->SetValue(false);
    cancel->SetValue(false);
    if (string != nil) {
	stringeditor->Message(string);
    }
    stringeditor->Select(0, strlen(stringeditor->Text()));

    PopUp();

    int accepted = false;
    int cancelled = false;
    while (!accepted && !cancelled) {
	stringeditor->Edit();
	accept->GetValue(accepted);
	if (accepted == '\007') {
	    accept->SetValue(false);
	    cancel->SetValue(true);
	} else if (accepted == '\015') {
	    accept->SetValue(true);
	    cancel->SetValue(false);
	} else {
	    Event e;
	    Read(e);
	    if (
		e.target == acceptbutton || e.target == cancelbutton ||
		e.target == unit1button || e.target == unit2button
	    ) {
		e.target->Handle(e);
	    }
	}
	accept->GetValue(accepted);
	cancel->GetValue(cancelled);
    }

    Disappear();

    char* result = nil;
    if (accepted) {
	const char* text = stringeditor->Text();
	if (text[0] != '\0') {
	    void* v;
	    unit->GetValue(v);
	    const char* u = (char*)v;
	    result = new char[strlen(text) + 1 + strlen(u) + 1];
	    strcpy(result, text);
	    strcat(result, " ");
	    strcat(result, u);
	}
    }
    return result;
}

/*
 * Init composes NamerNUnit's view with boxes, glue, and frames.
 */

void NamerNUnit::Init () {
    SetClassName("NamerNUnit");

    HBox* hbox = new HBox;
    hbox->Align(Center);
    hbox->Insert(new HGlue);
    hbox->Insert(new Frame(new MarginFrame(stringeditor, 5, 2)));
    hbox->Insert(new HGlue);
    hbox->Insert(unit1button);
    hbox->Insert(new HGlue);
    hbox->Insert(unit2button);
    hbox->Insert(new HGlue);

    HBox* buttons = new HBox;
    buttons->Insert(new HGlue);
    buttons->Insert(acceptbutton);
    buttons->Insert(new HGlue);
    buttons->Insert(cancelbutton);
    buttons->Insert(new HGlue);

    VBox* vbox = new VBox;
    vbox->Align(Center);
    vbox->Insert(new VGlue);
    vbox->Insert(warning);
    vbox->Insert(new VGlue);
    vbox->Insert(message);
    vbox->Insert(new VGlue);
    vbox->Insert(hbox);
    vbox->Insert(new VGlue);
    vbox->Insert(buttons);
    vbox->Insert(new VGlue);
    vbox->Propagate(false);	/* for reshaping stringeditor w/o looping */

    Insert(new Frame(vbox, 2));
}

/*
 * Reconfig pads NamerNUnit's shape to make the view look less crowded.
 */

void NamerNUnit::Reconfig () {
    DialogBox::Reconfig();
    Shape s = *stringeditor->GetShape();
    s.Rigid();
    stringeditor->Reshape(s);
    Font* font = output->GetFont();
    shape->width += 2 * font->Width("mmmm");
    shape->height += 4 * font->Height();
}

/*
 * Helper function for Finder.
 */

static const char* abspath (const char* file = nil) {
    const int bufsize = MAXPATHLEN+1;
    static char buf[bufsize];

    getcwd(buf, bufsize);
    strcat(buf, "/");

    if (file != nil) {
        strcat(buf, file);
    }
    return buf;
}

Finder::Finder (
    Interactor* u, const char* t
) : (new ButtonState, abspath(), 10, 24, Center) {
    underlying = u;
    Init("", t);
    Insert(Interior());
}

static const char* ChdirIfNecessary (Finder* finder) {
    static char buf[MAXPATHLEN+1];
    const char* filename = finder->Choice();
    strcpy(buf, filename);
    char* bufptr = strrchr(buf, '/');

    if (bufptr != NULL) {
        *bufptr = '\0';
        if (chdir(buf) == 0) {
            filename = ++bufptr;
            finder->Message(abspath(filename));
        }
    }
    finder->SelectFile();
    return filename;
}            

const char* Finder::Find () {
    const char* name = nil;
    Event e;
    if (Popup(e)) {
	name = ChdirIfNecessary(this);
    }
    return name;
}

Interactor* Finder::Interior () {
    return new Frame(FileChooser::Interior(" Open "), 2);
}

boolean Finder::Popup (Event&, boolean) {
    World* world = underlying->GetWorld();
    Coord x, y;
    underlying->Align(Center, 0, 0, x, y);
    underlying->GetRelative(x, y, world);
    world->InsertTransient(this, underlying, x, y, Center);
    boolean accepted = Accept();
    world->Remove(this);
    SetTitle("");
    return accepted;
}

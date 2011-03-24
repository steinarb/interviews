/*
 * Copyright (c) 1989 Stanford University
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
 * Implementation of various dialogs boxes.
 */

#include "dialogs.h"

#include <InterViews/adjuster.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/button.h>
#include <InterViews/filebrowser.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/message.h>
#include <InterViews/scroller.h>
#include <InterViews/sensor.h>
#include <InterViews/streditor.h>

#include <string.h>

/*****************************************************************************/

static const float fspace = .375;                // space in cm

/*****************************************************************************/

BasicDialog::BasicDialog () : (new ButtonState, nil) {
    input = new Sensor;
    input->Catch(KeyEvent);
    input->Reference();
}

boolean BasicDialog::Accept () {
    Event e;
    int v = 0;

    state->SetValue(0);

    do {
	Read(e);
        if (!KeyEquiv(e)) {
            Forward(e);
        }
        state->GetValue(v);
    } while (v == 0);

    return v == '\r';
}

void BasicDialog::Forward (Event& e) {
    Coord x = e.x, y = e.y;

    e.target->GetRelative(x, y, this);
    if (x >= 0 && y >= 0 && x <= xmax && y <= ymax) {
        e.target->Handle(e);
    }
}

boolean BasicDialog::KeyEquiv (Event& e) {
    boolean keyEquiv = false;

    if (e.eventType == KeyEvent && e.len > 0) {
        char c = e.keystring[0];

        if (c == '\r' || c == '\007' || c == '\033') {
            state->SetValue(c);
            keyEquiv = true;
        }
    }
    return keyEquiv;
}

/*****************************************************************************/

AcknowledgeDialog::AcknowledgeDialog (const char* msg, const char* btnLbl) {
    int space = round(fspace*cm);

    Insert(
        new MarginFrame(
            new VBox(
                new Message(msg),
                new VGlue(space, space, 0),
                new HBox(
                    new HGlue,
                    new PushButton(btnLbl, state, '\r'),
                    new HGlue
                )
            ), space, space, 0
        )
    );
}

void AcknowledgeDialog::Acknowledge () {
    Event e;
    int v = 0;

    state->SetValue(v);

    do {
	Read(e);
        if (!KeyEquiv(e)) {
            Forward(e);
        }
	state->GetValue(v);
    } while (v == 0);
}

/*****************************************************************************/

ConfirmDialog::ConfirmDialog (const char* msg, const char* confirmLbl) {
    int space = round(fspace*cm);

    Insert(
        new MarginFrame(
            new VBox(
                new Message(msg),
                new VGlue(space, space, 0),
                new HBox(
                    new HGlue,
                    new PushButton(confirmLbl, state, '\r'),
                    new HGlue(space, space, 0),
                    new PushButton(" Cancel ", state, '\007'),
                    new HGlue
                )
            ), space, space, 0
        )
    );
}

/*****************************************************************************/

StringDialog::StringDialog (
    const char* msg, const char* sample, const char* confirmLbl
) {
    Init(msg, confirmLbl);
    _sedit->Message(sample);
}

StringDialog::StringDialog (
    const char* msg, int width, const char* confirmLbl
) {
    Init(msg, confirmLbl, width);
}

void StringDialog::Select () {
    _sedit->Select(0, strlen(_sedit->Text()));
}

void StringDialog::Select (int pos) { _sedit->Select(pos); }
void StringDialog::Select (int left, int right) { _sedit->Select(left, right);}

void StringDialog::Init (const char* msg, const char* confirmLbl, int width) {
    int space = round(fspace*cm);
    _sedit = new StringEditor(state, "");
    HBox* framedSedit = new HBox;

    if (width == 0) {
        framedSedit->Insert(_sedit);

    } else {
        framedSedit->Insert(new HGlue);
        framedSedit->Insert(
            new VBox(
                new Frame(new MarginFrame(_sedit, 2)),
                new HGlue(width, 0, 0)
            )
        );
        framedSedit->Insert(new HGlue);
    }

    Insert(
        new MarginFrame(
            new VBox(
                new Message(msg),
                new VGlue(space, space, 0),
                framedSedit,
                new VGlue(space, space, 0),
                new HBox(
                    new HGlue,
                    new PushButton(confirmLbl, state, '\r'),
                    new HGlue(space, space, 0),
                    new PushButton(" Cancel ", state, '\007')
                )
            ), space, space, 0
        )
    );
}

const char* StringDialog::String () { return _sedit->Text(); }

boolean StringDialog::Accept () {
    Event e;
    int v = 0;

    state->SetValue(0);
    Select();
    _sedit->Edit();

    for (;;) {
        state->GetValue(v);
        if (v != 0) {
            break;
        }
	Read(e);
        if (!KeyEquiv(e)) {
            Forward(e);
        }
    }

    return v == '\r';
}

/*****************************************************************************/

FileDialog::FileDialog (
    const char* msg, const char* dir, const char* confirmLbl
) {
    int space = round(fspace*cm);
    _browser = new FileBrowser(state, dir, 10, 24, false);

    Insert(
        new MarginFrame(
            new VBox(
                new Message(msg),
                new VGlue(space, space, 0),
                new Frame(AddScroller(_browser)),
                new VGlue(space, space, 0),
                new HBox(
                    new HGlue,
                    new PushButton(confirmLbl, state, '\r'),
                    new HGlue(space, space, 0),
                    new PushButton(" Cancel ", state, '\007')
                )
            ), space, space, 0
        )
    );
}

Interactor* FileDialog::AddScroller (Interactor* i) {
    return new HBox(
        new MarginFrame(i, 2),
        new VBorder,
        new VBox(
            new UpMover(i, 1),
            new HBorder,
            new VScroller(i),
            new HBorder,
            new DownMover(i, 1)
        )                
    );
}

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
 * FileChooser implementation.
 */

#include <InterViews/adjuster.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/button.h>
#include <InterViews/event.h>
#include <InterViews/filebrowser.h>
#include <InterViews/filechooser.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/message.h>
#include <InterViews/scroller.h>
#include <InterViews/sensor.h>
#include <InterViews/streditor.h>

#include <string.h>

FileChooser::FileChooser (
    const char* t, const char* subt, const char* d, 
    int r, int c, const char* acceptLabel, Alignment a
) : (new ButtonState, a) {
    FileBrowser* fb = new FileBrowser(state, d, r, c);
    StringChooser::Init(new StringEditor(state, fb->GetDirectory()), fb);
    Init(t, subt);
    Insert(Interior(acceptLabel));
}

FileChooser::FileChooser (
    const char* name, const char* t, const char* subt, const char* d, 
    int r, int c, const char* acceptLabel, Alignment a
) : (new ButtonState, a) {
    SetInstance(name);
    FileBrowser* fb = new FileBrowser(state, d, r, c);
    StringChooser::Init(new StringEditor(state, fb->GetDirectory()), fb);
    Init(t, subt);
    Insert(Interior(acceptLabel));
}

FileChooser::FileChooser (
    ButtonState* bs, const char* d, int r, int c, Alignment a
) : (bs, a) {
    FileBrowser* fb = new FileBrowser(state, d, r, c);
    StringChooser::Init(new StringEditor(state, fb->GetDirectory()), fb);
}    

FileChooser::FileChooser (ButtonState* bs, Alignment a) : (bs, a) { }

void FileChooser::Init (const char* t, const char* subt) {
    if (*t == '\0') {
        title = new MarginFrame(new VGlue(0, 0));
    } else {
        title = new MarginFrame(new Message(t));
    }
    if (*subt == '\0') {
        subtitle = new MarginFrame(new VGlue(0, 0));
    } else {
        subtitle = new MarginFrame(new Message(subt));
    }
}

void FileChooser::SelectFile () {
    const char* path = _sedit->Text();
    int left = strlen(browser()->ValidDirectories(path));
    int right = strlen(path);

    Select(left, right);
}

void FileChooser::UpdateEditor () {
    int index = browser()->Selection();

    if (index >= 0) {
        _sedit->Message(browser()->Path(index));
        browser()->UnselectAll();
    } else {
        _sedit->Message(browser()->Normalize(_sedit->Text()));
    }
    SelectFile();
}

void FileChooser::UpdateBrowser () {
    browser()->SetDirectory(Choice());
}

boolean FileChooser::Accept () {
    boolean accepted, dirSelected;

    do {
        accepted = StringChooser::Accept();
        dirSelected = browser()->IsADirectory(Choice());
    } while (accepted && dirSelected);

    return accepted;
}

static void ChangeMsg (const char* name, MarginFrame* frame) {
    Interactor* msg;

    if (*name == '\0') {
        msg = new VGlue(0, 0);
    } else {
        msg = new Message(name);
    }
    frame->Insert(msg);
    frame->Change(msg);
}

void FileChooser::SetTitle (const char* name) {
    ChangeMsg(name, title);
}

void FileChooser::SetSubtitle (const char* name) {
    ChangeMsg(name, subtitle);
}

Interactor* FileChooser::AddScroller (Interactor* i) {
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

Interactor* FileChooser::Interior (const char* acptLbl) {
    const int space = round(.5*cm);
    VBox* titleblock = new VBox(
        new HBox(title, new HGlue),
        new HBox(subtitle, new HGlue)
    );

    return new MarginFrame(
        new VBox(
            titleblock,
            new VGlue(space, 0),
            new Frame(new MarginFrame(_sedit, 2)),
            new VGlue(space, 0),
            new Frame(AddScroller(browser())),
            new VGlue(space, 0),
            new HBox(
                new VGlue(space, 0),
                new HGlue,
                new PushButton("Cancel", state, '\007'),
                new HGlue(space, 0),
                new PushButton(acptLbl, state, '\r')
            )
        ), space, space/2, 0
    );
}

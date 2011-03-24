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
 * sted - a simple text editor
 */

#include <InterViews/adjuster.h>
#include <InterViews/border.h>
#include <InterViews/box.h>
#include <InterViews/button.h>
#include <InterViews/glue.h>
#include <InterViews/regexp.h>
#include <InterViews/painter.h>
#include <InterViews/scene.h>
#include <InterViews/scroller.h>
#include <InterViews/sensor.h>
#include <InterViews/streditor.h>
#include <InterViews/textbuffer.h>
#include <InterViews/texteditor.h>
#include <InterViews/world.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

static const int MINTEXTSIZE = 10000;
static const int BUFFERSIZE = 100;
static const int FILENAMESIZE = 100;

class StedWindow;

struct StedWindowInfo {
public:
    StedWindow* window;
    boolean closed;
    Coord x, y;
    StedWindowInfo* next;
};

class Sted {
public:
    Sted(World* world);
    ~Sted();
    void Open(const char*);
    void Close(const char*);
    void Quit();
    void Run();
    void Complain();
private:
    StedWindowInfo* Find(const char*);

    World* world;
    StedWindowInfo* windows;
    boolean done;
};

class StedWindow : public MonoScene {
public:
    StedWindow(Sted*, const char* filename);
    virtual ~StedWindow();
    const char* Filename();

    virtual void Handle(Event&);
protected:
    virtual void Update();
protected:
    void Edit(const char* filename);
    void Do(const char*);
    void Ask(const char*, int, int);

    void HandleChar(char);
    void LeftMouse(Event&);

    Sted* sted;
    char lastchar;
    char filename[FILENAMESIZE];
    boolean modified;
    boolean prefix1;
    boolean prefix2;
    char* buffer;
    int size;
    TextBuffer* text;
    TextEditor* editor;
    StringEditor* command;
    ButtonState* state;
};

Sted::Sted (World* w) {
    world = w;
    windows = nil;
    done = false;
}

Sted::~Sted () {
    StedWindowInfo* w = windows;
    while (w != nil) {
        StedWindowInfo* prev = w;
        w = w->next;
        if (!prev->closed) {
            world->Remove(prev->window);
        }
        delete prev->window;
        delete prev;
    }
}

void Sted::Open (const char* filename) {
    StedWindowInfo* w = Find(filename);
    if (w == nil) {
        w = new StedWindowInfo();
        w->window = new StedWindow(this, filename);
        w->closed = false;
        w->next = windows;
        if (windows == nil) {
            w->x = 0;
            w->y = 0;
            world->InsertApplication(w->window);
            w->window->GetRelative(w->x, w->y);
        } else {
            w->x = windows->x - 15;
            w->y = windows->y - 20;
            world->InsertApplication(w->window, w->x, w->y);
        }
        windows = w;
    } else {
        if (w->closed) {
            w->closed = false;
            world->InsertApplication(w->window, w->x, w->y);
        } else {
            w->window->DeIconify();
            world->Raise(w->window);
        }
    }
}

void Sted::Close (const char* filename) {
    StedWindowInfo* w = Find(filename);
    if (w != nil) {
        w->closed = true;
    }
}

StedWindowInfo* Sted::Find (const char* filename) {
    StedWindowInfo* w;
    for (w = windows; w != nil; w = w->next) {
        if (strcmp(filename, w->window->Filename()) == 0) {
            return w;
        }
    }
    return nil;
}

void Sted::Complain () {
    world->RingBell(1);
}

void Sted::Quit () {
    done = true;
}

void Sted::Run () {
    Event e;
    boolean alive;
    do {
        world->Read(e);
        e.target->Handle(e);
        alive = false;
        for (StedWindowInfo* w = windows; w != nil; w = w->next) {
            if (w->closed) {
                world->Remove(w->window);
            } else {
                alive = true;
            }
        }
    } while (alive && !done && e.target != nil);
}

StedWindow::StedWindow (Sted* s, const char* name) {
    sted = s;
    input = new Sensor();
    input->Catch(KeyEvent);
    input->Catch(DownEvent);
    state = new ButtonState(false);
    state->Attach(this);
    command = new StringEditor(state, "                    ");
    buffer = nil;
    size = 0;
    text = nil;
    editor = new TextEditor(24, 80, 8, Reversed);
    Insert(
        new VBox(
            new HBox(
                new HGlue(5, 0, 0),
                new VBox(
                    new VGlue(3, 0, 0),
                    editor,
                    new VGlue(3, 0, 0)
                ),
                new HGlue(5, 0, 0),
                new VBorder,
                new VBox(
                    new UpMover(editor, 1),
                    new HBorder(),
                    new VScroller(editor),
                    new HBorder(),
                    new DownMover(editor, 1)
                )
            ),
            new HBorder,
            new VGlue(2, 0, 0),
            new HBox(
                new HGlue(5, 0, 0),
                command,
                new HGlue(5, 0, 0)
            ),
            new VGlue(2, 0, 0)
        )
    );
    prefix1 = false;
    prefix2 = false;
    Edit(name);
}

StedWindow::~StedWindow () {
    sted->Close(filename);
    delete text;
    delete buffer;
    state->Detach(this);
    Unref(state);
}

const char* StedWindow::Filename () {
    return filename;
}

void StedWindow::Edit (const char* name) {
    delete buffer;
    delete text;
    sprintf(filename, "%s", name);
    FILE* f = fopen(filename, "r");
    if (f != nil) {
        struct stat filestats;
        stat(filename, &filestats);
        size = max(round(filestats.st_size * 1.2), MINTEXTSIZE);
        buffer = new char[size];
        char* b = buffer;
        int remaining = size;
        while (remaining > 1 && fgets(b, remaining, f) != nil) {
            int l = strlen(b);
            remaining -= l;
            b += l;
        }
        fclose(f);
        text = new TextBuffer(buffer, b-buffer, size);
        command->Message("");
    } else {
        size = MINTEXTSIZE;
        buffer = new char[size];
        text = new TextBuffer(buffer, 0, size);
        command->Message("new file");
    }
    editor->Edit(text);
    modified = false;
    SetName(filename);
    char* s = strrchr(filename, '/');
    if (s != nil) {
        SetIconName(s+1);
    } else {
        SetIconName(filename);
    }
}

void StedWindow::Update () {
    int value;
    state->GetValue(value);
    if (value == '\r') {
        Do(command->Text());
    }
    state->SetValue('\0');
}

void StedWindow::Do (const char* s) {
    static char operation[BUFFERSIZE];
    static char parameter[BUFFERSIZE];
    static char buffer[BUFFERSIZE];
    strcpy(buffer, s);
    sscanf(buffer, "%s %s", operation, parameter);
    if (strcmp(operation, "backward-page") == 0) {
        editor->BackwardPage(atoi(parameter));
    } else if (strcmp(operation, "forward-page") == 0) {
        editor->ForwardPage(atoi(parameter));
    } else if (strcmp(operation, "backward-character") == 0) {
        editor->BackwardCharacter(atoi(parameter));
    } else if (strcmp(operation, "forward-character") == 0) {
        editor->ForwardCharacter(atoi(parameter));
    } else if (strcmp(operation, "backward-line") == 0) {
        editor->BackwardLine(atoi(parameter));
    } else if (strcmp(operation, "forward-line") == 0) {
        editor->ForwardLine(atoi(parameter));
    } else if (strcmp(operation, "beginning-of-text") == 0) {
        editor->BeginningOfText();
    } else if (strcmp(operation, "end-of-text") == 0) {
        editor->EndOfText();
    } else if (strcmp(operation, "beginning-of-line") == 0) {
        editor->BeginningOfLine();
    } else if (strcmp(operation, "end-of-line") == 0) {
        editor->EndOfLine();
    } else if (strcmp(operation, "delete-character") == 0) {
        if (editor->Dot() != editor->Mark()) {
            editor->DeleteSelection();
        } else {
            editor->DeleteText(atoi(parameter));
        }
        modified = true;
    } else if (strcmp(operation, "insert-character") == 0) {
        editor->DeleteSelection();
        editor->InsertText(&lastchar, 1);
        modified = true;
    } else if (strcmp(operation, "quit") == 0) {
        sted->Quit();
    } else if (strcmp(operation, "close") == 0) {
        sted->Close(filename);
    } else if (strcmp(operation, "visit") == 0) {
        sted->Open(parameter);
    } else if (strcmp(operation, "file") == 0) {
        Edit(parameter);
    } else if (strcmp(operation, "search") == 0) {
        Regexp re(parameter);
        if (
            text->ForwardSearch(&re, editor->Dot()) >= 0
            || text->ForwardSearch(&re, text->BeginningOfText()) >= 0
        ) {
            editor->Select(re.EndOfMatch(), re.BeginningOfMatch());
        } else {
            sted->Complain();
        }
    } else if (strcmp(operation, "goto") == 0) {
        editor->Select(text->LineIndex(atoi(parameter)));
    } else {
        sted->Complain();
    }
    editor->ScrollToSelection();
    command->Message("");
}

void StedWindow::Ask (const char* message, int begin, int end) {
    command->Edit(message, begin, end);
}

void StedWindow::Handle (Event& e) {
    if (e.eventType == KeyEvent && e.len > 0) {
        HandleChar(e.keystring[0]);

    } else if (e.eventType == DownEvent) {
        switch (e.button) {
            case LEFTMOUSE:     LeftMouse(e); break;
            case MIDDLEMOUSE:   editor->GrabScroll(e); break;
            case RIGHTMOUSE:    editor->RateScroll(e); break;
        }
    }
}

void StedWindow::HandleChar (char c) {
    if (prefix1) {
        prefix1 = false;
        switch (c) {
          case 'v':     Do("backward-page 1"); break;
          case 'x':     Ask("", 0, 0); break;
          case '<':     Do("beginning-of-text"); break;
          case '>':     Do("end-of-text"); break;
          case '\r':    Do(""); break;
          case '=':     Ask("goto ", 5, 5); break;
          default:      Do("complain"); break;
        }
    } else if (prefix2) {
        prefix2 = false;
        switch (c) {
          case '\006':  Ask("file ", 5, 5); break;
          case '\026':  Ask("visit ", 6, 6); break;
          case '\003':  Ask("quit", 0, 4); break;
          case 'k':     Ask("close", 0, 5); break;
          default:      Do("complain"); break;
        }
    } else {
        switch (c) {
          case '\033':  prefix1 = true; break;
          case '\030':  prefix2 = true; break;
          case '\026':  Do("forward-page 1"); break;
          case '\001':  Do("beginning-of-line"); break;
          case '\005':  Do("end-of-line"); break;
          case '\006':  Do("forward-character 1"); break;
          case '\002':  Do("backward-character 1"); break;
          case '\016':  Do("forward-line 1"); break;
          case '\020':  Do("backward-line 1"); break;
          case '\023':  Ask("search ", 7, 7); break;
          case '\004':  Do("delete-character 1"); break;
          case '\010':  Do("delete-character -1"); break;
          case '\177':  Do("delete-character -1"); break;
          case '\011':  lastchar = '\t'; Do("insert-character"); break;
          case '\015':  lastchar = '\n'; Do("insert-character"); break;
          case '\007':  Do("complain"); break;
            default:
            if (!iscntrl(c)) {
                lastchar = c;
                Do("insert-character");
            } else {
                Do("complain");
            }
            break;
        }
    }
}

void StedWindow::LeftMouse (Event& e) {
    GetRelative(e.x, e.y, editor);
    editor->Select(editor->Locate(e.x, e.y));
    do {
        editor->ScrollToView(e.x, e.y);
        editor->SelectMore(editor->Locate(e.x, e.y));
        Poll(e);
        GetRelative(e.x, e.y, editor);
    } while (e.leftmouse);
}

int main (int argc, char* argv[]) {
    World* world = new World("sted", argc, argv);
    Sted* sted = new Sted(world);
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            sted->Open(argv[i]);
        }
    } else {
        sted->Open("/interviews/lib/all/sted.help");
    }

    sted->Run();

    delete sted;
    delete world;
    return 0;
}

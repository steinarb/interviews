#include "dialogs.h"

#include <IV-look/2.6/InterViews/button.h> 
#include <IV-look/2.6/InterViews/filebrowser.h> 
#include <IV-look/2.6/InterViews/frame.h> 
#include <IV-look/2.6/InterViews/streditor.h> 
#include <InterViews/2.6/InterViews/box.h> 
#include <InterViews/2.6/InterViews/glue.h> 
#include <InterViews/2.6/InterViews/message.h> 
#include <InterViews/sensor.h> 
#include <InterViews/window.h>
#include <InterViews/world.h>

#include <InterViews/2.6/_enter.h>

#include <string.h>

Chooser::Chooser (
    const char* name, const char* prompt
) : FileChooser(name, prompt, ".", 10, 20, "  OK  ") { }

const char* Chooser::post (Window* window, const char* ext) {
    if (ext != nil) {
        static char filter[100];
        strcpy(filter, ".*\\.");
        strcat(filter, ext);
        browser()->SetTextFilter(filter);
        browser()->Update();
    }
    World* world = World::current();
    IntCoord x, y;
    if (window == nil) {
        x = IntCoord(world->width() / 2);
        y = IntCoord(world->height() / 2);
    } else {
        x = IntCoord(window->left() + window->width() / 2);
        y = IntCoord(window->bottom() + window->height() / 2);
    }
    world->InsertTransient(this, this, x, y, Center);

    Accept();

    world->Remove(this);

    if (Status() == '\007') {
        return nil;
    }
    return Choice();
}

Asker::Asker (
    const char* name, const char* prompt
) : Dialog(name, askerbs = new ButtonState(0), nil) {
    input = new Sensor;
    input->Catch(KeyEvent);
    askereditor = new StringEditor(askerbs, "");
    Insert(
        new MarginFrame(
            new VBox(
                new HBox(new Message(prompt), new HGlue()),
                new VGlue(11, 11, vfil),
                new Frame(new MarginFrame(askereditor, 2, 0, 0, 2, 0, 0), 1),
                new VGlue(11, 11, vfil),
                new HBox(
                    new HGlue(100, 100, hfil),
                    new PushButton("Cancel", askerbs, 2),
                    new HGlue(9, 9, 0),
                    new PushButton("  OK  ", askerbs, 1)
                )
            ), 11, 0, 0, 11, 0, 0
        )
    );
}

const char* Asker::post (Window* window, const char* initial) {
    World* world = World::current();
    IntCoord x, y;
    if (window == nil) {
        x = IntCoord(world->width() / 2);
        y = IntCoord(world->height() / 2);
    } else {
        x = IntCoord(window->left() + window->width() / 2);
        y = IntCoord(window->bottom() + window->height() / 2);
    }
    world->InsertTransient(this, this, x, y, Center);

    askerbs->SetValue(0);
    if (initial != nil) {
        askereditor->Message(initial);
    }
    askereditor->Edit();
    if (Status() == 0) {
        Accept();
    }
    world->Remove(this);

    if (Status() == 2) {
        return nil;
    }
    return askereditor->Text();
}

Confirmer::Confirmer (
    const char* name, const char* prompt
) : Dialog(name, confirmerbs = new ButtonState(0), nil) {
    Insert(
        new MarginFrame(
            new VBox(
                new HBox(new Message(prompt), new HGlue()),
                new VGlue(16, 8, 20),
                new HBox(
                    new HGlue(30, 0, hfil),
                    new PushButton("Cancel", confirmerbs, 3),
                    new HGlue(8, 0, 0),
                    new PushButton("  No  ", confirmerbs, 2),
                    new HGlue(8, 0, 0),
                    new PushButton(" Yes ", confirmerbs, 1)
                )
            ), 28, 20, 100, 10, 5, 50
        )
    );
}

int Confirmer::post (Window* window) {
    World* world = World::current();
    IntCoord x, y;
    if (window == nil) {
        x = IntCoord(world->width() / 2);
        y = IntCoord(world->height() / 2);
    } else {
        x = IntCoord(window->left() + window->width() / 2);
        y = IntCoord(window->bottom() + window->height() / 2);
    }
    world->InsertTransient(this, this, x, y, Center);
    confirmerbs->SetValue(0);
    Accept();
    world->Remove(this);
    return Status();
}

Reporter::Reporter (
    const char* name, const char* prompt
) : Dialog(name, reporterbs = new ButtonState(0), nil) {
    Insert(
        new MarginFrame(
            new VBox(
                new HBox(new Message(prompt), new HGlue()),
                new VGlue(16, 8, 20),
                new HBox(
                    new HGlue(36, 0, 1000000),
                    new PushButton("  OK  ", reporterbs, 1)
                )
            ), 28, 20, 100, 10, 5, 50
        )
    );
}

void Reporter::post (Window* window) {
    World* world = World::current();
    IntCoord x, y;
    if (window == nil) {
        x = IntCoord(world->width() / 2);
        y = IntCoord(world->height() / 2);
    } else {
        x = IntCoord(window->left() + window->width() / 2);
        y = IntCoord(window->bottom() + window->height() / 2);
    }
    world->InsertTransient(this, this, x, y, Center);
    reporterbs->SetValue(0);
    Accept();
    world->Remove(this);
}

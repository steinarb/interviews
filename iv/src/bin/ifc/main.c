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
 * File chooser main program.
 */

#include <InterViews/filechooser.h>
#include <InterViews/window.h>
#include <InterViews/world.h>
#include <stdlib.h>
#include <stream.h>
#include <string.h>

static PropertyData properties[] = {
    { "*caption", "Please select a file:" },
    { "*subcaption", "" },
    { "*rows", "10" },
    { "*cols", "24" },
    { "*accept", " Open " },
    { "*static", "false" },
    { nil }
};

static OptionDesc options[] = {
    { "-caption", "*caption", OptionValueNext },
    { "-subcaption", "*subcaption", OptionValueNext },
    { "-rows", "*rows", OptionValueNext },
    { "-cols", "*cols", OptionValueNext },
    { "-accept", "*accept", OptionValueNext },
    { "-static", "*static", OptionValueImplicit, "true" },
    { nil }
};

int main (int argc, char** argv) {
    World* world = new World("Ifc", argc, argv, options, properties);
    const char* dir = (argc == 2) ? argv[1] : "~";
    const char* _static = world->GetAttribute("static");
    boolean transient = _static == nil || strcmp(_static, "true") != 0;
    int status = 0;

    FileChooser* chooser = new FileChooser(
        world->GetAttribute("caption"),
        world->GetAttribute("subcaption"),
        dir,
        atoi(world->GetAttribute("rows")),
        atoi(world->GetAttribute("cols")),
        world->GetAttribute("accept")
    );

    ApplicationWindow* w = new ApplicationWindow(chooser);
    w->name("InterViews file chooser");
    w->icon_name("ifc");
    w->map();

    chooser->SelectFile();
    
    do {
        status = chooser->Accept() ? 0 : 1;
        if (status == 0) {
            cout << chooser->Choice() << "\n";
            cout.flush();
        }
    } while (!transient && status == 0);

    return status;
}

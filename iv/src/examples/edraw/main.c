#include "glypheditor.h"
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/window.h>
#include <stdlib.h>

static PropertyData props[] = {
    { "*GlyphEditor*name", "Elastic Drawing editor" },
    { "*GlyphEditor*iconName", "EDraw" },
    { nil }
};

static OptionDesc options[] = {
    { nil }
};

int main(int argc, char** argv) {
    Session* session = new Session("Glypher", argc, argv, options, props);
    GlyphEditor* ged = new GlyphEditor;
    ApplicationWindow* w = new ApplicationWindow(ged);
    Style* s = new Style(session->style());
    s->attribute("name", "Elastic Drawing Editor");
    s->attribute("iconName", "EDraw");
    w->style(s);
    return session->run_window(w);
}

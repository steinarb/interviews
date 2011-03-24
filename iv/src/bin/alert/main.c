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
 *  alert - displays a message in a dialog box
 */

#include <IV-look/button.h>
#include <IV-look/kit.h>
#include <InterViews/background.h>
#include <InterViews/box.h>
#include <InterViews/center.h>
#include <InterViews/display.h>
#include <InterViews/glue.h>
#include <InterViews/label.h>
#include <InterViews/margin.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/window.h>
#include <OS/string.h>
#include <stdio.h>

static PropertyData props[] = {
    { "*quitbutton", "OK, OK ..." },
    { "*font", "9x15" },
    { "*transient", "on" },
    { nil }
};

static OptionDesc options[] = {
    { "font=", "*font", OptionValueAfter },
    { "button=", "*quitbutton", OptionValueAfter },
    { "-top", "*transient", OptionValueImplicit, "off" },
    { nil }
};

static Glyph* make_message(Glyph*, Kit*, Style*);

int main(int argc, char** argv) {
    Session* session = new Session("Alert", argc, argv, options, props);
    Style* style = session->style();
    Kit* kit = Kit::instance();
    String button_label;
    style->find_attribute("quitbutton", button_label);

    Glyph* vspace = new VGlue(18.0);
    Glyph* hspace = new HGlue(36.0);
    Glyph* dialog = new Background(
	new TBBox(
	    new VCenter(vspace, 1.0),
	    new LRBox(
		hspace,
		new TBBox(
		    new VCenter(make_message(new TBBox, kit, style), 1.0),
		    vspace,
		    new LMargin(
			kit->simple_push_button(
			    button_label, style, kit->quit()
			),
			0.0, fil, 0.0
		    )
		),
		hspace
	    ),
	    vspace
	),
	style->flat()
    );

    Window* w;
    if (style->value_is_on("transient")) {
	TransientWindow* t = new TransientWindow(
	    kit->outset_frame(new Margin(dialog, 2.0), style)
	);
	t->transient_for(t);
	w = t;
    } else {
	w = new ApplicationWindow(dialog);
    }
    Display* d = session->default_display();
    w->place(d->width() / 2, d->height() / 2);
    w->align(0.5, 0.5);
    d->ring_bell(0);
    session->run_window(w);
    delete w;
    delete session;
    return 0;
}

static Glyph* make_message(Glyph* container, Kit* kit, Style* style) {
    char buffer[1024];
    /* guarantee null-termination */
    buffer[sizeof(buffer) - 1] = '\0';

    while (fgets(buffer, sizeof(buffer) - 1, stdin) != nil) {
	String s(buffer);
	int n = s.length();
	if (s[n - 1] == '\n') {
	    s.set_to_left(n - 1);
	}
	container->append(new RMargin(kit->label(s, style), 0.0, fil, 0.0));
    }
    return container;
}

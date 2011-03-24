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

#include <InterViews/button.h>
#include <InterViews/world.h>
#include <stdio.h>

#include "fontbrowser.h"

static PropertyData properties[] = {
    { "ifb*sample", "The quick brown fox..." },
    { "ifb*choice*font",
        "*-*-times-medium-r-normal-*-*-120-*-*-*-*-iso8859-1"
    },
    { "ifb*label*font",
        "*-*-times-bold-r-normal-*-*-120-*-*-*-*-iso8859-1"
    },
    { "ifb*PushButton*font",
        "*-*-times-medium-i-normal-*-*-140-*-*-*-*-iso8859-1"
    },
    { nil }
};

static OptionDesc options[] = {
    { "-sample", "ifb*sample", OptionValueNext },
    { nil }
};

int main (int argc, char* argv[]) {
    World* world = new World("ifb", properties, options, argc, argv);
    ButtonState* quit = new ButtonState(false);
    FontBrowser* browser = new FontBrowser(
        quit, world->GetAttribute("sample")
    );

    browser->SetName("InterViews font browser");
    browser->SetIconName("ifb");

    world->InsertApplication(browser);
    browser->Accept();
    world->Remove(browser);

    puts(browser->Fontname());

    delete browser;
    Unref(quit);
    delete world;
    return 0;
}

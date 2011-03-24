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
 * InterViews squares demo program.
 */

#include "sframe.h"
#include "squares.h"
#include "view.h"
#include <InterViews/sensor.h>
#include <InterViews/world.h>

static Squares* MakeInitialSquares();
static SquaresView* MakeInitialView();

static PropertyData props[] = {
    { "*adjustersize", "s" },
    { "*font", "*helvetica-medium-r-normal*14*" },
    { nil }
};

static OptionDesc options[] = {
    { "-panner", "*panner", OptionValueImplicit, "0" },
    { "-above", "*above", OptionValueImplicit, "true" },
    { "-left", "*left", OptionValueImplicit, "true" },
    { nil }
};

int main (int argc, char* argv[]) {
    World* world = new World("Squares", props, options, argc, argv);
    world->InsertApplication(new SquaresFrame(MakeInitialView()));
    world->Run();
    return 0;
}

static Squares* MakeInitialSquares () {
    const initnumsquares = 3;
    Squares* s;
    int i;

    s = new Squares;
    for (i = 0; i < initnumsquares; i++) {
	s->Add();
    }
    return s;
}

static SquaresView* MakeInitialView () {
    return new SquaresView(MakeInitialSquares());
}

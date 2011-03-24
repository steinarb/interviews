/*
 * Copyright (c) 1991 Stanford University
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
 * User interface builder main program.
 * $Header: main.c,v 1.3 91/09/27 15:29:01 vlis Exp $
 */

#include "ibcreator.h"
#include "ibed.h"

#include <Unidraw/catalog.h>
#include <Unidraw/unidraw.h>
#include <InterViews/world.h>
#include <stream.h>

/*****************************************************************************/

static PropertyData properties[] = {
    { "*domain",	"user interface" },
    { "*history",	"20" },
    { "*initialborder",  "2" },
    { "*initialfgcolor","1" },
    { "*initialbgcolor","10" },
    { "*initialfont",   "4" },
    { "*initialpattern","1" },
    { "*pattern1",      "1.0" },
    { "*font1", "-*-courier-medium-r-normal-*-8-*-*-*-*-*-*-* Courier 8" },
    { "*font2", "-*-courier-medium-r-normal-*-10-*-*-*-*-*-*-* Courier 10" },
    { "*font3", "-*-courier-bold-r-normal-*-12-*-*-*-*-*-*-* Courier-Bold 12" },
    { "*font4", "-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-* Helvetica 12" },
    { "*font5", "-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-* Helvetica 14" },
    { "*font6", "-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-* Helvetica-Bold 14" },
    { "*font7", "-*-helvetica-medium-o-normal-*-14-*-*-*-*-*-*-* Helvetica-Oblique 14" },
    { "*font8",	"-*-times-medium-r-normal-*-12-*-*-*-*-*-*-*  Times-Roman 12" },
    { "*font9", "-*-times-medium-r-normal-*-14-*-*-*-*-*-*-* Times-Roman 14" },
    { "*font10", "-*-times-bold-r-normal-*-14-*-*-*-*-*-*-*  Times-Bold 14" },
    { "*font11", "-*-times-medium-i-normal-*-14-*-*-*-*-*-*-* Times-Italic 14" },
    { "*border1",        "ffff 1" },
    { "*border2",        "ffff 2" },
    { "*border3",        "ffff 3" },
    { "*border4",        "ffff 4" },
    { "*border5",        "ffff 5" },
    { "*fgcolor1",      "Black 0 0 0" },
    { "*fgcolor2",      "Brown 42240 10752 10752" },
    { "*fgcolor3",      "Red 65535 0 0" },
    { "*fgcolor4",      "Orange 65535 42405 0" },
    { "*fgcolor5",      "Yellow 65535 65535 0" },
    { "*fgcolor6",      "Green 0 65535 0" },
    { "*fgcolor7",      "Blue 0 0 65535" },
    { "*fgcolor8",      "Indigo 48896 0 65280" },
    { "*fgcolor9",      "Violet 20224 12032 20224" },
    { "*fgcolor10",     "White 65535 65535 65535" },
    { "*fgcolor11",     "LtGray 50000 50000 50000" },
    { "*fgcolor12",     "DkGray 33000 33000 33000" },
    { "*bgcolor1",      "Black 0 0 0" },
    { "*bgcolor2",      "Brown 42240 10752 10752" },
    { "*bgcolor3",      "Red 65535 0 0" },
    { "*bgcolor4",      "Orange 65535 42405 0" },
    { "*bgcolor5",      "Yellow 65535 65535 0" },
    { "*bgcolor6",      "Green 0 65535 0" },
    { "*bgcolor7",      "Blue 0 0 65535" },
    { "*bgcolor8",      "Indigo 48896 0 65280" },
    { "*bgcolor9",      "Violet 20224 12032 20224" },
    { "*bgcolor10",     "White 65535 65535 65535" },
    { "*bgcolor11",     "LtGray 50000 50000 50000" },
    { "*bgcolor12",     "DkGray 33000 33000 33000" },
    { nil }
};

static OptionDesc options[] = {
    { nil }
};

/*****************************************************************************/

int main (int argc, char** argv) {
    IBCreator creator;
    int exit_status = 0;
    Unidraw *unidraw = new Unidraw(
        new Catalog("ibuild", &creator), argc, argv, options, properties
    );

    if (argc > 2) {
	cerr << "Usage: ibuild [file]" << "\n";
	exit_status = 1;

    } else {
	const char* initial_file = (argc == 2) ? argv[1] : nil;
	IBEditor* ed = new IBEditor(initial_file);

	unidraw->Open(ed);
	unidraw->Run();
    }

    delete unidraw;
    return exit_status;
}

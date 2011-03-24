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

// $Header: keystrokes.h,v 1.12 89/10/09 14:48:30 linton Exp $
// defines all the key strokes that idraw handles.

// You can select a PanelItem by typing one of these characters.

static const char SELECTCHAR		= 's';
static const char MOVECHAR		= 'm';
static const char SCALECHAR		= 'j';
static const char STRETCHCHAR		= ';';
static const char ROTATECHAR		= 'k';
static const char RESHAPECHAR		= 'q';
static const char MAGNIFYCHAR		= 'z';
static const char TEXTCHAR		= 't';
static const char LINECHAR		= 'l';
static const char MULTILINECHAR		= 'w';
static const char BSPLINECHAR		= 'h';
static const char ELLIPSECHAR		= 'o';
static const char RECTCHAR		= 'r';
static const char POLYGONCHAR		= 'p';
static const char CLOSEDBSPLINECHAR	= 'y';

// You can execute a PullDownMenuCommand by typing one of these
// characters.

static const char NEWCHAR		= '\016'; // ^N
static const char REVERTCHAR		= '\022'; // ^R
static const char OPENCHAR		= '\017'; // ^O
static const char SAVECHAR		= '\023'; // ^S
static const char SAVEASCHAR		= '\001'; // ^A
static const char PRINTCHAR		= '\020'; // ^P
static const char QUITCHAR		= '\021'; // ^Q

static const char UNDOCHAR		= 'U';
static const char REDOCHAR		= 'R';
static const char CUTCHAR		= 'x';
static const char COPYCHAR		= 'c';
static const char PASTECHAR		= 'v';
static const char DUPLICATECHAR		= 'd';
static const char DELETECHAR		= '\004'; // ^D
static const char SELECTALLCHAR		= 'a';
static const char FLIPHORIZONTALCHAR	= '_';
static const char FLIPVERTICALCHAR	= '|';
static const char _90CLOCKWISECHAR	= ']';
static const char _90COUNTERCWCHAR	= '[';
static const char PRECISEMOVECHAR	= 'M';
static const char PRECISESCALECHAR	= 'J';
static const char PRECISEROTATECHAR	= 'K';

static const char GROUPCHAR		= 'g';
static const char UNGROUPCHAR		= 'u';
static const char BRINGTOFRONTCHAR	= 'f';
static const char SENDTOBACKCHAR	= 'b';
static const char NUMBEROFGRAPHICSCHAR	= '#';

static const char ALIGNLEFTSIDESCHAR	= '1';
static const char ALIGNRIGHTSIDESCHAR	= '2';
static const char ALIGNBOTTOMSCHAR	= '3';
static const char ALIGNTOPSCHAR		= '4';
static const char ALIGNVERTCENTERSCHAR	= '5';
static const char ALIGNHORIZCENTERSCHAR	= '6';
static const char ALIGNCENTERSCHAR	= '7';
static const char ALIGNLEFTTORIGHTCHAR	= '8';
static const char ALIGNRIGHTTOLEFTCHAR	= '9';
static const char ALIGNBOTTOMTOTOPCHAR	= '0';
static const char ALIGNTOPTOBOTTOMCHAR	= '-';
static const char ALIGNTOGRIDCHAR	= '.';

static const char REDUCECHAR		= 'i';
static const char ENLARGECHAR		= 'e';
static const char NORMALSIZECHAR	= 'n';
static const char REDUCETOFITCHAR	= '=';
static const char CENTERPAGECHAR	= '/';
static const char REDRAWPAGECHAR	= '\014'; // ^L
static const char GRIDDINGONOFFCHAR	= ',';
static const char GRIDVISIBLEINVISIBLECHAR = '?';
static const char GRIDSPACINGCHAR	= 'S';
static const char ORIENTATIONCHAR	= '+';
static const char SHOWVERSIONCHAR	= '$';

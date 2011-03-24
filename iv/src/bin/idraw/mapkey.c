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

// $Header: mapkey.c,v 1.7 89/10/09 14:49:03 linton Exp $
// implements class MapKey.

#include "mapkey.h"
#include <ctype.h>
#include <stdio.h>

// MapKey clears MapKey's array to all nils.

MapKey::MapKey () {
    for (int i = 0; i <= MAXCHAR; i++) {
	array[i] = nil;
    }
}

// Enter enters an Interactor into a slot in MapKey's array.  If the
// slot doesn't exist or another Interactor already occupies it,
// Enter prints a warning message.

void MapKey::Enter (Interactor* i, char c) {
    if (c >= 0 && c <= MAXCHAR) {
	if (array[c] == nil) {
	    array[c] = i;
	} else {
	    fprintf(stderr, "MapKey: slot %d already occupied!\n", c);
	}
    } else {
	fprintf(stderr, "MapKey: slot %d not in array!\n", c);
    }
}

// LookUp returns the Interactor associated with the given character
// or nil if there's no Interactor or the character's out of bounds.

Interactor* MapKey::LookUp (char c) {
    if (c >= 0 && c <= MAXCHAR) {
	return array[c];
    } else {
	fprintf(stderr, "MapKey: slot %d not in array!\n", c);
	return nil;
    }
}

// ToStr returns a printable string representing the given character.
// The caller must copy the returned string because the next call of
// ToStr will change it.

const char* MapKey::ToStr (char c) {
    static char key[3];
    if (!isprint(c)) {
	c = toascii(c + 0100);
	key[0] = '^';
    } else {
	key[0] = ' ';
    }
    key[1] = c;
    key[2] = 0;
    return key;
}

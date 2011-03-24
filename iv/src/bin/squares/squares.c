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
 * Implementation of squares subject.
 */

#include "squares.h"

#if defined(hpux)
#   include <stdlib.h>
#else
#   include <random.h>
#endif

Squares::Squares () {
    head = nil;
}

Squares::~Squares () {
    register SquareData* p, * next;

    for (p = head; p != nil; p = next) {
	next = p->next;
	delete p;
    }
}

void Squares::Add (float cx, float cy, float size) {
    register SquareData* s = new SquareData;
    s->cx = cx;
    s->cy = cy;
    s->size = size;
    s->next = head;
    head = s;
    Notify();
}

static float Random () {
#ifdef hpux
    const int bigint = 1<<14;
    long r = rand();
#else
    const int bigint = 1<<30;
    long r = random();
#endif
    if (r < 0) {
	r = -r;
    }
    return float(r % bigint) / float(bigint);
}

void Squares::Add () {
    Add(Random()/2 + .25, Random()/2 + .25, Random()/2);
}

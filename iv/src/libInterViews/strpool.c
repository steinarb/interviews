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
 * A string pool is an area for allocating string constants
 * without the overhead (hopefully) of malloc/new.
 */

#include <InterViews/strpool.h>
#include <string.h>

StringPool::StringPool (int poolsize) {
    data = new char[poolsize];
    size = poolsize;
    cur = 0;
    prev = nil;
};

/*
 * Tail-recursive deletion to walk the list back to the head
 * of the pool.
 */

StringPool::~StringPool () {
    delete data;
    delete prev;
}

/*
 * Add a string of a given length to the pool.  If it won't fit,
 * create a copy of the current pool and allocate space for a new one.
 *
 * No null-padding is implied, so if you want that you must include
 * the null in the length.
 */

char* StringPool::Append (const char* str, int len) {
    register int index = cur;	/* cse: this->cur */
    register int newcur = index + len;
    if (newcur > size) {
	StringPool* s = new StringPool;
	*s = *this;
	data = new char[size];
	prev = s;
	index = 0;
	newcur = len;
    }
    char* r = &data[index];
    strncpy(r, str, len);
    cur = newcur;
    return r;
}

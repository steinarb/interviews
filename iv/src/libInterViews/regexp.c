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
 * Regexp - regular expression searching
 */

#include <InterViews/defs.h>
#include <InterViews/regexp.h>
#include <regexpr.h>
#include <string.h>

/*
 * This version is based on the BSD regular expression functions.
 * A better approach would be to reimplement the searching in C++
 */

Regexp::Regexp (const char* pat) {
    int length = strlen(pat);
    pattern = new char[length+1];
    strncpy(pattern, pat, length);
    pattern[length] = '\0';
}

Regexp::Regexp (const char* pat, int length) {
    pattern = new char[length+1];
    strncpy(pattern, pat, length);
    pattern[length] = '\0';
}

Regexp::~Regexp () {
    delete pattern;
}

int Regexp::Search (const char* text, int length, int index, int range) {
    beginning = -1;
    end = -1;
    char save = *(text+length);
    *(char*)(text+length) = '\0';
    if (re_comp(pattern) == nil) {
        const char* t = text;
        const char* b = t + index;
        if (pattern[0] == '^' && (b != t || b[-1] == '\n')) {
            b = strchr(b, '\n') + 1;
        }
        const char* e = strchr(b, '\n');
        char c;
        int result;
        while (b - t < index + range && e != nil && e - t < length) {
            c  = *e;
            *(char*)e = '\0';
            result = re_exec(b);
            *(char*)e = c;
            if (result != 0) {
                break;
            } else {
                b = e + 1;
                e = strchr(b, '\n');
            }
        }
        if (result == 1) {
            if (pattern[strlen(pattern)-1] != '$') {
                result = 1;
                while (result == 1) {
                    --e;
                    c = *e;
                    *(char*)e = '\0';
                    result = re_exec(b);
                    *(char*)e = c;
                }
                ++e;
            }
            if (pattern[0] != '^') {
                c = *e;
                *(char*)e = '\0';
                b = e;
                result = 0;
                while (result != 1) {
                    --b;
                    result = re_exec(b);
                }
                *(char*)e = c;
            }
            beginning = b - t;
            end = e - t;
        }
    }
    *(char*)(text+length) = save;
    return beginning;
}

int Regexp::Match (const char* text, int length, int index) {
    beginning = -1;
    end = -1;
    char save = *(text+length);
    *(char*)(text+length) = '\0';
    if (re_comp(pattern) == nil) {
        const char* t = text + index;
        const char* e = strchr(t, '\n');
        if (e != nil) {
            *(char*)e = '\0';
            int result = re_exec(t);
            *(char*)e = '\n';
            if (result == 1) {
                beginning = t - text;
                end = e - text;
            }
        }
    }
    *(char*)(text+length) = save;
    if (beginning != -1) {
        return beginning - end;
    } else {
        return -1;
    }
}

int Regexp::BeginningOfMatch (int) {
    return beginning;
}

int Regexp::EndOfMatch (int) {
    return end;
}

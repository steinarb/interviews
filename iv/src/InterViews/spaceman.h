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
 * Object space manager.
 */

#ifndef spaceman_h
#define spaceman_h

#include <InterViews/defs.h>
#include <InterViews/deputy.h>

class Connection;

class SpaceManager : public Deputy {
public:
    SpaceManager();
    ~SpaceManager();

    void UsePath(const char*);
    void Register(const char*, Connection* local, Connection* remote);
    void UnRegister(const char*);
    Connection* Find(const char*, boolean wait = false);
private:
    char filename[128];
    char hostname[128];
};

extern const char* spaceman_dir;
extern const char* spaceman_mgr;
extern const char* spaceman_name;

static const int spaceman_UsePath = 1;
static const int spaceman_Register = 2;
static const int spaceman_UnRegister = 3;
static const int spaceman_Find = 4;
static const int spaceman_WaitFor = 5;

#endif

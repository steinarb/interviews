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
 * Each host has an object space manager that is a switchboard
 * for naming spaces.
 */

#include <InterViews/spaceman.h>
#include <InterViews/chief.h>
#include <InterViews/connection.h>
#include <os/fs.h>
#include <os/host.h>
#include <os/proc.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* spaceman_dir = "/tmp/.allegro";
const char* spaceman_mgr = "/tmp/.allegro/spaceman";
const char* spaceman_name = "/tmp/.allegro/p%5d";

static const int spaceman_namelen = 21;

inline void fatal (const char* s) {
    perror(s);
    exit(1);
}

SpaceManager::SpaceManager () {
    Connection* c;
    char cwd[1024];
    int* tag;
    void* tmp;

    c = new Connection;
    if (!c->OpenLocalService(spaceman_mgr)) {
	fatal("can't connect to space manager");
    }
    chief = new ChiefDeputy(c);
    hostname[0] = '\0';
    chief->Alloc(tmp, chief->Tag(), 0, sizeof(int));
    tag = (int*)tmp;
    *tag = Tag();
    UsePath(getwd(cwd));
}

SpaceManager::~SpaceManager () {
    delete chief;
}

void SpaceManager::UsePath (const char* s) {
    chief->StringMsg(Tag(), spaceman_UsePath, s);
}

void SpaceManager::Register (const char* name, Connection* a, Connection*) {
    int* msg;
    void* tmp;

    int pid = getpid();
    sprintf(filename, spaceman_name, pid);
    a->CreateLocalService(filename);
    int n = strlen(name);
    chief->Alloc(tmp, Tag(), spaceman_Register, sizeof(int) + n + 1);
    msg = (int*)tmp;
    msg[0] = pid;
    chief->PackString(name, n, &msg[1]);
    chief->Sync();
}

void SpaceManager::UnRegister (const char* name) {
    chief->StringMsg(Tag(), spaceman_UnRegister, name);
    chief->Sync();
}

Connection* SpaceManager::Find (const char* name, boolean wait) {
    int pid;
    Connection* c;
    char* local;

    chief->StringMsg(Tag(), wait ? spaceman_WaitFor : spaceman_Find, name);
    chief->GetReply(&pid, sizeof(pid));
    if (pid == 0) {
	c = nil;
    } else {
	c = new Connection;
	local = new char[spaceman_namelen];
	sprintf(local, spaceman_name, pid);
	if (!c->OpenLocalService(local)) {
	    fatal("can't open local service");
	}
    }
    return c;
}

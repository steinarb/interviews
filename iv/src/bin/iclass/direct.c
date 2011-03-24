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

/*
 * Directory object implementation.
 */

#include <bstring.h>
#include <pwd.h>
#include <os/auth.h>
#include <os/fs.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "direct.h"
#include "globals.h"

/*****************************************************************************/

Directory::Directory (const char* name) {
    const int defaultSize = 256;

    strbufsize = defaultSize;
    strbuf = new char*[strbufsize];
    strcount = 0;
    LoadDirectory(name);
}

Directory::~Directory () {
    Clear();
}

const char* Directory::RealPath (const char* path) {
    const char* realpath;

    if (*path == '\0') {
        realpath = "./";
    } else {
        realpath = InterpTilde(InterpSlashSlash(path));
    }
    return realpath;
}

boolean Directory::LoadDirectory (const char* name) {
    char buf[MAXPATHLEN+2];
    const char* path = buf;

    strcpy(buf, ValidDirectories(RealPath(name)));
    return Reset(buf);
}

int Directory::Index (const char* name) {
    for (int i = 0; i < strcount; ++i) {
        if (strcmp(strbuf[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

boolean Directory::Reset (const char* path) {
    DIR* dir = opendir(path);
    boolean successful = dir != NULL;
    struct dirent* d;

    if (successful) {
        Clear();

        for (d = readdir(dir); d != NULL; d = readdir(dir)) {
            Insert(d->d_name, Position(d->d_name));
        }
        closedir(dir);
    }
    return successful; 
}

boolean Directory::IsADirectory (const char* path) {
    struct stat filestats;
    stat(path, &filestats);
    return filestats.st_mode & S_IFDIR;
}

const char* Directory::Home (const char* name) {
    struct passwd* pw =
        (name == nil) ? getpwuid(getuid()) : getpwnam(name);
    return (pw == nil) ? nil : pw->pw_dir;
}

inline boolean DotSlash (const char* path) {
    return 
        path[0] != '\0' && path[0] == '.' &&
        (path[1] == '/' || path[1] == '\0');
}

inline boolean DotDotSlash (const char* path) {
    return 
        path[0] != '\0' && path[1] != '\0' &&
        path[0] == '.' && path[1] == '.' &&
        (path[2] == '/' || path[2] == '\0');
}

const char* Directory::Normalize (const char* path) {
    static char newpath[MAXPATHLEN+1];
    const char* buf;

    buf = InterpSlashSlash(path);
    buf = ElimDot(buf);
    buf = ElimDotDot(buf);
    buf = InterpTilde(buf);

    if (*buf == '\0') {
        strcpy(newpath, "./");

    } else if (!DotSlash(buf) && !DotDotSlash(buf) && *buf != '/') {
        strcpy(newpath, "./");
        strcat(newpath, buf);

    } else {
        strcpy(newpath, buf);
    }

    if (IsADirectory(newpath) && newpath[strlen(newpath)-1] != '/') {
        strcat(newpath, "/");
    }
    return newpath;
}

const char* Directory::ValidDirectories (const char* path) {
    static char buf[MAXPATHLEN+1];
    strcpy(buf, path);
    int i = strlen(path);

    while (!IsADirectory(RealPath(buf)) && i >= 0) {
        for (--i; buf[i] != '/' && i >= 0; --i);
        buf[i+1] = '\0';
    }
    return buf;
}

const char* Directory::InterpSlashSlash (const char* path) {
    for (int i = strlen(path)-1; i > 0; --i) {
        if (path[i] == '/' && path[i-1] == '/') {
            return &path[i];
        }
    }
    return path;
}

const char* Directory::ElimDot (const char* path) {
    static char newpath[MAXPATHLEN+1];
    const char* src;
    char* dest = newpath;

    for (src = path; src < &path[strlen(path)]; ++src) {
        if (!DotSlash(src)) {
            *dest++ = *src;

        } else if (*(dest-1) == '/') {
            ++src;

        } else {
            *dest++ = *src;
        }            
    }
    *dest = '\0';
    return newpath;
}

static boolean CollapsedDotDotSlash (const char* path, const char*& start) {
    if (path == start || *(start-1) != '/') {
        return false;

    } else if (path == start-1 && *path == '/') {
        return true;

    } else if (path == start-2) {               // NB: won't handle '//' right
        start = path;
        return *start != '.';

    } else if (path < start-2 && !DotDotSlash(start-3)) {
        for (start -= 2; path <= start; --start) {
            if (*start == '/') {
                ++start;
                return true;
            }
        }
        start = path;
        return true;
    }
    return false;
}

const char* Directory::ElimDotDot (const char* path) {
    static char newpath[MAXPATHLEN+1];
    const char* src;
    char* dest = newpath;

    for (src = path; src < &path[strlen(path)]; ++src) {
        if (DotDotSlash(src) && CollapsedDotDotSlash(newpath, dest)) {
            src += 2;
        } else {
            *dest++ = *src;
        }
    }
    *dest = '\0';
    return newpath;
}

const char* Directory::InterpTilde (const char* path) {
    static char realpath[MAXPATHLEN+1];
    const char* beg = strrchr(path, '~');
    boolean validTilde = beg != NULL && (beg == path || *(beg-1) == '/');

    if (validTilde) {
        const char* end = strchr(beg, '/');
        int length = (end == nil) ? strlen(beg) : end - beg;
        const char* expandedTilde = ExpandTilde(beg, length);

        if (expandedTilde == nil) {
            validTilde = false;
        } else {
            strcpy(realpath, expandedTilde);
            if (end != nil) {
                strcat(realpath, end);
            }
        }
    }
    return validTilde ? realpath : path;
}

const char* Directory::ExpandTilde (const char* tildeName, int length) {
    const char* name = nil;

    if (length > 1) {
        static char buf[MAXNAMLEN+1];
        strncpy(buf, tildeName+1, length-1);
        buf[length-1] = '\0';
        name = buf;
    }
    return Home(name);
}        

void Directory::Insert (const char* s, int index) {
    BufInsert(strdup(s), index, (void**&) strbuf, strbufsize, strcount);
}

void Directory::Remove (int index) {
    if (0 <= index && index < strcount) {
        delete strbuf[index];
        BufRemove(index, (void**) strbuf, strcount);
    }
}

void Directory::Clear () {
    for (int i = 0; i < strcount; ++i) {
        delete strbuf[i];
    }
    strcount = 0;
}

int Directory::Position (const char* s) {
    int i;

    for (i = 0; i < strcount; ++i) {
        if (strcmp(s, strbuf[i]) < 0) {
            return i;
        }
    }
    return strcount;
}

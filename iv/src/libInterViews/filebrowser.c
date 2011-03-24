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
 * FileBrowser implementation.
 */

#include <InterViews/filebrowser.h>

#include <bstring.h>
#include <pwd.h>
#include <os/auth.h>
#include <os/fs.h>
#include <string.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <sys/stat.h>

/*****************************************************************************/

class FBDirectory {       
public:
    FBDirectory(const char* name);
    ~FBDirectory();

    boolean LoadDirectory(const char*);
    const char* Normalize(const char*);
    const char* ValidDirectories(const char*);

    int Index(const char*);
    const char* File(int index);
    int Count();

    boolean IsADirectory(const char*);
private:
    const char* Home(const char* = nil);
    const char* ElimDot(const char*);
    const char* ElimDotDot(const char*);
    const char* InterpSlashSlash(const char*);
    const char* InterpTilde(const char*);
    const char* ExpandTilde(const char*, int);
    const char* RealPath(const char*);

    boolean Reset(const char*);
    void Clear();
    void Check(int index);
    void Insert(const char*, int index);
    void Append(const char*);
    void Remove(int index);
    virtual int Position(const char*);
private:
    char** strbuf;
    int strcount;
    int strbufsize;
};

inline int FBDirectory::Count () { return strcount; }
inline void FBDirectory::Append (const char* s) { Insert(s, strcount); }
inline const char* FBDirectory::File (int index) { 
    return (0 <= index && index < strcount) ? strbuf[index] : nil;
}

inline char* strdup (const char* s) {
    char* dup = new char[strlen(s) + 1];
    strcpy(dup, s);
    return dup;
}

FBDirectory::FBDirectory (const char* name) {
    const int defaultSize = 256;

    strbufsize = defaultSize;
    strbuf = new char*[strbufsize];
    strcount = 0;
    LoadDirectory(name);
}

FBDirectory::~FBDirectory () {
    Clear();
}

const char* FBDirectory::RealPath (const char* path) {
    const char* realpath;

    if (*path == '\0') {
        realpath = "./";
    } else {
        realpath = InterpTilde(InterpSlashSlash(path));
    }
    return realpath;
}

boolean FBDirectory::LoadDirectory (const char* name) {
    char buf[MAXPATHLEN+2];
    const char* path = buf;

    strcpy(buf, ValidDirectories(RealPath(name)));
    return Reset(buf);
}

int FBDirectory::Index (const char* name) {
    for (int i = 0; i < strcount; ++i) {
        if (strcmp(strbuf[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

boolean FBDirectory::Reset (const char* path) {
    boolean successful = IsADirectory(path);
    if (successful) {
	DIR* dir = opendir(path);
        Clear();
        for (struct direct* d = readdir(dir); d != NULL; d = readdir(dir)) {
            Insert(d->d_name, Position(d->d_name));
        }
        closedir(dir);
    }
    return successful; 
}

boolean FBDirectory::IsADirectory (const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && (st.st_mode & S_IFMT) == S_IFDIR;
}

const char* FBDirectory::Home (const char* name) {
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

const char* FBDirectory::Normalize (const char* path) {
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

    } else if (IsADirectory(buf) && buf[strlen(buf)-1] != '/') {
        strcpy(newpath, buf);
        strcat(newpath, "/");

    } else {
        strcpy(newpath, buf);
    }
    return newpath;
}

const char* FBDirectory::ValidDirectories (const char* path) {
    static char buf[MAXPATHLEN+1];
    strcpy(buf, path);
    int i = strlen(path);

    while (!IsADirectory(RealPath(buf)) && i >= 0) {
        for (--i; buf[i] != '/' && i >= 0; --i);
        buf[i+1] = '\0';
    }
    return buf;
}

const char* FBDirectory::InterpSlashSlash (const char* path) {
    for (int i = strlen(path)-1; i > 0; --i) {
        if (path[i] == '/' && path[i-1] == '/') {
            return &path[i];
        }
    }
    return path;
}

const char* FBDirectory::ElimDot (const char* path) {
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

const char* FBDirectory::ElimDotDot (const char* path) {
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

const char* FBDirectory::InterpTilde (const char* path) {
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

const char* FBDirectory::ExpandTilde (const char* tildeName, int length) {
    const char* name = nil;

    if (length > 1) {
        static char buf[MAXNAMLEN+1];
        strncpy(buf, tildeName+1, length-1);
        buf[length-1] = '\0';
        name = buf;
    }
    return Home(name);
}        

void FBDirectory::Check (int index) {
    char** newstrbuf;

    if (index >= strbufsize) {
        strbufsize = (index+1) * 2;
        newstrbuf = new char*[strbufsize];
        bcopy(strbuf, newstrbuf, strcount*sizeof(char*));
        delete strbuf;
        strbuf = newstrbuf;
    }
}

void FBDirectory::Insert (const char* s, int index) {
    const char** spot;
    index = (index < 0) ? strcount : index;

    if (index < strcount) {
        Check(strcount+1);
        spot = &strbuf[index];
        bcopy(spot, spot+1, (strcount - index)*sizeof(char*));
    } else {
        Check(index);
        spot = &strbuf[index];
    }
    char* string = strdup(s);
    *spot = string;
    ++strcount;
}

void FBDirectory::Remove (int index) {
    if (index < --strcount) {
        const char** spot = &strbuf[index];
        delete spot;
        bcopy(spot+1, spot, (strcount - index)*sizeof(char*));
    }
}

void FBDirectory::Clear () {
    for (int i = 0; i < strcount; ++i) {
        delete strbuf[i];
    }
    strcount = 0;
}

int FBDirectory::Position (const char* s) {
    int i;

    for (i = 0; i < strcount; ++i) {
        if (strcmp(s, strbuf[i]) < 0) {
            return i;
        }
    }
    return strcount;
}

/*****************************************************************************/

FileBrowser::FileBrowser (
    ButtonState* bs, const char* dir, int r, int c,
    boolean u, int h, const char* d
) : (bs, r, c, u, h, d) {
    Init(dir);
    Update();
}

FileBrowser::FileBrowser (
    const char* name, ButtonState* bs, const char* dir, int r, int c,
    boolean u, int h, const char* d
) : (name, bs, r, c, u, h, d) {
    Init(dir);
    Update();
}

void FileBrowser::Init (const char* d) {
    dir = new FBDirectory(d);
    lastpath = strdup(ValidDirectories(Normalize(d)));
}

FileBrowser::~FileBrowser () {
    delete dir;
    delete lastpath;
}

static const char* Concat (const char* path, const char* file) {
    static char buf[MAXPATHLEN+1];

    strcpy(buf, path);
    if (path[strlen(path)-1] != '/') {
        strcat(buf, "/");
    }
    return strcat(buf, file);
}

boolean FileBrowser::IsADirectory (const char* path) {
    return dir->IsADirectory(Normalize(path));
}

boolean FileBrowser::SetDirectory (const char* path) {
    boolean successful = true;
    path = ValidDirectories(path);
    const char* normpath = Normalize(path);

    if (strcmp(normpath, lastpath) != 0) {
        char* newnormpath = strdup(normpath);
        successful = dir->LoadDirectory(newnormpath);

        if (successful) {
            delete lastpath;
            lastpath = newnormpath;
            Update();
        } else {
            delete newnormpath;
        }
    }
    return successful;
}

const char* FileBrowser::ValidDirectories (const char* path) {
    return dir->ValidDirectories(path);
}

const char* FileBrowser::Normalize (const char* path) {
    return dir->Normalize(path);
}

const char* FileBrowser::Path (int index) {
    const char* s = String(index);

    return (s == nil ) ? nil : Normalize(Concat(lastpath, s));
}

void FileBrowser::Update () {
    Clear();

    for (int i = 0; i < dir->Count(); ++i) {
        const char* name = dir->File(i);

        if (dir->IsADirectory(Concat(lastpath, name))) {
            char buf[MAXPATHLEN+1];
            strcpy(buf, name);
            strcat(buf, "/");
            Append(buf);
        } else {
            Append(name);
        }
    }
}

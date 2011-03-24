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
 * ClassBuffer implementation.
 */

#include "classbuffer.h"
#include "classinfo.h"
#include "direct.h"
#include "globals.h"

#include <InterViews/defs.h>
#include <InterViews/regexp.h>
#include <InterViews/textbuffer.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stream.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/*****************************************************************************/

static const int MINTEXTSIZE = 10000;

/*****************************************************************************/

class Classes {
public:
    Classes();
    ~Classes();

    void Include(ClassInfo*);
    ClassInfo* Class(int index);
    ClassInfo* Find(const char* className);
    void Clear();
private:
    void Insert(ClassInfo*, int index);
    void Remove(int index);
    int Index(ClassInfo*);
    int Position(ClassInfo*);
private:
    ClassInfo** _buf;
    int _bufsize;
    int _count;
};

Classes::Classes () {
    const int defaultSize = 64;

    _bufsize = defaultSize;
    _buf = new ClassInfo*[_bufsize];
    _count = 0;
}

Classes::~Classes () {
    Clear();
    delete _buf;
}

void Classes::Include (ClassInfo* info) {
    Insert(info, Position(info));
}

ClassInfo* Classes::Class (int index) {
    return (0 <= index && index < _count) ? _buf[index] : nil;
}

ClassInfo* Classes::Find (const char* className) {
    for (int i = 0; i < _count; ++i) {
        if (strcmp(className, _buf[i]->Name()) == 0) {
            return _buf[i];
        }
    }
    return nil;
}

void Classes::Insert (ClassInfo* info, int index) {
    BufInsert(info, index, (void**&) _buf, _bufsize, _count);
}

void Classes::Remove (int index) {
    if (0 <= index && index < _count) {
        BufRemove(index, (void**) _buf, _count);
    }
}

int Classes::Index (ClassInfo* info) {
    for (int i = 0; i < _count; ++i) {
        if (info == _buf[i]) {
            return i;
        }
    }
    return -1;
}

int Classes::Position (ClassInfo* info) {
    for (int i = 0; i < _count; ++i) {
        if (strcmp(info->Name(), _buf[i]->Name()) < 0) {
            return i;
        }
    }
    return _count;
}

void Classes::Clear () {
    for (int i = 0; i < _count; ++i) {
        delete _buf[i];
    }
    _count = 0;
}

/*****************************************************************************/

ClassBuffer::ClassBuffer (boolean recursive, boolean verbose) {
    _classes = new Classes;
    _recursive = recursive;
    _verbose = verbose;
}

ClassBuffer::~ClassBuffer () {
    delete _classes;
}

const char* ClassBuffer::Class (int index) {
    ClassInfo* info = _classes->Class(index);
    return (info == nil) ? nil : info->Name();
}

const char* ClassBuffer::Parent (const char* className, int index) {
    ClassInfo* info = _classes->Find(className);
    ClassInfo* parentInfo = (info == nil) ? nil : info->Parent(index);
    return (parentInfo == nil) ? nil : parentInfo->Name();
}

const char* ClassBuffer::Child (const char* className, int index) {
    ClassInfo* info = _classes->Find(className);
    ClassInfo* childInfo = (info == nil) ? nil : info->Child(index);
    return (childInfo == nil) ? nil : childInfo->Name();
}

const char* ClassBuffer::Path (const char* className) {
    ClassInfo* info = _classes->Find(className);
    return (info == nil) ? nil : info->Path();
}

int ClassBuffer::LineNumber (const char* className) {
    ClassInfo* info = _classes->Find(className);
    return (info == nil) ? nil : info->LineNumber();
}

ClassInfo* ClassBuffer::Info (const char* className) {
    return _classes->Find(className);
}    

inline boolean DotOrDotDot (const char* dir) {
    return strcmp(dir, ".") == 0 || strcmp(dir, "..") == 0;
}

inline boolean HeaderFile (const char* file) {
    int length = strlen(file);

    return file[length-1] == 'h' && file[length-2] == '.';
}

static boolean IsADirectory (const char* path, struct stat& filestats) {
    stat(path, &filestats);
    return filestats.st_mode & S_IFDIR;
}

void ClassBuffer::Search (const char* path) {
    struct stat filestats;

    if (IsADirectory(path, filestats)) {
        if (_recursive) {
            SearchDirs(path);
        } else {
            SearchDir(path);
        }

    } else if (HeaderFile(path)) {
        SearchFile(path, filestats);
    }
}

void ClassBuffer::SearchDir (const char* path) {
    Directory dir(path);

    if (_verbose) {
        cout << "searching directory " << path << "\n";
    }

    for (int i = 0; i < dir.Count(); ++i) {
        const char* file = dir.File(i);

        if (HeaderFile(file)) {
            struct stat filestats;
            char filePath[MAXPATHLEN+1];
            strcpy(filePath, dir.Normalize(path));
            strcat(filePath, file);
        
            if (!IsADirectory(filePath, filestats)) {
                SearchFile(filePath, filestats);
            }
        }
    }            
}

void ClassBuffer::SearchDirs (const char* path) {
    Directory dir(path);

    if (_verbose) {
        cout << "recursively searching directory " << path << "\n";
    }

    for (int i = 0; i < dir.Count(); ++i) {
        const char* file = dir.File(i);

        if (!DotOrDotDot(file)) {
            char filePath[MAXPATHLEN+1];
            strcpy(filePath, dir.Normalize(path));
            strcat(filePath, file);

            Search(filePath);
        }
    }
}

void ClassBuffer::SearchFile (const char* path, struct stat& filestats) {
    FILE* f = fopen(path, "r");

    if (f == nil) {
        return;
    }
    if (_verbose) {
        cout << "searching file " << path << "\n";
    }

    int bufsize = max(round(filestats.st_size * 1.2), MINTEXTSIZE);
    char* buf = new char[bufsize];
    char* b = buf;
    int remaining = bufsize;

    while (remaining > 1 && fgets(b, remaining, f) != nil) {
        int l = strlen(b);
        remaining -= l;
        b += l;
    }
    fclose(f);
    TextBuffer textbuf(buf, b-buf, bufsize);

    SearchTextBuffer(&textbuf, path);

    delete buf;
}

void ClassBuffer::SearchTextBuffer (TextBuffer* tb, const char* path) {
    int beg = 0;

    for (;;) {
        char* className = FindClassDecl(tb, beg);

        if (className == nil) {
            break;
        }
        ClassInfo* info = _classes->Find(className);
        
        if (info == nil) {
            info = new ClassInfo(className, path, tb->LineNumber(beg));
            _classes->Include(info);
        } else {
            info->Path(path);
            info->LineNumber(tb->LineNumber(beg));
        }

        for (;;) {
            char* parentName = ParentName(tb, beg);

            if (parentName == nil) {
                break;
            }
            ClassInfo* parentInfo = _classes->Find(parentName);

            if (parentInfo == nil) {
                parentInfo = new ClassInfo(parentName);
                _classes->Include(parentInfo);
            }
            info->IncludeParent(parentInfo);
            parentInfo->IncludeChild(info);
            delete parentName;
        }
        delete className;
    }
}

inline boolean KeyWord (const char* string) {
    return 
        strcmp(string, "public") == 0 ||
        strcmp(string, "protected") == 0 ||
        strcmp(string, "private") == 0 ||
        strcmp(string, "virtual") == 0;
}

char* ClassBuffer::ParentName (TextBuffer* tb, int& beg) {
    Regexp delimiter("{");
    int delim = tb->ForwardSearch(&delimiter, beg);

    for (;;) {
        char* string = Identifier(tb, beg);

        if (string == nil || beg >= delim) {
            delete string;
            beg = delim;
            return nil;

        } else if (KeyWord(string)) {
            delete string;
            string = nil;

        } else {
            return string;
        }
    }
}

char* ClassBuffer::FindClassDecl (TextBuffer* tb, int& beg) {
    Regexp classKeyWord("^class[ $]");
    Regexp delimiter("[:{;]");
    char* className = nil;

    for (;;) {
        beg = tb->ForwardSearch(&classKeyWord, beg);

        if (beg < 0) {
            break;
        }
        int tmp = beg;
        int delim = tb->ForwardSearch(&delimiter, tmp);
            
        if (delim >= 0 && *tb->Text(delim-1) != ';') {
            className = Identifier(tb, beg);
            break;
        }
    }
    return className;
}

inline boolean IsValidChar (char c) {
    return isalpha(c) || c == '_' || c == '$';
}

char* ClassBuffer::Identifier (TextBuffer* tb, int& beg) {
    int i, j;
    const char* text = tb->Text();
    char* string = nil;

    for (i = beg; i < tb->Length(); ++i) {
        if (IsValidChar(text[i])) {
            break;
        }
    }
    for (j = i+1; j < tb->Length(); ++j) {
        char c = text[j];

        if (!IsValidChar(c) && !isdigit(c)) {
            break;
        }
    }
    if (j < tb->Length()) {
        string = strndup(&text[i], j-i);
        beg = j;
    }
    return string;
}

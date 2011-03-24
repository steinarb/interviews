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
 * PFile encapsulates standard file operations.
 */

#ifndef pfile_h
#define pfile_h

#include <stdio.h>
#include <InterViews/defs.h>

class PFile {
public:
    PFile(char* filename);
    ~PFile();
    
    char* GetName();
    boolean Exists();
    boolean Exists(char* filename);

    boolean Read(short& i);
    boolean Read(int& i);
    boolean Read(float& f);
    boolean Read(char& c);
    boolean Read(short* i, int count);
    boolean Read(int* i, int count);
    boolean Read(long* i, int count);
    boolean Read(float* f, int count);
    boolean Read(char* string);
    boolean Read(char* string, int count);

    boolean Write(int i);
    boolean Write(float f);
    boolean Write(short* i, int count);
    boolean Write(int* i, int count);
    boolean Write(long* i, int count);
    boolean Write(float* f, int count);
    boolean Write(char* string);
    boolean Write(char* string, int count);
    
    boolean SeekTo(long offset); /* offset bytes from beginning of file */
    boolean SeekToBegin();
    boolean SeekToEnd();

    long CurOffset();
    boolean IsEmpty();
    boolean Erase();
protected:
    char* name;
    FILE* fd;
};

#endif

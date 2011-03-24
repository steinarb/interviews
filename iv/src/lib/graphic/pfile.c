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
 * Implementation of PFile class.
 */

#include <InterViews/defs.h>
#include <InterViews/Graphic/pfile.h>

#include <osfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>

static const int FROM_BEGINNING = 0;
static const int FROM_CURPOS = 1;
static const int FROM_END = 2;

PFile::PFile (char* filename) {
    name = new char [strlen(filename) + 1];
    strcpy(name, filename);
    fd = fopen(name, "r+");
    if (fd == nil) {
	fd = fopen(name, "w+");
	if (fd == nil) {
	    fflush(stdout);
	    fprintf(stderr, "couldn't open %s\n", name);
	    exit(2);
	}
    }
}

PFile::~PFile () {
    if (fd != nil) {
	fclose(fd);
    }
    delete name;
}

char* PFile::GetName () {
    return name;
}

boolean PFile::Exists () {
    return access(name, F_OK) >= 0;
}

boolean PFile::Exists (char* filename) {
    return access(filename, F_OK) >= 0;
}

boolean PFile::Read (short& i) {
    int tmp;

    boolean ok = fread((char*) &tmp, sizeof(int), 1, fd) == 1;
    i = short(tmp);
    return ok;
}

boolean PFile::Read (int& i)  {
    return fread((char*) &i, sizeof(int), 1, fd) == 1; 
}

boolean PFile::Read (float& f) {
    double tmp;
    
    boolean ok = fread((char*) &tmp, sizeof(double), 1, fd) == 1; 
    f = float(tmp);
    return ok;
}

boolean PFile::Read (char& c) {
    int tmp;

    boolean ok = fread((char*) &tmp, sizeof(int), 1, fd) == 1;
    c = char(tmp);
    return ok;
}

boolean PFile::Read (short* i, int count) {
    return fread((char*) i, sizeof(short), count, fd) == count; 
}

boolean PFile::Read (int* i, int count) {
    return fread((char*) i, sizeof(int), count, fd) == count; 
}

boolean PFile::Read (long* i, int count) {
    return fread((char*) i, sizeof(long), count, fd) == count; 
}

boolean PFile::Read (float* f, int count) {
    return fread((char*) f, sizeof(float), count, fd) == count; 
}

boolean PFile::Read (char* string) {
    // will read up to a null character or count characters
    // assumes that there is room to read all count characters
    // string will be null terminated iff null was read
    char c;

    while (fread((char*) &c, sizeof(char), 1, fd) == 1) {
	*string = c;
	string++;
	if ( c == '\0' ) {
	    return true;
	}
    }
    return false;
}

boolean PFile::Read (char* string, int count) {
    return fread((char*) string, sizeof(char), count, fd) == count; 
}

boolean PFile::Write (int i) {
    return fwrite((char*) &i, sizeof(int), 1, fd) == 1; 
}

boolean PFile::Write (float f) {
    double tmp = double(f);
    return fwrite((char*) &tmp, sizeof(double), 1, fd) == 1; 
}

boolean PFile::Write (short* i, int count) {
    return fwrite((char*) i, sizeof(short), count, fd) == count;
}

boolean PFile::Write (int* i, int count) {
    return fwrite((char*) i, sizeof(int), count, fd) == count; 
}

boolean PFile::Write (long* i, int count) {
    return fwrite((char*) i, sizeof(long), count, fd) == count; 
}

boolean PFile::Write (float* f, int count) {
    return fwrite((char*) f, sizeof(float), count, fd) == count; 
}

boolean PFile::Write (char* string) {
    // write up to and including null character
    // beware of non-terminated strings!

    int len = strlen( string ) + 1;
    return fwrite((char*) string, sizeof(char), len, fd) == len;
}

boolean PFile::Write (char* string, int count) {
    return fwrite((char*) string, sizeof(char), count, fd) == count; 
}


boolean PFile::SeekTo (long offset) {
    return fseek(fd, offset, FROM_BEGINNING) >= 0; 
}	

boolean PFile::SeekToBegin () {
    return fseek(fd, 0, FROM_BEGINNING) >= 0; 
}

boolean PFile::SeekToEnd ()  {
    return fseek(fd, 0, FROM_END) >= 0; 
}

long PFile::CurOffset () {
    return ftell(fd); 
}

boolean PFile::IsEmpty () {
    int dummy;

    if (Read(dummy)) {
	rewind(fd);
	return false;
    } else {
	return true;
    }
}

boolean PFile::Erase () {
    return 
	unlink(name) == 0 && fclose(fd) != EOF && 
	(fd = fopen(name, "w+")) != nil;
}

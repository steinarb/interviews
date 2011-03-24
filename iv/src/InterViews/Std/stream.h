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
 * C++ stream I/O
 */

#ifndef stream_h
#define stream_h

#include <InterViews/defs.h>
#include <stdio.h>
#include <os/fs.h>

enum open_mode { input = 0, output = 1, append = 2 };

class streambuf {
public:
    streambuf();
    streambuf(char* base, int length);
    ~streambuf();

    virtual int fill();
    virtual int flush(int c = EOF);

    int sgetc();
    int snextc();
    void stossc();
    void sputbackc(char);
    int sputc(int c = EOF);

    streambuf* setbuf(char* base, int len, int count = 0);
    char* base () { return _base; }
protected:
    boolean allocate();
protected:
    char* _base;
    char* _free;
    char* _used;
    char* _end;
    boolean _allocated;
    FILE* _fp;
};

class filebuf : public streambuf {
public:
    filebuf();
    filebuf(FILE*);
    filebuf(int fd);
    filebuf(int fd, char* base, int length);
    ~filebuf();

    filebuf* open(char* name, open_mode om);
    int close();

    virtual int fill();
    virtual int flush(int c = EOF);
private:
    int _fd;
    char _isopen;
};

class circbuf : public streambuf {
public:
    circbuf () {}
    ~circbuf () {}

    virtual int fill();
    virtual int flush(int c = EOF);
};

class whitespace {
public:
};

/*
 * Abstract class for sharing io status between istream and ostream.
 */

class iostream {
public:
    boolean operator !();
    boolean good();
    boolean eof();
    boolean fail();
    boolean bad();
    int status () { return _status; }
    void status (int s) { _status |= s; }
    void clear (int s = 0) { _status = s; }
    char* bufptr () { return _buf->base(); }
protected:
    iostream();
    iostream(int fd);
    iostream(streambuf*);
private:
    friend class istream;
    friend class ostream;

    streambuf* _buf;
    int _status;
};

class ostream : public iostream {
public:
    ostream(int fd);
    ostream(int size, char* base);
    ostream(streambuf*);
    ~ostream();

    operator void* () { return fail() ? nil : this; }
    ostream& operator <<(const char*);
    ostream& put(char);
    ostream& operator << (int i) { return *this << (long)i; }
    ostream& operator << (unsigned u) { return *this << (unsigned long)u; }
    ostream& operator <<(long);
    ostream& operator <<(unsigned long);
    ostream& operator <<(double);
    ostream& operator <<(streambuf&);

    ostream& put(const char*, char);
    ostream& put(const char*, char*);
    ostream& put (const char* s, int i) { return put(s, (long)i); }
    ostream& put (const char* s, unsigned u) {
	return put(s, (unsigned long)u);
    }
    ostream& put(const char*, long);
    ostream& put(const char*, unsigned long);
    ostream& put(const char*, double);

    ostream& flush();
private:
    friend class istream;
};

class istream : public iostream {
public:
    istream(int fd, boolean skip = true, ostream* tied = nil);
    istream(int size, char* base, boolean skip = true);
    istream(streambuf* s, boolean skip = true, ostream* tied = nil);

    operator void* () { return fail() ? nil : this; }
    istream& operator >>(char*);
    istream& operator >>(char&);
    istream& operator >>(short&);
    istream& operator >>(int&);
    istream& operator >>(long&);
    istream& operator >>(float&);
    istream& operator >>(double&);
    istream& operator >>(streambuf&);
    istream& operator >>(whitespace&);

    istream& get(char*, int, char = '\n');
    istream& get(streambuf&, char = '\n');
    istream& get(char&);
    istream& putback(char);
    ostream* tie(ostream*);

    boolean skip(boolean);
private:
    friend ostream;

    boolean _skipws;
    ostream* _tied;

    void doskip();
    void sync();
};

extern istream cin;
extern ostream cout;
extern ostream cerr;

extern whitespace WS;

#endif

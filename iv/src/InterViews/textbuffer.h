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
 * TextBuffer - editable text buffer
 */

#ifndef textbuffer_h
#define textbuffer_h

#include <InterViews/defs.h>

class Regexp;

class TextBuffer {
public:
    TextBuffer(char* buffer, int length, int size);
    ~TextBuffer();

    int Search(Regexp* regexp, int index, int range, int stop);
    int ForwardSearch(Regexp* regexp, int index);
    int BackwardSearch(Regexp* regexp, int index);

    int Match(Regexp* regexp, int index, int stop);
    boolean ForwardMatch(Regexp* regexp, int index);
    boolean BackwardMatch(Regexp* regexp, int index);

    virtual int Insert(int index, const char* string, int count);
    virtual int Delete(int index, int count);
    int Copy(int index, char* buffer, int count);

    int Height();
    int Width();
    int Length();

    const char* Text();
    const char* Text(int index);
    const char* Text(int index1, int index2);
    char Char (int index);

    int LineIndex(int line);
    int LinesBetween(int index1, int index2);
    int LineNumber(int index);
    int LineOffset (int index);

    int PreviousCharacter(int index);
    int NextCharacter(int index);

    boolean IsBeginningOfText(int index);
    int BeginningOfText();

    boolean IsEndOfText(int index);
    int EndOfText();

    boolean IsBeginningOfLine(int index);
    int BeginningOfLine(int index);
    int BeginningOfNextLine(int index);

    boolean IsEndOfLine(int index);
    int EndOfLine(int index);
    int EndOfPreviousLine(int index);

    boolean IsBeginningOfWord(int index);
    int BeginningOfWord(int index);
    int BeginningOfNextWord(int index);

    boolean IsEndOfWord(int index);
    int EndOfWord(int index);
    int EndOfPreviousWord(int index);
protected:
    char* text;
    int length;
    int size;
private:
    int linecount;
    int lastline;
    int lastindex;
};

inline char TextBuffer::Char (int i) {
    return (i<0) ? text[0] : (i>length) ? text[length] : text[i];
}
inline const char* TextBuffer::Text () {
    return text;
}
inline const char* TextBuffer::Text (int i) {
    return text + ((i<0) ? 0 : (i>length) ? length : i);
}
inline const char* TextBuffer::Text (int i, int) {
    return text + ((i<0) ? 0 : (i>length) ? length : i);
}
inline int TextBuffer::PreviousCharacter (int i) {
    return (i<=0) ? 0 : i-1;
}
inline int TextBuffer::NextCharacter (int i) {
    return (i>=length) ? length : i+1;
}
inline boolean TextBuffer::IsBeginningOfText (int i) {
    return i <= 0;
}
inline int TextBuffer::BeginningOfText () {
    return 0;
}
inline boolean TextBuffer::IsEndOfText (int i) {
    return i >= length;
}
inline int TextBuffer::EndOfText () {
    return length;
}
inline int TextBuffer::Height () {
    return linecount;
}
inline int TextBuffer::Length () {
    return length;
}

#endif

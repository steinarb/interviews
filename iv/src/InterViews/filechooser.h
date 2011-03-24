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
 * FileChooser - a StringChooser for choosing files from a directory.
 */

#ifndef filechooser_h
#define filechooser_h

#include <InterViews/strchooser.h>

class Button;
class FileBrowser;
class MarginFrame;

class FileChooser : public StringChooser {
public:
    FileChooser(
        const char* title = "Please select a file:",
        const char* subtitle = "",
        const char* dir = "~",
        int rows = 10, int cols = 24,
        const char* acceptLabel = " Open ",
        Alignment = Center
    );
    FileChooser(
        const char* name, const char* title, const char* subtitle,
        const char* dir, int, int, const char* acceptLabel, Alignment
    );

    void SetTitle(const char*);
    void SetSubtitle(const char*);
    void SelectFile();

    virtual boolean Accept();
protected:
    FileChooser(ButtonState*, Alignment = Center);
    FileChooser(ButtonState*, const char* dir, int rows, int cols, Alignment);
    void Init(const char*, const char*);
    Interactor* Interior(const char* acptlbl);
    Interactor* AddScroller(Interactor*);
    FileBrowser* browser();

    virtual void UpdateEditor();
    virtual void UpdateBrowser();
protected:
    MarginFrame* title, *subtitle;
};

inline FileBrowser* FileChooser::browser () { return (FileBrowser*) _browser; }

#endif

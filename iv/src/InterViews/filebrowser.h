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
 * FileBrowser - a StringBrowser that displays file names.
 */

#ifndef filebrowser_h
#define filebrowser_h

#include <InterViews/strbrowser.h>

class FileBrowser : public StringBrowser {
public:
    FileBrowser(
        ButtonState*, const char* dir = "~", int rows = 10, int cols = 24,
        boolean uniqueSel = true, int highlight = Reversed,
        const char* done = SBDone
    );
    FileBrowser(
        const char* name, ButtonState*, const char* = "~", int = 10, int = 24,
        boolean = true, int = Reversed, const char* = SBDone
    );
    virtual ~FileBrowser();

    boolean IsADirectory(const char*);
    boolean SetDirectory(const char*);
    const char* GetDirectory();
    const char* ValidDirectories(const char*);
    const char* Normalize(const char*);
    const char* Path(int index);

    virtual void Update();
private:
    void Init(const char*);
private:
    class FBDirectory* dir;
    char* lastpath;
};

inline const char* FileBrowser::GetDirectory () { return lastpath; }

#endif

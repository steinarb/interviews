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
 * CompletionEditor - StringEditor with completion
 */

#include <compeditor.h>
#include <InterViews/textbuffer.h>
#include <InterViews/world.h>
#include <string.h>

CompletionEditor::CompletionEditor (
    ButtonState* s, const char* sample, const char* done
) : (s, sample, done) {
    Init();
}

CompletionEditor::CompletionEditor (
    const char* name, ButtonState* s, const char* sample, const char* done
) : (name, s, sample, done) {
    Init();
}

void CompletionEditor::Init () {
    completions = nil;
    count = 0;
}

void CompletionEditor::Completions (const char* list[], int c) {
    completions = list;
    count = c;
}

boolean CompletionEditor::HandleChar (char c) {
    if (c == CEComplete) {
        InsertText("", 0);

        const char* best = nil;
        int match = 0;
        int length = text->LineOffset(text->EndOfLine(0));
        for (int i = 0; i < count; ++i) {
            for (int j = 0; ; ++j) {
                char c = completions[i][j];
                if (j < length) {
                    if (text->Char(j) != c) {
                        match = max(match, j);
                        break;
                    }
                } else {
                    if (best == nil) {
                        best = completions[i];
                        match = strlen(best);
                        break;
                    } else {
                        if (c == '\0' || best[j] != c) {
                            match = min(match, j);
                            break;
                        }
                    }
                }
            }
        }

        Select(match, length);
        if (match > length) {
            InsertText(best+length, match-length);
        } else if (best != nil && best[match] != '\0') {
            GetWorld()->RingBell(1);
        }
        return false;
    } else {
        return StringEditor::HandleChar(c);
    }
}

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
 * TextDisplay - basic text displaying
 */

#include <InterViews/font.h>
#include <InterViews/painter.h>
#include <InterViews/shape.h>
#include <InterViews/textdisplay.h>
#include <bstring.h>
#include <string.h>

class TextLine {
public:
    TextLine();
    ~TextLine();

    void Style(TextDisplay*, int line, int first, int last, int style);
    void AddStyle(TextDisplay*, int line, int first, int last, int style);
    void RemoveStyle(TextDisplay*, int line, int first, int last, int style);

    void Insert(TextDisplay*, int line, int index, const char*, int count);
    void Delete(TextDisplay*, int line, int index, int count);
    void Replace(TextDisplay*, int line, const char*, int count);

    void Draw(TextDisplay*, int line, int first, int last);

    int Index(TextDisplay*, Coord x, boolean between);
    Coord Offset(TextDisplay*, int index);
private:
    void Size(int);
    char* text;
    char* attr;
    int size;
    int lastchar;
    char prefix;
    char postfix;
};

TextDisplay::TextDisplay (boolean a) {
    painter = nil;
    canvas = nil;
    autosized = a;
    xmin = 0;
    xmax = 0;
    ymax = 0;
    ymin = 0;
    x0 = 0;
    y0 = 0;
    width = -1;
    lineheight = 1;
    tabwidth = 0;
    firstline = 0;
    lastline = 0;
    topline = 0;
    bottomline = -1;
    widestline = 0;
    lines = nil;
    maxlines = 0;
    Size(firstline, lastline);
    CaretStyle(DefaultCaret);
    Caret(0, 0);
}

TextDisplay::~TextDisplay () {
    for (int i = firstline; i <= lastline; ++i) {
        delete Line(i, false);
    }
    delete lines;
}

void TextDisplay::Scroll (int line, Coord x, Coord y) {
    while (y < ymax) {
        line -= 1;
        y += lineheight;
    }
    while (y > ymax) {
        line += 1;
        y -= lineheight;
    }
    int yshift = y - Top(line);
    y0 += yshift;
    topline = line;
    bottomline = line + (y - ymin + 1) / lineheight - 1;
    if (canvas != nil && yshift > 0) {
        painter->Copy(
            canvas, xmin, ymin, xmax, ymax-yshift, canvas, xmin, ymin+yshift
        );
        Coord top = Top(topline);
        if (top < ymax) {
            Redraw(xmin, top+1, xmax, ymax);
        }
        Redraw(xmin, ymin, xmax, ymin+yshift-1);
    } else if (canvas != nil && yshift < 0) {
        painter->Copy(
            canvas, xmin, ymin-yshift, xmax, ymax, canvas, xmin, ymin
        );
        Coord bottom = Base(bottomline);
        if (bottom > ymin) {
            Redraw(xmin, ymin, xmax, bottom-1);
        }
        Redraw(xmin, ymax+yshift+1, xmax, ymax);
    }
    int xshift = x - Left(line, 0);
    x0 += xshift;
    if (canvas != nil && xshift > 0) {
        painter->Copy(
            canvas, xmin, ymin, xmax-xshift, ymax, canvas, xmin+xshift, ymin
        );
        Redraw(xmin, ymin, xmin+xshift-1, ymax);
    } else if (canvas != nil && xshift < 0) {
        painter->Copy(
            canvas, xmin-xshift, ymin, xmax, ymax, canvas, xmin, ymin
        );
        Redraw(xmax+xshift+1, ymin, xmax, ymax);
    }
}
    
void TextDisplay::Draw (Painter* p, Canvas* c) {
    painter = p;
    canvas = c;
}

void TextDisplay::LineHeight (Coord height) {
    lineheight = height;
}

void TextDisplay::TabWidth (Coord width) {
    tabwidth = width;
}

void TextDisplay::Resize (Coord xn, Coord yn, Coord xx, Coord yx) {
    xmin = xn;
    ymin = yn;
    xmax = xx;
    ymax = yx;
    bottomline = topline + (ymax+y0 - ymin + 1) / lineheight - 1;
}

void TextDisplay::Bounds (Coord& xn, Coord& yn, Coord& xx, Coord& yx) {
    xn = xmin;
    yn = ymin;
    xx = xmax;
    yx = ymax;
}

void TextDisplay::Redraw (Coord l, Coord b, Coord r, Coord t) {
    if (canvas != nil) {
        int first = LineNumber(t);
        int last = LineNumber(b);
        for (int i = first; i <= last; ++i) {
            int begin = LineIndex(i, l, false);
            int end = LineIndex(i, r, false);
            TextLine* line = Line(i, false);
            if (line != nil) {
                line->Draw(this, i, begin, end);
            } else {
                Coord base = Base(i);
                Coord top = Top(i);
                painter->ClearRect(canvas, l, max(base, b), r, min(top, t));
            }
            if (caretline == i && caretindex >= begin && caretindex <= end) {
                ShowCaret();
            }
        }
    }
}

void TextDisplay::Size (int first, int last) {
    if (last - first >= maxlines) {
        int newmaxlines = last - first + 10;
        TextLine** newlines = new TextLine* [newmaxlines];
        bzero(newlines, newmaxlines * sizeof(TextLine*));
        bcopy(lines, newlines, maxlines * sizeof(TextLine*));
        delete lines;
        lines = newlines;
        maxlines = newmaxlines;
    }
    if (first < firstline) {
        int count = firstline-first;
        bcopy(lines, lines+count, (lastline-firstline+1) * sizeof(TextLine*));
        bzero(lines, count * sizeof(TextLine*));
    }
    firstline = first;
    lastline = last;
}

TextLine* TextDisplay::Line (int line, boolean create) {
    if (create) {
        Size(min(firstline, line), max(lastline, line));
    }
    if (line < firstline || line > lastline) {
        return nil;
    } else {
        TextLine* l = lines[Index(line)];
        if (l == nil && create) {
            l = new TextLine();
            lines[Index(line)] = l;
        }
        return l;
    }
}

int TextDisplay::Index (int line) {
    return line - firstline;
}

void TextDisplay::InsertLinesAfter (int line, int count) {
    if (count > 0) {
        Size(min(firstline, line), max(lastline, line) + count);
        bcopy(
            lines + Index(line + 1), lines + Index(line + 1 + count),
            (lastline - line - count) * sizeof(TextLine*)
        );
        bzero(lines + Index(line+1), count * sizeof(TextLine*));
        if (canvas != nil) {
            if (autosized) {
                ymin = min(ymin, Base(lastline));
                bottomline = topline + (ymax+y0 - ymin + 1) / lineheight - 1;
            }
            Coord y = Base(line) - 1;
            int shift = count * lineheight;
            painter->Copy(
                canvas, xmin, ymin + shift, xmax, y, canvas, xmin, ymin
            );
            Coord bottom = Base(bottomline);
            if (bottom > ymin) {
                Redraw(xmin, ymin, xmax, bottom-1);
            }
            Redraw(xmin, y-shift+1, xmax, y);
        }
    }
}

void TextDisplay::InsertLinesBefore (int line, int count) {
    if (count > 0) {
        Size(min(firstline, line) - count, max(lastline, line));
        bcopy(
            lines + Index(firstline + count), lines + Index(firstline),
            (line - firstline - count) * sizeof(TextLine*)
        );
        bzero(lines + Index(line - count), count * sizeof(TextLine*));
        if (canvas != nil) {
            if (autosized) {
                ymax = max(ymax, Top(firstline));
                topline = bottomline - (ymax+y0 - ymin + 1) / lineheight + 1;
            }
            Coord y = Top(line) + 1;
            int shift = count * lineheight;
            painter->Copy(
                canvas, xmin, y, xmax, ymax-shift, canvas, xmin, y+shift
            );
            Coord top = Top(topline);
            if (top < ymax) {
                Redraw(xmin, top, xmax, ymax);
            }
            Redraw(xmin, y, xmax, y+shift-1);
        }
    }
}

void TextDisplay::DeleteLinesAfter (int line, int count) {
    count = min(count, lastline - line);
    if (count > 0) {
        Size(min(firstline, line), max(lastline, line));
        for (int i = 0; i < count; ++i) {
            delete Line(line+i+1, false);
        }
        bcopy(
            lines + Index(line + 1 + count), lines + Index(line + 1),
            (lastline - line - count) * sizeof(TextLine*)
        );
        bzero(lines + Index(lastline - count + 1), count * sizeof(TextLine*));
        if (canvas != nil) {
            Coord y = Base(line) - 1;
            int shift = count * lineheight;
            painter->Copy(
                canvas, xmin, ymin, xmax, y-shift, canvas, xmin, ymin+shift
            );
            Redraw(xmin, ymin, xmax, ymin+shift-1);
        }
        Size(firstline, lastline - count);
    }
}

void TextDisplay::DeleteLinesBefore (int line, int count) {
    count = min(count, line - firstline);
    if (count > 0) {
        Size(min(firstline, line), max(lastline, line));
        for (int i = 0; i < count; ++i) {
            delete Line(line-i-1, false);
        }
        bcopy(
            lines + Index(firstline), lines + Index(firstline + count),
            (line - firstline - count) * sizeof(TextLine*)
        );
        bzero(lines + Index(firstline), count * sizeof(TextLine*));
        if (canvas != nil) {
            Coord y = Top(line) + 1;
            int shift = count * lineheight;
            painter->Copy(
                canvas, xmin, y+shift, xmax, ymax, canvas, xmin, y
            );
            Redraw(xmin, ymax-shift+1, xmax, ymax);
        }
        Size(firstline + count, lastline);
    }
}

void TextDisplay::InsertText (int l, int i, const char* t, int c) {
    TextLine* line = Line(l, true);
    line->Insert(this, l, i, t, c);
    if (painter != nil && width != -1) {
        Coord w = line->Offset(this, 10000);
        if (w > width) {
            width = w;
            widestline = l;
        }
    }
    if (autosized) {
        Coord dw = Width() - (xmax - xmin);
        if (dw > 0) {
            xmax += dw;
            Redraw(xmax - dw + 1, ymin, xmax, ymax);
        }
    }
    if (l == caretline) {
        ShowCaret();
    }
}

void TextDisplay::DeleteText (int l, int i, int c) {
    TextLine* line = Line(l, true);
    line->Delete(this, l, i, c);
    if (painter != nil && width != -1) {
        if (l == widestline) {
            Coord w = line->Offset(this, 10000);
            if (w < width) {
                width = -1;
            }
        }
    }
    if (l == caretline) {
        ShowCaret();
    }
}

void TextDisplay::ReplaceText (int l, const char* t, int c) {
    TextLine* line = Line(l, true);
    line->Replace(this, l, t, c);
    if (painter != nil && width != -1) {
        Coord w = line->Offset(this, 10000);
        if (w > width) {
            width = w;
            widestline = l;
        } else if (l == widestline && w < width) {
            width = -1;
        }
    }
    if (autosized) {
        Coord dw = Width() - (xmax - xmin);
        if (dw > 0) {
            xmax += dw;
            Redraw(xmax - dw + 1, ymin, xmax, ymax);
        }
    }
    if (l == caretline) {
        ShowCaret();
    }
}

void TextDisplay::Style (int l1, int i1, int l2, int i2, int style) {
    for (int l = l1; l <= l2; ++l) {
        int first = (l == l1) ? i1 : -10000;
        int last = (l == l2) ? i2 : 10000;
        Line(l, true)->Style(this, l, first, last, style);
    }
    if (l1 <= caretline && l2 >= caretline) {
        ShowCaret();
    }
}

void TextDisplay::AddStyle (int l1, int i1, int l2, int i2, int style) {
    for (int l = l1; l <= l2; ++l) {
        int first = (l == l1) ? i1 : -10000;
        int last = (l == l2) ? i2 : 10000;
        Line(l, true)->AddStyle(this, l, first, last, style);
    }
    if (l1 <= caretline && l2 >= caretline) {
        ShowCaret();
    }
}

void TextDisplay::RemoveStyle (int l1, int i1, int l2, int i2, int style) {
    for (int l = l1; l <= l2; ++l) {
        int first = (l == l1) ? i1 : -10000;
        int last = (l == l2) ? i2 : 10000;
        Line(l, true)->RemoveStyle(this, l, first, last, style);
    }
    if (l1 <= caretline && l2 >= caretline) {
        ShowCaret();
    }
}

void TextDisplay::CaretStyle (int style) {
    HideCaret();
    caretstyle = style;
    ShowCaret();
}
        
void TextDisplay::Caret (int line, int index) {
    HideCaret();
    caretline = line;
    caretindex = index;
    ShowCaret();
}

void TextDisplay::HideCaret () {
    if (canvas != nil && caretline >= topline && caretline <= bottomline) {
        TextLine* l = Line(caretline, true);
        l->Draw(this, caretline, caretindex-1, caretindex);
    }
}

void TextDisplay::ShowCaret () {
    if (canvas != nil && caretline >= topline && caretline <= bottomline) {
        Coord l = Left(caretline, caretindex);
        Coord r = Right(caretline, caretindex);
        Coord b = Base(caretline);
        Coord t = Top(caretline);
        if (l >= xmin && r <= xmax) {
            switch (caretstyle) {
            case NoCaret:
                break;
            case DefaultCaret:
            case BarCaret:
                painter->FillRect(canvas, l, b, l, t);
                break;
            case UnderscoreCaret:
                painter->FillRect(canvas, l, b, r, b+1);
                break;
            case OutlineCaret:
                painter->Rect(canvas, l, b, r, t);
                break;
            }
        }
    }
}

Coord TextDisplay::Width () {
    if (width < 0) {
        if (painter != nil) {
            for (int i = firstline; i <= lastline; ++i) {
                TextLine* line = Line(i, false);
                if (line != nil) {
                    width = max(width, line->Offset(this, 10000));
                }
            }
        }
    }
    return width;
}

Coord TextDisplay::Height () {
    return (lastline-firstline + 1) * lineheight;
}

int TextDisplay::LineNumber (Coord y) {
    Coord dy = ymax + y0 - y;
    if (dy >= 0) {
        return dy / lineheight;
    } else {
        return - ((-1 - dy) / lineheight + 1);
    }
}

int TextDisplay::LineIndex (int line, Coord x, boolean between) {
    TextLine* l = Line(line, false);
    if (l == nil) {
        return 0;
    } else {
        return l->Index(this, x - (xmin + x0), between);
    }
}

Coord TextDisplay::Base (int line) {
    return ymax + y0 - line * lineheight - (lineheight - 1);
}

Coord TextDisplay::Top (int line) {
    return ymax + y0 - line * lineheight;
}

Coord TextDisplay::Left (int line, int index) {
    TextLine* l = Line(line, false);
    if (l == nil) {
        return xmin + x0;
    } else {
        return xmin + x0 + l->Offset(this, index);
    }
}

Coord TextDisplay::Right (int line, int index) {
    TextLine* l = Line(line, false);
    if (l == nil) {
        return xmin + x0;
    } else {
        return xmin + x0 + l->Offset(this, index+1) - 1;
    }
}

TextLine::TextLine () {
    size = 0;
    text = nil;
    attr = nil;
    prefix = 0;
    postfix = 0;
    lastchar = -1;
    Size(0);
}

TextLine::~TextLine () {
    delete text;
    delete attr;
}

Coord TextLine::Offset (TextDisplay* display, int index) {
    if (display->painter != nil) {
        Font* f = display->painter->GetFont();
        index = max(0, min(index, lastchar + 1));
        int w = 0;
        int i = 0;
        int cw;
        while (i < index) {
            if (text[i] == '\t') {
                if (display->tabwidth > 0) {
                    cw = display->tabwidth - w % display->tabwidth;
                } else {
                    cw = 0;
                }
            } else {
                cw = f->Width(text+i, 1);
            }
            w += cw;
            ++i;
        }
        return w;
    } else {
        return 0;
    }
}

int TextLine::Index (TextDisplay* display, Coord x, boolean between) {
    if (x < 0) {
        if (!between) {
            return -1;
        } else {
            return 0;
        }
    } else if (display->painter != nil) {
        Font* f = display->painter->GetFont();
        int i = 0;
        int w = 0;
        int cw = 0;
        while (i <= lastchar) {
            if (text[i] == '\t') {
                if (display->tabwidth > 0) {
                    cw = display->tabwidth - w % display->tabwidth;
                } else {
                    cw = 0;
                }
            } else {
                cw = f->Width(text+i, 1);
            }
            w += cw;
            if (w > x) {
                break;
            }
            ++i;
        }
        if (between && i <= lastchar && x > w - cw/2 || !between && x > w) {
            return i+1;
        } else {
            return i;
        }
    } else {
        return 0;
    }
}

void TextLine::Size (int last) {
    if (last >= size) {
        int newsize = last<28 ? 28 : last<124 ? 124 : last<1020 ? 1020 : last;
        char* newtext = new char[newsize];
        bzero(newtext, newsize);
        bcopy(text, newtext, size);
        delete text;
        text = newtext;
        char* newattr = new char[newsize];
        bzero(newattr, newsize);
        bcopy(attr, newattr, size);
        delete attr;
        attr = newattr;
        size = newsize;
    }
}

void TextLine::Style (
    TextDisplay* display, int line, int first, int last, int style
) {
    if (first < 0) {
        prefix = style;
    }
    if (last > lastchar) {
        postfix = style;
    }
    int f = max(first, 0);
    int l = min(last, lastchar);
    for (int i = f; i <= l; ++i) {
        attr[i] = style;
    }
    Draw(display, line, first, last);
}

void TextLine::AddStyle (
    TextDisplay* display, int line, int first, int last, int style
) {
    if (first < 0) {
        prefix = prefix | style;
    }
    if (last > lastchar) {
        postfix = postfix | style;
    }
    int f = max(first, 0);
    int l = min(last, lastchar);
    for (int i = f; i <= l; ++i) {
        attr[i] = attr[i] | style;
    }
    Draw(display, line, first, last);
}

void TextLine::RemoveStyle (
    TextDisplay* display, int line, int first, int last, int st
) {
    if (first < 0) {
        prefix = prefix & ~st;
    }
    if (last > lastchar) {
        postfix = postfix & ~st;
    }
    int f = max(first, 0);
    int l = min(last, lastchar);
    for (int i = f; i <= l; ++i) {
        attr[i] = attr[i] & ~st;
    }
    Draw(display, line, first, last);
}

void TextLine::Insert (
    TextDisplay* display, int line, int index, const char* s, int count
) {
    Coord left, right;
    int shift;
    index = max(0, index);
    Size(max(index, lastchar) + count);
    int src = index;
    int dst = index + count;
    int len = max(0, lastchar - index + 1);
    lastchar += count;
    if (display->canvas != nil) {
        left = display->Left(line, index);
        right = display->Right(line, lastchar+1);
    }
    bcopy(text + src, text + dst, len);
    bcopy(attr + src, attr + dst, len);
    bcopy(s, text + src, count);
    bzero(attr + src, count);
    if (display->canvas != nil) {
        Font* f = display->painter->GetFont();
        if (strchr(text+index, '\t') != nil) {
            Draw(display, line, index, lastchar+1);
        } else {
            shift = display->Left(line, index+count) - left;
            right = min(right, display->xmax - shift);
            if (left <= right) {
                Coord bottom = display->Base(line);
                Coord top = bottom + f->Height() - 1;
                display->painter->Copy(
                    display->canvas, left, bottom, right, top,
                    display->canvas, left+shift, bottom
                );
            }
            Draw(display, line, index, index+count-1);
        }
    }
}

void TextLine::Delete (
    TextDisplay* display, int line, int index, int count
) {
    Coord left, right;
    int shift;
    Size(max(lastchar, index));
    count = max(0, min(count, lastchar-index+1));
    int src = index + count;
    int dst = index;
    int len = lastchar - (index + count) + 1;
    if (display->canvas != nil) {
        left = display->Left(line, index + count);
        right = min(display->Right(line, lastchar + 1), display->xmax);
    }
    bcopy(text + src, text + dst, len);
    bcopy(attr + src, attr + dst, len);
    bzero(text + lastchar + 1 - count, count);
    bzero(attr + lastchar + 1 - count, count);
    lastchar -= count;
    if (display->canvas != nil) {
        if (strchr(text+index, '\t') != nil) {
            Draw(display, line, index, lastchar+1);
        } else {
            shift = left - display->Left(line, index);
            Coord bottom = display->Base(line);
            Coord top = display->Top(line);
            if (left <= right) {
                display->painter->Copy(
                    display->canvas, left, bottom, right, top,
                    display->canvas, left-shift, bottom
                );
            }
            if (shift > 0) {
                int last = display->LineIndex(line, right-shift+1, false);
                Draw(display, line, last, lastchar + 1);
            }
        }
    }
}

void TextLine::Replace (
    TextDisplay* display, int line, const char* t, int c
) {
    delete text;
    text = nil;
    delete attr;
    attr = nil;
    size = 0;
    Size(c);
    prefix = 0;
    postfix = 0;
    lastchar = c - 1;
    bcopy(t, text, c);
    bzero(attr, c);
    Draw(display, line, -1, lastchar+1);
}

void TextLine::Draw (
    TextDisplay* display, int line, int first, int last
) {
    if (display->canvas != nil) {
        Font* f = display->painter->GetFont();
        Coord bottom = display->Base(line);
        Coord top = bottom + f->Height() - 1;
        if (line < display->topline || line > display->bottomline) {
            if (top >= display->ymin && bottom <= display->ymax) {
                display->painter->ClearRect(
                    display->canvas,
                    display->xmin, max(display->ymin, bottom),
                    display->xmax, min(display->ymax, top)
                );
            }
        } else {
            int start = max(
                display->LineIndex(line, display->xmin-1, false) + 1,
                max(0, first)
            );
            int finish = min(
                display->LineIndex(line, display->xmax+1, false) - 1,
                min(lastchar, last)
            );
            Coord left = display->Left(line, start);
            Coord right = display->Right(line, finish);
            if (start > first && left > display->xmin) {
                if ((start>0 ? attr[start-1] : prefix) & Reversed) {
                    display->painter->FillRect(
                        display->canvas, display->xmin, bottom, left-1, top
                    );
                } else {
                    display->painter->ClearRect(
                        display->canvas, display->xmin, bottom, left-1, top
                    );
                }
            }
            int done = start;
            display->painter->MoveTo(left, bottom);
            for (int i = start; i <= finish+1; ++i) {
                if (i==finish+1 || attr[i]!=attr[done] || text[i]=='\t') {
                    if (done != i && text[done] == '\t') {
                        Coord l, r, y;
                        display->painter->GetPosition(l, y);
                        r = display->Right(line, done);
                        if (attr[done] & Reversed) {
                            display->painter->FillRect(
                                display->canvas, l, bottom, r, top
                            );
                        } else {
                            display->painter->ClearRect(
                                display->canvas, l, bottom, r, top
                            );
                        }
                        display->painter->MoveTo(r+1, bottom);
                        ++done;
                    }
                    if (done != i) {
                        display->painter->SetStyle(attr[done]);
                        display->painter->Text(
                            display->canvas, text + done, i - done
                        );
                        done = i;
                    }
                }
            }
            display->painter->SetStyle(Plain);
            if (finish < last && right < display->xmax) {
                if ((finish<lastchar ? attr[finish+1] : postfix) & Reversed) {
                    display->painter->FillRect(
                        display->canvas, right+1, bottom, display->xmax, top
                    );
                } else {
                    display->painter->ClearRect(
                        display->canvas, right+1, bottom, display->xmax, top
                    );
                }
            }
        }
    }
}

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
 * Hash table class for storing painters.  On X10, SetColor, SetFont, etc.
 * operations require a client-server ipc round-trip and hence are
 * inefficient.  To avoid this, we keep a hash table of painters that have
 * preset colors, patterns, etc.  When it's time to Draw a graphic, we
 * use a painter whose graphics state matches the graphic's own.  If none
 * exists, then we create a new painter, set its graphics state to match
 * the graphic's, and use it to draw the graphic.  The new painter is stored
 * in the hash table for future use.
 */

#ifndef hash_h
#define hash_h

class Entry;
class Graphic;
class Painter;

class HashTable {
public:
    HashTable(int);
    ~HashTable();

    void Insert(void* tag, void* value);
    void Remove(void* tag);
    void* Find(void* tag);
protected:
    virtual boolean Match(void* target, void* entry);
    virtual int Hash(void*);
protected:
    Entry** entries;
    int count;
};

class GraphicToPainter : public HashTable {
public:
    GraphicToPainter(int);
    ~GraphicToPainter();

    void Insert(Graphic*, Painter*&);
    void Remove(Graphic* g) { HashTable::Remove(g); }
    Painter* Find(Graphic* g);
    void Clip(Canvas*, Coord, Coord, Coord, Coord);
    void NoClip();
private:
    boolean Match(void*, void*);
    int Hash(void*);
private:
    class Canvas* canvas;
    class BoxObj* box;
};

#endif

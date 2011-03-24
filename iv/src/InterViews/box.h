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
 * Boxes are used to compose side-by-side.
 */

#ifndef box_h
#define box_h

#include <InterViews/scene.h>

class BoxElement;
class BoxCanonical;

class Box : public Scene {
public:
    void Align(Alignment);
    void Draw();
    void GetComponents(Interactor**, int, Interactor**&, int&);
    void Resize();
protected:
    Alignment align;

    Box();
    ~Box();

    virtual void ComputeShape(Shape*);
    virtual void GetActual(int& major, int& minor);
    virtual void GetCanonical(Shape*, BoxCanonical&);
    BoxElement* Head();
    virtual void PlaceElement(
	Interactor*, Coord major, int len, int thick, int minor
    );
    virtual void Reconfig();
private:
    int nelements;
    BoxElement* head;
    BoxElement* tail;

    void DoInsert(Interactor*, boolean, Coord& x, Coord& y);
    void DoChange(Interactor*);
    void DoRemove(Interactor*);
};

class HBox : public Box {
public:
    HBox();
    HBox(Interactor*);
    HBox(Interactor*, Interactor*);
    HBox(Interactor*, Interactor*, Interactor*);
    HBox(Interactor*, Interactor*, Interactor*, Interactor*);
    HBox(Interactor*, Interactor*, Interactor*, Interactor*, Interactor*);
    HBox(
	Interactor*, Interactor*, Interactor*, Interactor*, Interactor*,
	Interactor*
    );
    HBox(
	Interactor*, Interactor*, Interactor*, Interactor*, Interactor*,
	Interactor*, Interactor*
    );
protected:
    void ComputeShape(Shape*);
    void GetActual(int& major, int& minor);
    void GetCanonical(Shape*, BoxCanonical&);
    void PlaceElement(Interactor*, Coord, int, int, int);
private:
    void Init();
};

class VBox : public Box {
public:
    VBox();
    VBox(Interactor*);
    VBox(Interactor*, Interactor*);
    VBox(Interactor*, Interactor*, Interactor*);
    VBox(Interactor*, Interactor*, Interactor*, Interactor*);
    VBox(Interactor*, Interactor*, Interactor*, Interactor*, Interactor*);
    VBox(
	Interactor*, Interactor*, Interactor*, Interactor*, Interactor*,
	Interactor*
    );
    VBox(
	Interactor*, Interactor*, Interactor*, Interactor*, Interactor*,
	Interactor*, Interactor*
    );
protected:
    void ComputeShape(Shape*);
    void GetActual(int& major, int& minor);
    void GetCanonical(Shape*, BoxCanonical&);
    void PlaceElement(Interactor*, Coord, int, int, int);
private:
    void Init();
};

#endif

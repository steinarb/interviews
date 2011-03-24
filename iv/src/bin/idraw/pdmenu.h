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

// $Header: pdmenu.h,v 1.10 89/10/09 14:49:16 linton Exp $
// declares pulldown menu classes.

#ifndef pdmenu_h
#define pdmenu_h

#include "highlighter.h"

// Declare imported and used-before-defined types.

class PullDownMenuActivator;
class PullDownMenuCommand;

// A PullDownMenuBar displays several activators and coordinates which
// activator will open its menu.

class PullDownMenuBar : public HighlighterParent {
public:

    PullDownMenuBar();
    ~PullDownMenuBar();

    void Enter(PullDownMenuActivator*);
    boolean Contains(Interactor*);

    boolean MenuActive();
    boolean MenuShouldActivate(PullDownMenuActivator*);

    void MenuActivate(PullDownMenuActivator*);
    void MenuDeactivate();

protected:

    void GrowActivators();

    PullDownMenuActivator* cur;	// stores currently active activator
    int sizeactivators;		// stores current size of dynamic array
    int numactivators;		// stores number of activators in array
    PullDownMenuActivator**
	activators;		// stores bar's interior activators

};

// A PullDownMenuActivator displays a text label and opens a menu when
// you activate it.

class PullDownMenuActivator : public Highlighter {
public:

    PullDownMenuActivator(PullDownMenuBar*, const char*);
    ~PullDownMenuActivator();

    void SetMenu(Scene*);
    void Handle(Event&);

    void Enter(PullDownMenuCommand*);
    boolean Contains(Interactor*);

    void Open();
    void Close();

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();
    void GrowCommands();

    PullDownMenuBar* bar;	// stores bar containing this activator
    char* name;			// stores activator's text label
    Scene* menu;		// stores menu to be opened when activated
    int sizecommands;		// stores current size of dynamic array
    int numcommands;		// stores number of commands in array
    PullDownMenuCommand**
	commands;		// stores activator's interior commands

    Coord name_x, name_y;	// stores position at which to display name

};

// A PullDownMenuCommand displays a text label and executes a command.

class PullDownMenuCommand : public Highlighter {
public:

    PullDownMenuCommand(PullDownMenuActivator*, const char*, const char*);
    ~PullDownMenuCommand();

    void Handle(Event&);

    virtual void Execute(Event&);

protected:

    void Reconfig();
    void Redraw(Coord, Coord, Coord, Coord);
    void Resize();

    PullDownMenuActivator*
	activator;		// stores activator which this cmd belongs to
    char* name;			// stores command's text label
    char* key;			// stores label of key which selects this cmd

    Coord name_x, name_y;	// stores position at which to display name
    Coord key_x, key_y;		// stores position at which to display key

};

// A PullDownMenuDivider displays a horizontal line extending the full
// width of the menu, dividing it into two submenus.

class PullDownMenuDivider : public PullDownMenuCommand {
public:

    PullDownMenuDivider();

protected:

    void Redraw(Coord, Coord, Coord, Coord);

};

#endif

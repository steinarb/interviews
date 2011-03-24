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
 * a Dialog for browsing fonts
 */

#include "fontbrowser.h"

#include "cyclebutton.h"
#include "fontsample.h"
#include "stringbutton.h"

#include <InterViews/box.h>
#include <InterViews/button.h>
#include <InterViews/deck.h>
#include <InterViews/frame.h>
#include <InterViews/glue.h>
#include <InterViews/message.h>
#include <InterViews/sensor.h>
#include <InterViews/streditor.h>
#include <InterViews/tray.h>
#include <stdio.h>
#include <string.h>

class ButtonChoice {
public:
    const char* label;
    void* value;
};

static ButtonChoice ModeChoices[] = {
    { "family", "family" },
    { "name", "name" },
    { nil }
};

static ButtonChoice FamilyChoices[] = {
    { "helvetica", "helvetica" },
    { "times", "times" },
    { "", "a longish font family name" },
    { nil }
};

static ButtonChoice WeightChoices[] = {
    { "medium", "medium" },
    { "bold", "bold" },
    { "", "a weight" },
    { nil }
};

static ButtonChoice WidthChoices[] = {
    { "normal", "normal" },
    { "", "a width name" },
    { nil }
};

static ButtonChoice SlantChoices[] = {
    { "roman", "r" },
    { "oblique", "o" },
    { "italic", "i" },
    { nil }
};

static CycleButtonChoice PointChoices[] = {
    { "24 point", "24" },
    { "18 point", "18" },
    { "14 point", "14" },
    { "12 point", "12" },
    { "10 point", "10" },
    { "8 point", "8" },
    { nil }
};

static Interactor* ButtonBox (
    const char* name, ButtonState* state, ButtonChoice* choices
) {
    HBox* box = new HBox();
    box->Align(Center);
    for (int i = 0; choices[i].label != nil; ++i) {
        if (strlen(choices[i].label) == 0) {
            box->Insert(
                new StringButton(name, state, (char*)choices[i].value)
            );
        } else {
            box->Insert(
                new RadioButton(name, choices[i].label, state, choices[i].value)
            );
        }
        box->Insert(new HGlue(round(0.05*inch), round(0.05*inch), 0));
    }
    return box;
}

FontBrowser::FontBrowser (ButtonState* s, const char* samp) : Dialog(s, nil) {
    Init(samp);
}

FontBrowser::FontBrowser (
    const char* name, ButtonState* s, const char* samp
) : Dialog(s, nil) {
    SetInstance(name);
    Init(samp);
}

FontBrowser::~FontBrowser () {
    Unref(mode);
    Unref(family);
    Unref(weight);
    Unref(slant);
    Unref(width);
    Unref(point);
}

void FontBrowser::Init (const char* samp) {
    SetClassName("FontBrowser");

    mode = new ButtonState(ModeChoices[0].value);
    family = new ButtonState(FamilyChoices[0].value);
    weight = new ButtonState(WeightChoices[0].value);
    width = new ButtonState(WidthChoices[0].value);
    slant = new ButtonState(SlantChoices[0].value);
    point = new ButtonState(PointChoices[0].value);

    sample = new FontSample(samp);
    fontname = new StringEditor(
        (ButtonState*)nil,
        "a very long font name with space for lots of characters", "\r"
    );

    Tray* familymode = new Tray();
    Interactor* familylabel = new Message("label", "Family");
    Interactor* familychoices = ButtonBox("choice", family, FamilyChoices);
    Interactor* weightlabel = new Message("label", "Weight");
    Interactor* weightchoices = ButtonBox("choice", weight, WeightChoices);
    Interactor* widthlabel = new Message("label", "Width");
    Interactor* widthchoices = ButtonBox("choice", width, WidthChoices);
    Interactor* slantlabel = new Message("label", "Slant");
    Interactor* slantchoices = ButtonBox("choice", slant, SlantChoices);
    Interactor* pointlabel = new Message("label", "Size");
    Interactor* pointchoices = new CycleButton("choice", point, PointChoices);
    familymode->VBox(
        familymode,
        familychoices,
        new VGlue(round(0.05*inch), round(0.05*inch), 0),
        weightchoices,
        new VGlue(round(0.05*inch), round(0.05*inch), 0),
        widthchoices
    );
    familymode->VBox(
        widthchoices,
        new VGlue(round(0.05*inch), round(0.05*inch), 0),
        slantchoices,
        new VGlue(round(0.05*inch), round(0.05*inch), 0),
        pointchoices,
        familymode
    );
    familymode->HBox(
        familymode,
        familylabel,
        new HGlue(round(0.2*inch), round(0.1*inch), 0),
        familychoices,
        new HGlue(0, 0, hfil),
        familymode
    );
    familymode->Align(VertCenter, familylabel, familychoices);
    familymode->HBox(
        familymode,
        weightlabel,
        new HGlue(round(0.2*inch), round(0.1*inch), 0),
        weightchoices,
        new HGlue(0, 0, hfil),
        familymode
    );
    familymode->Align(VertCenter, weightlabel, weightchoices);
    familymode->HBox(
        familymode,
        widthlabel,
        new HGlue(round(0.2*inch), round(0.1*inch), 0),
        widthchoices,
        new HGlue(0, 0, hfil),
        familymode
    );
    familymode->Align(VertCenter, widthlabel, widthchoices);
    familymode->HBox(
        familymode,
        slantlabel,
        new HGlue(round(0.2*inch), round(0.1*inch), 0),
        slantchoices,
        new HGlue(0, 0, hfil),
        familymode
    );
    familymode->Align(VertCenter, slantlabel, slantchoices);
    familymode->HBox(
        familymode,
        pointlabel,
        new HGlue(round(0.2*inch), round(0.1*inch), 0),
        pointchoices,
        new HGlue(0, 0, hfil),
        familymode
    );
    familymode->Align(VertCenter, pointlabel, pointchoices);
    familymode->Align(Left,
        familylabel, weightlabel, widthlabel, slantlabel, pointlabel
    );
    familymode->Align(Left,
        familychoices, weightchoices, widthchoices, slantchoices, pointchoices
    );
    Interactor* namemode = new VBox(
        new Frame(new MarginFrame(fontname, 2, 2)),
        new VGlue(0, 0, vfil)
    );

    modeoptions = new Deck();
    modeoptions->Insert(familymode);
    modeoptions->Insert(namemode);

    Insert(
        new MarginFrame(
            new VBox(
                new ShadowFrame(new MarginFrame(sample, 2, 2), 2, 2),
                new VGlue(round(0.3*inch), round(0.2*inch), 0),
                new HBox(
                    new VBox(
                        new HBox(
                            new Message("label", "Browse by"),
                            new HGlue(round(0.1*inch), 0, 0),
                            ButtonBox("choice", mode, ModeChoices)
                        ),
                        new VGlue(round(0.1*inch), round(0.05*inch), 0),
                        new HBox(
                            new HGlue(round(0.3*inch), round(0.2*inch), 0),
                            modeoptions
                        )
                    ),
                    new HGlue(round(0.3*inch), round(0.2*inch), hfil),
                    new PushButton("Quit", state, true)
                )
            ),
            round(0.5*inch), round(0.4*inch), round(2.0*inch),
            round(0.3*inch), round(0.2*inch), round(1.0*inch)
        )
    );

    UpdateFontname();
    input = new Sensor;
    input->Catch(KeyEvent);
}

const char* FontBrowser::Fontname () {
    return fontname->Text();
}

boolean FontBrowser::Accept () {
    Event e;
    int v;
    void* m;

    state->SetValue(0);
    do {
        Read(e);
        e.target->Handle(e);
        mode->GetValue(m);
        if (strcmp((char*)m, "family") == 0) {
            modeoptions->FlipTo(1);
        } else if (strcmp((char*)m, "name") == 0) {
            modeoptions->FlipTo(2);
        }
        UpdateFontname();
        state->GetValue(v);
    } while (v == 0 && e.target != nil);
    return v == 1 || e.target == nil;
}

void FontBrowser::UpdateFontname () {
    void* m;
    mode->GetValue(m);
    if (strcmp((char*)m, "family") == 0) {
        void* v;
        family->GetValue(v);
        const char* fam = (char*)v;
        weight->GetValue(v);
        const char* wgt = (char*)v;
        slant->GetValue(v);
        const char* sl = (char*)v;
        width->GetValue(v);
        const char* wid = (char*)v;
        point->GetValue(v);
        const char* pnt = (char*)v;
        char name[1000];
        sprintf(name,
            "*-*-%s-%s-%s-%s-*-*-%s0-*-*-*-*-iso8859-1",
            fam, wgt, sl, wid, pnt
        );
        fontname->Message(name);
    }
    sample->ShowFont(Fontname());
}

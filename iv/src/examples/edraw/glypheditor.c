/*
 * glypheditor implementation
 */

#include <InterViews/action.h>
#include <InterViews/adjust.h>
#include <InterViews/box.h>
#include <InterViews/canvas.h>
#include <InterViews/color.h>
#include <InterViews/debug.h>
#include <InterViews/label.h>
#include <InterViews/layout.h>
#include <InterViews/session.h>
#include <InterViews/style.h>
#include <InterViews/telltale.h>
#include <InterViews/transformer.h>
#include <IV-look/button.h>
#include <IV-look/dialogs.h>
#include <IV-look/field.h>
#include <IV-look/fchooser.h>
#include <IV-look/menu.h>
#include <IV-look/kit.h>
#include <OS/file.h>
#include <OS/string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysent.h>

#include "figure.h"
#include "glypheditor.h"
#include "glyphviewer.h"
#include "pacekeeper.h"
#include "idraw.h"

static const char* tempfile = "/tmp/edraw";

class Displayer : public FieldEditor {
public:
    Displayer(
        const String& sample, WidgetKit*, Style*, FieldEditorAction* = nil
    );
    virtual void press(const Event&);
};

Displayer::Displayer(
    const String& sample, WidgetKit* w, Style* s, FieldEditorAction* fa
) : FieldEditor(sample, w, s, fa) {}

void Displayer::press (const Event&) {}

class BoundedValue : public Adjustable {
protected:
    BoundedValue();
public:
    BoundedValue(Coord lower, Coord upper);
    virtual ~BoundedValue();

    virtual void lower_bound(Coord);
    virtual void upper_bound(Coord);
    virtual void current_value(Coord);
    virtual void scroll_incr(Coord);
    virtual void page_incr(Coord);

    virtual Coord lower(DimensionName) const;
    virtual Coord upper(DimensionName) const;
    virtual Coord length(DimensionName) const;
    virtual Coord cur_lower(DimensionName) const;
    virtual Coord cur_upper(DimensionName) const;
    virtual Coord cur_length(DimensionName) const;

    virtual void scroll_to(DimensionName, Coord position);
    virtual void scroll_forward(DimensionName);
    virtual void scroll_backward(DimensionName);
    virtual void page_forward(DimensionName);
    virtual void page_backward(DimensionName);
private:
    Coord curvalue_;
    Coord lower_;
    Coord span_;
    Coord scroll_incr_;
    Coord page_incr_;
};

BoundedValue::BoundedValue() {
    scroll_incr_ = 0.0;
    page_incr_ = 0.0;
}

BoundedValue::BoundedValue(Coord lower, Coord upper) {
    lower_ = lower;
    span_ = upper - lower;
    scroll_incr_ = span_ * 0.04;
    page_incr_ = span_ * 0.4;
    curvalue_ = (lower + upper) * 0.5;
}

BoundedValue::~BoundedValue() { }

void BoundedValue::lower_bound(Coord c) { lower_ = c; }
void BoundedValue::upper_bound(Coord c) { span_ = c - lower_; }

void BoundedValue::current_value(Coord value) {
    curvalue_ = value;
    constrain(Dimension_X, curvalue_);
    notify(Dimension_X);
    notify(Dimension_Y);
}

void BoundedValue::scroll_incr(Coord c) { scroll_incr_ = c; }
void BoundedValue::page_incr(Coord c) { page_incr_ = c; }

Coord BoundedValue::lower(DimensionName) const { return lower_; }
Coord BoundedValue::upper(DimensionName) const { return lower_ + span_; }
Coord BoundedValue::length(DimensionName) const { return span_; }
Coord BoundedValue::cur_lower(DimensionName) const { return curvalue_; }
Coord BoundedValue::cur_upper(DimensionName) const { return curvalue_; }
Coord BoundedValue::cur_length(DimensionName) const { return 0; }

void BoundedValue::scroll_to(DimensionName d, Coord position) {
    Coord p = position;
    constrain(d, p);
    if (p != curvalue_) {
        curvalue_ = p;
        notify(Dimension_X);
        notify(Dimension_Y);
    }
}

void BoundedValue::scroll_forward(DimensionName d) {
    scroll_to(d, curvalue_ + scroll_incr_);
}

void BoundedValue::scroll_backward(DimensionName d) {
    scroll_to(d, curvalue_ - scroll_incr_);
}

void BoundedValue::page_forward(DimensionName d) {
    scroll_to(d, curvalue_ + page_incr_);
}

void BoundedValue::page_backward(DimensionName d) {
    scroll_to(d, curvalue_ - page_incr_);
}

class Valuator : public MonoGlyph, public Observer {
public:
    Valuator(BoundedValue*, Style*);
    virtual ~Valuator();

    virtual InputHandler* focusable() const;

    virtual void update(Observable*);
    virtual void disconnect(Observable*);
private:
    BoundedValue* bvalue_;
    FieldEditor* editor_;

    void accept_editor(FieldEditor*);
    void cancel_editor(FieldEditor*);
};

declareFieldEditorCallback(Valuator)
implementFieldEditorCallback(Valuator)

Valuator::Valuator(BoundedValue* bv, Style* style) : MonoGlyph(nil) {
    Style* s = new Style(style);
    s->alias("Valuator");
    bvalue_ = bv;
    bv->attach(Dimension_X, this);
    editor_ = new Displayer(
        String("9999"), WidgetKit::instance(), s,
        new FieldEditorCallback(Valuator)(
            this, &Valuator::accept_editor, &Valuator::cancel_editor
        )
    );
    editor_->field("0.50");
    body(editor_);
    update(bv->observable(Dimension_X));
}

Valuator::~Valuator() {
    if (bvalue_ != nil) {
        bvalue_->detach(Dimension_X, this);
    }
}

InputHandler* Valuator::focusable() const {
    return editor_;
}

float frame_dist;

void Valuator::update(Observable*) {
    Coord v = bvalue_->cur_lower(Dimension_X);
    char buf[20];
    sprintf(buf, "%.2f", v);
    editor_->field(buf);
    frame_dist = v*300.0 + 2.0;
}

void Valuator::disconnect(Observable*) {
    bvalue_ = nil;
}

void Valuator::accept_editor(FieldEditor*) {
    Coord v;
    const String& value = *editor_->text();
    if (value.convert(v)) {
        bvalue_->current_value(v);
    }
}

void Valuator::cancel_editor(FieldEditor*) {
    update(bvalue_->observable(Dimension_X));
}

/******************************************************/
class CatalogButton : public Button {
public:
    CatalogButton(Glyph*, Style*, TelltaleState*, Action*);
    virtual ~CatalogButton();
    virtual void enter();
    virtual void leave();
};

CatalogButton::CatalogButton(
    Glyph* g, Style* s, TelltaleState* t, Action* a
) : Button(g, s, t, a) {}

CatalogButton::~CatalogButton () {}

void CatalogButton::enter () {}
void CatalogButton::leave () {}

/******************************************************/
class ToolCallback : public Action {
public:
    ToolCallback(GlyphViewer*, unsigned int);
    virtual void execute();
private:
    GlyphViewer* _gv;
    unsigned _type;
};

ToolCallback::ToolCallback (
    GlyphViewer* gv, unsigned int t
) {
    _gv = gv;
    _type = t;
}

void ToolCallback::execute () {
    _gv->tool().tool(_type);
}

/******************************************************/
class PolicyCallback : public Action {
public:
    PolicyCallback(GlyphViewer*, unsigned int);
    virtual void execute();
private:
    GlyphViewer* _gv;
    unsigned int _type;
};

PolicyCallback::PolicyCallback (
    GlyphViewer* gv, unsigned int t
) {
    _gv = gv;
    _type = t;
}

void PolicyCallback::execute () {
    _gv->set_policy(_type);
}

/******************************************************/
GlyphEditor::GlyphEditor () {
    body(interior());
}

Glyph* GlyphEditor::interior () {
    BoundedValue* bv = new BoundedValue(0.0, 1.0);
    bv->current_value(0.5);
    bv->scroll_incr(0.01);
    bv->page_incr(0.1);

    LayoutKit* layout = LayoutKit::instance();
    WidgetKit* widget = WidgetKit::instance();

    Style* s = new Style;
    s->attribute("foreground", "black");
    s->attribute("font", "-*-times-bold-r-normal-*-14-*-*-*-*-*-*-*");
    widget->push_style(s);

    Glyph* label1 = widget->label("Policies");
    Glyph* label2 = widget->label("Tools");

    widget->pop_style();

    _gviewer = new GlyphViewer(600, 500);
    return widget->inset_frame(
        layout->vbox(
            menus(),
            layout->hbox(
                layout->vbox(
                    widget->inset_frame(
                        _gviewer
                    )
                ),
                widget->outset_frame(
                    layout->vbox(
                        layout->vglue(5, fil/2.0, 0),
                        layout->hmargin(
                            label1,
                            1.0, fil/10, 0, 1.0, fil/10, 0
                        ),
                        pacekeepers(),
                        layout->vglue(10, 0, 0),
                        layout->hmargin(
                            label2,
                            1.0, fil/10, 0, 1.0, fil/10, 0
                        ),
                        buttons(),
                        layout->vglue(10, 0, 0),
                        layout->vglue(20, fil, 0)
                    )
                )
            )
        )
    );
}

class Teller : public TelltaleState {
public:
    Teller(const TelltaleFlags, Action*);
    virtual void set(const TelltaleFlags, boolean);
protected:
    Action* _action;
};

Teller::Teller(const TelltaleFlags f, Action* a) : TelltaleState (f) {
    _action = a;
}

void Teller::set (const TelltaleFlags f, boolean flag) {
    if (flag && (f == is_active)) {
        TelltaleState::set(is_active | is_chosen, flag);
        if (_action != nil) {
            _action->execute();
        }
    } else if (!flag && f == is_chosen) {
        TelltaleState::set(is_active | is_chosen, flag);

    } else if (!flag && f == is_active) {

    } else if (flag && f == is_chosen) {

    } else {
        TelltaleState::set(f, flag);
    }
}

Button* GlyphEditor::make_tool (
    TelltaleGroup* teller, const char* label, unsigned int tool_id
) {
    WidgetKit* widget = WidgetKit::instance();
    LayoutKit* layout = LayoutKit::instance();
    TelltaleFlags f = TelltaleState::is_enabled | TelltaleState::is_choosable;

    widget->begin_style("PushButton", "Button");
    Action* a = new ToolCallback(
        _gviewer, tool_id
    );
    TelltaleState* ts = new Teller(f, a);
    ts->join(teller);
    Button* tool = new CatalogButton(
        widget->push_button_look(
            layout->hmargin(
                widget->label(label), 1.0, fil/10, 0, 1.0, fil/10, 0
            ), ts
        ), 
        widget->style(), ts, nil
    );
    widget->end_style();
    return tool;
}

Button* GlyphEditor::make_policy (
    TelltaleGroup* teller, const char* label, unsigned int policy
) {
    WidgetKit* widget = WidgetKit::instance();
    LayoutKit* layout = LayoutKit::instance();
    TelltaleFlags f = TelltaleState::is_enabled | TelltaleState::is_choosable;

    widget->begin_style("PushButton", "Button");
    Action* a = new PolicyCallback(
        _gviewer, policy
    );
    TelltaleState* ts = new Teller(f, a);
    ts->join(teller);
    Button* tool = new CatalogButton(
        widget->push_button_look(
            layout->hmargin(
                widget->label(label), 1.0, fil/10, 0, 1.0, fil/10, 0
            ), ts
        ), 
        widget->style(), ts, nil
    );
    widget->end_style();
    return tool;
}

Glyph* GlyphEditor::pacekeepers () {
    LayoutKit* layout = LayoutKit::instance();
    TelltaleGroup* teller = new TelltaleGroup;

    Button* nopk = make_policy(teller, "None", PaceKeeper::NoPK);
    Button* edriven = make_policy(teller, "Event Driven", PaceKeeper::EDriven);
    Button* damposc = make_policy(teller, "Damped Osc", PaceKeeper::DampedOsc);
    Button* pc = make_policy(teller, "Penalty/Credit", PaceKeeper::PC);
    Button* coh = make_policy(teller, "Coherence", PaceKeeper::Coh);

    coh->state()->set(TelltaleState::is_active, true);
    _gviewer->set_policy(PaceKeeper::Coh);
    
    return layout->vbox(
        nopk, edriven, damposc, pc, coh
    );
}

Glyph* GlyphEditor::buttons () {
    LayoutKit* layout = LayoutKit::instance();
    TelltaleGroup* teller = new TelltaleGroup;

    Button* move = make_tool(teller, "Move", Tool::move);
    Button* scale = make_tool(teller, "Scale", Tool::scale);
    Button* rotate = make_tool(teller, "Rotate", Tool::rotate);

    move->state()->set(TelltaleState::is_active, true);
    
    return layout->vbox(
        move, scale, rotate 
    );
}

void GlyphEditor::new_session () {
    _gviewer->setrootgr(new PolyGraphic);
}

void GlyphEditor::allocate (Canvas* c, const Allocation& a, Extension& ext) {
    _w = c->window();
    MonoGlyph::allocate(c, a, ext);
}

class Importer : public Action {
public:
    Importer(GlyphEditor* ed);
    virtual void execute();
protected:
    GlyphEditor* _ed;
    static FileChooser* _chooser;
};

FileChooser* Importer::_chooser;

Importer::Importer (GlyphEditor* ed) { _ed = ed; }

void Importer::execute () {
    Style* style;
    boolean resetcaption = false;
    if (_chooser == nil) {
        style = new Style(Session::instance()->style());
        _chooser = DialogKit::instance()->file_chooser(".", style);
        Resource::ref(_chooser);
        char buf[256];
        sprintf(buf, "Select an idraw drawing to import:");
        style->attribute("caption", "");
        style->attribute("subcaption", buf);
    } else {
        style = _chooser->style();
    }
    while (_chooser->post_for(_ed->window())) {
        const String* s = _chooser->selected();
        InputFile* f = InputFile::open(*s);
        if (f != nil) {
            Graphic* gr = IdrawReader::load(f);
            if (gr != nil) {
                gr->flush();
                _ed->viewer()->setrootgr(gr);
                delete f;
                break;
            } else {
                style->attribute("caption", "Import failed!");
                resetcaption = true;
                delete f;
            }
        } else {
            unlink(tempfile);
            char cmd[256];
            sprintf(cmd, "ls %s > %s", s->string(), tempfile);
            system(cmd);

            String fname(tempfile);
            f = InputFile::open(fname);
            if (f != nil) {
                long len = f->length();
                if (len == 0) {
                    delete f;
                    style->attribute("caption", "Import failed!");
                    resetcaption = true;
                } else {
                    char* buffer;
                    int len = f->read(buffer);
                    PolyGraphic* poly = new PolyGraphic;

                    char* prev = buffer;
                    while(true) {
                        char* ptr = strchr(prev, '\n');
                        if (ptr == nil || (buffer+len < ptr)) {
                            break;
                        }
                        CopyString str(prev, ptr-prev);
                        Graphic* gr = IdrawReader::load(str.string());
                        if (gr != nil) {
                            poly->append_(gr);
/*
                            if (gr != poly->child_(0)) {
                                poly->child_(0)->align(Center, gr, Center);
                            }
*/
                        }
                        prev = ptr+1;
                    }
                    if (poly->count_() == 0) {
                        delete f;
                        delete poly;
                        style->attribute("caption", "Import failed!");
                        resetcaption = true;
                    } else {
                        poly->flush();
                        
                        Transformer* tr = new Transformer;
                        for (long i = 0; i < poly->count_(); i++) {
                            Graphic* gr = poly->child_(i);
                            gr->transformer(tr);
                        }
                        tr->unref();
                        long sel = poly->child_(0)->count_();
                        for (long j = 0; j < sel; j++) {
                            tr = new Transformer;
                            for (i = 0; i < poly->count_(); i++) {
                                Graphic* gr = poly->child_(i)->child_(j);;
                                gr->transformer(tr);
                            }
                            tr->unref();
                        }
                        _ed->viewer()->setrootgr(poly);
                        delete f;
                        break;
                    }
                }                        
            } else {
                style->attribute("caption", "Import failed!");
                resetcaption = true;
            }
        }
    }
    if (resetcaption) {
        style->attribute("caption", "");
    }
}
    
class QuitAction : public Action {
public:
    QuitAction();
    virtual void execute();
};

QuitAction::QuitAction () {}

void QuitAction::execute () { WidgetKit::instance()->quit()->execute(); }

class Cleaner : public Action {
public:
    Cleaner(GlyphEditor* ed);
    virtual void execute();
protected:
    GlyphEditor* _ed;
};

Cleaner::Cleaner (GlyphEditor* ed) {
    _ed = ed;
}

void Cleaner::execute () {
    _ed->new_session();
}

Glyph* GlyphEditor::menus () {
    WidgetKit* widget = WidgetKit::instance();

    Menu* mb = widget->menubar();
    MenuItem* mi = widget->menubar_item("File");
    Menu* pulldown = widget->pulldown();
    MenuItem* cleaner = widget->menu_item("Purge");
    MenuItem* import = widget->menu_item("Import");
    MenuItem* quit = widget->menu_item("Quit");
    cleaner->action(new Cleaner(this));
    import->action(new Importer(this));
    quit->action(new QuitAction);

    mb->append_item(mi);
    mi->menu(pulldown);
    pulldown->append_item(import);
    pulldown->append_item(cleaner);
    pulldown->append_item(widget->menu_item_separator());
    pulldown->append_item(quit);

    return mb;
}


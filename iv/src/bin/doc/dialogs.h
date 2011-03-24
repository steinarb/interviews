#ifndef dialogs_h
#define dialogs_h

#include <IV-look/2.6/InterViews/dialog.h>
#include <IV-look/2.6/InterViews/filechooser.h> 

class Window;
class Dialog;
class FileChooser;
class ButtonState;
class StringEditor;

class Chooser : public FileChooser {
public:
    Chooser(const char*, const char* prompt);

    virtual const char* post (Window*, const char* ext);
};

class Asker : public Dialog {
public:
    Asker(const char*, const char* prompt);

    virtual const char* post (Window*, const char* initial);
protected:
    ButtonState* askerbs;
    StringEditor* askereditor;
};

class Confirmer : public Dialog {
public:
    Confirmer(const char*, const char* prompt);

    virtual int post (Window*);
protected:
    ButtonState* confirmerbs;
};

class Reporter : public Dialog {
public:
    Reporter(const char*, const char* message);

    virtual void post (Window*);
protected:
    ButtonState* reporterbs;
};

#endif

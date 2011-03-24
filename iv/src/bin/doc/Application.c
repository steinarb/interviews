/*
 * Copyright (c) 1991 Stanford University
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
 * Application
 */

#include "Application.h"

#include "Document.h"
#include "DocViewer.h"
#include "DialogMgr.h"

#include "properties.h"

#include <InterViews/event.h>
#include <InterViews/world.h>

#include <fstream.h>
#include <strstream.h>
#include <string.h>
#ifndef __GNUC__
#include <stdlib.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

static int DEFAULT_SIZE_HINT = 1000;
static const char* MAGIC = "%Doc-";

class ApplicationViewerInfo {
public:
    DocumentViewer* _viewer;
    DialogManager* _dialogs;
    char* _window_name;
    char* _icon_name;
};

#include "list.h"

declareList(ApplicationViewerInfo_List,ApplicationViewerInfo)
implementList(ApplicationViewerInfo_List,ApplicationViewerInfo)

static int viewer_info(
    DocumentViewer* viewer, ApplicationViewerInfo_List* viewers
) {
    long count = viewers->count();
    for (long i = 0; i < count; ++i) {
        ApplicationViewerInfo& info = viewers->item(i);
        if (info._viewer == viewer) {
            return i;
        }
    }
    return -1;
}

Application::Application () {
    _world = World::current();
    _viewer = new ApplicationViewerInfo_List;
}

Application::~Application () {
    delete _viewer;
}

void Application::run () {
    Event e;
    while (_viewer->count() > 0) {
        _world->read(e);
        e.handle();
    }
}

const char* Application::choose (
    DocumentViewer* viewer, const char* prompt, const char* filter
) {
    ApplicationViewerInfo& info = _viewer->item(viewer_info(viewer, _viewer));
    info._viewer->raise();
    return info._dialogs->choose(info._viewer, prompt, filter);
}

const char* Application::ask (
    DocumentViewer* viewer, const char* prompt, const char* initial
) {
    ApplicationViewerInfo& info = _viewer->item(viewer_info(viewer, _viewer));
    info._viewer->raise();
    return info._dialogs->ask(info._viewer, prompt, initial);
}

int Application::confirm (DocumentViewer* viewer, const char* prompt) {
    ApplicationViewerInfo& info = _viewer->item(viewer_info(viewer, _viewer));
    info._viewer->raise();
    return info._dialogs->confirm(info._viewer, prompt);
}

void Application::report (DocumentViewer* viewer, const char* prompt) {
    ApplicationViewerInfo& info = _viewer->item(viewer_info(viewer, _viewer));
    info._viewer->raise();
    info._dialogs->report(info._viewer, prompt);
}

void Application::complain (DocumentViewer* viewer, const char* prompt) {
    const char* complaint = _world->property_value(COMPLAINT_MODE);
    if (complaint == nil) {
        _world->RingBell(10);
    } else if (strcmp(complaint, "report") == 0) {
        report(viewer, prompt);
    } else if (strcmp(complaint, "bell") == 0) {
        _world->RingBell(10);
    } else if (strcmp(complaint, "ignore") == 0) {
        ;
    } else {
        _world->RingBell(10);
    }
}

void Application::open (DocumentViewer* viewer) {
    long index = viewer_info(viewer, _viewer);
    if (index < 0) {
        index = _viewer->count();
        ApplicationViewerInfo info;
        info._dialogs = new DialogManager();
        info._viewer = viewer;
        info._viewer->ref();
        info._viewer->map();
        info._window_name = nil;
        info._icon_name = nil;
        _viewer->append(info);
    }
    ApplicationViewerInfo& info = _viewer->item(index);
    const char* name = viewer->document()->name();
    if (info._window_name == nil || strcmp(info._window_name, name) != 0) {
        delete info._window_name;
        delete info._icon_name;
        if (name != nil) {
            info._window_name = strcpy(new char[strlen(name) + 1], name);
            const char* icon_name = strrchr(name, '/');
            if (icon_name != nil) {
                icon_name += 1;
            } else {
                icon_name = name;
            }
            info._icon_name = strcpy(new char[strlen(name) + 1], name);
            info._viewer->name(info._window_name);
            info._viewer->icon_name(info._icon_name);
        } else {
            info._window_name = nil;
            info._icon_name = nil;
        }
    }
}

void Application::close (DocumentViewer* viewer) {
    int index = viewer_info(viewer, _viewer);
    if (index >= 0) {
        ApplicationViewerInfo& info = _viewer->item(index);
        info._viewer->unmap();
        info._viewer->unref();
        delete info._dialogs;
        delete info._window_name;
        delete info._icon_name;
        _viewer->remove(index);
    }
}

Document* Application::read (const char* file_name) {
    Document* document = nil;
    struct stat filestats;
    if (strlen(file_name) > 0 && stat(file_name, &filestats) == 0) {
        ifstream in(file_name);
        char buffer[256];
        in.getline(buffer, 256);
        in.seekg(0);
        char* version = strrchr(buffer, MAGIC[0]);
        int l = strlen(MAGIC);
        if (
            version == nil
            || (
                strncmp(version, MAGIC, l) == 0
                && strcmp(version + l, _world->property_value(VERSION))==0
            )
        ) {
            document = new Document(this, int(filestats.st_size));
            document->read(in);
        }
    } else {
        document = new Document(this, DEFAULT_SIZE_HINT);
        strstream empty;
        empty << MAGIC;
        empty << _world->property_value(VERSION);
        empty << "\n";
        empty << "\\documentstyle{";
        empty << _world->property_value(DEFAULT_STYLE);
        empty << "}\n";
        empty.seekg(0);
        document->read(empty);
    }
    if (document != nil) {
        document->touch(false);
        document->notify();
    }
    return document;
}

void Application::write (Document* document, const char* name) {
    ofstream out(name);
    out << MAGIC;
    out << _world->property_value(VERSION);
    out << "\n";
    document->write(out);
    document->touch(false);
    document->notify();
}

boolean Application::file_path (
    const char* name, const char* extension, const char* pathlist, char* path
) {
    struct stat filestats;
    char filename[256];
    if (name[0] == '~' && name[1] == '/') {
        strcpy(filename, getenv("HOME"));
        strcat(filename, name+1);
    } else {
        strcpy(filename, name);
    }
    if (extension != nil && strrchr(filename, '.') <= strrchr(filename, '/')) {
        strcat(filename, ".");
        strcat(filename, extension);
    }
    if (filename[0] == '/') {
        strcpy(path, filename);
        return stat(path, &filestats) ==  0;
    } else {
        char path_list[256];
        strcpy(path_list, pathlist);
        char* p = strtok(path_list, ":");
        while (p != nil) {
            strcpy(path, p);
            strcat(path, "/");
            strcat(path, filename);
            if (stat(path, &filestats) == 0) {
                return true;
            } else {
                p = strtok(nil, ":");
            }
        }
        return false;
    }
}

boolean Application::command (const char* command) {
    if (strncmp(command, "application", 11) == 0) {
        const char* keyword = command + 12;
        if (strcmp(keyword, "quit") == 0) {
            long count = _viewer->count();
            for (long i = count-1; i >= 0; --i) {
                ApplicationViewerInfo& info = _viewer->item(i);
                info._viewer->ref();
                info._viewer->command("viewer close");
                info._viewer->unref();
            }
            return true;
        }
    }
    return false;
}

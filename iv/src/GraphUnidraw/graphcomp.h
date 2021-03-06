/*
 * Copyright (c) 1994-1999 Vectaport Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */

#ifndef graphcomp_h
#define graphcomp_h

#include <OverlayUnidraw/ovviews.h>
#include <OverlayUnidraw/ovcomps.h>
#include <OverlayUnidraw/scriptview.h>

#include <Unidraw/Components/psview.h>

class EdgeComp;
class NodeComp;
class UList;

//: component used to group a set of nodes and edges into a graph.
class GraphComp : public OverlaysComp {
public:
    GraphComp(const char* pathname = nil, OverlayComp* parent = nil);
    GraphComp(Graphic*, OverlayComp* parent = nil);
    GraphComp(istream&, const char* pathname = nil, OverlayComp* parent = nil);
    virtual ~GraphComp();

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    Component* Copy();
 
    void AppendEdge(EdgeComp*);
    // add edge component to the graph.
    UList* GraphEdges();
    // return list of edge components.

    virtual void GrowIndexedGS(Graphic*);
    virtual Graphic* GetIndexedGS(int);

    int GetNumEdge() { return _num_edge; }
    // number of edges in graph.
    int GetNumNode() { return _num_node; }
    // number of nodes in graph.

    virtual void SetPathName(const char*);
    // set pathname associated with graph from separate file.
    virtual const char* GetPathName();
    // get pathname associated with graph from separate file.
    virtual const char* GetBaseDir();
    // get base directory portion of associated pathname.
    virtual const char* GetFile();
    // get file portion of associated pathname.

protected:
    UList* _graphedges;
    int _num_edge;
    int _num_node;

    char* _pathname;
    char* _basedir;
    char* _file;

    Picture* _gslist;

protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _graph_params;

    CLASS_SYMID("GraphComp");
};

inline UList* GraphComp::GraphEdges() { return _graphedges; }

//: graphical view of GraphComp.
class GraphView : public OverlaysView {
public:
    GraphView(GraphComp* = nil);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
};

//: serialized view of GraphComp.
class GraphScript : public OverlaysScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    GraphScript(GraphComp* = nil);

    virtual void SetGSList(Clipboard*);
    virtual Clipboard* GetGSList();

    static int ReadChildren(istream&, void*, void*, void*, void*);
    virtual boolean Definition(ostream&);

protected:
    Clipboard* _gslist;
};

/*****************************************************************************/

//: top-level component in a graphdraw document.
// Note: in the future this class could be derived from GraphComp, once the
// signature of OverlayIdrawComp is completely available as a mixin.
class GraphIdrawComp : public OverlayIdrawComp {
public:
    GraphIdrawComp(const char* pathname = nil, OverlayComp* parent = nil);
    // construct from 'pathname'.
    GraphIdrawComp(istream&, const char* pathname = nil, OverlayComp* parent = nil);
    // construct from istream that may have been opened on 'pathname'.

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);

    virtual void Interpret(Command*);
    // interpret ungroup command; pass rest to base class.
    void Ungroup(OverlayComp*, Clipboard*, Command*);
    // method to assist in ungrouping.

    Component* Copy();
 
    int GetNumEdge() { return _num_edge; }
    // number of edges in the document.
    int GetNumNode() { return _num_node; }
    // number of nodes in the document.

protected:
    int _num_edge;
    int _num_node;

protected:
    ParamList* GetParamList();
    void GrowParamList(ParamList*);
    static ParamList* _graph_idraw_params;

    CLASS_SYMID("GraphIdrawComp");
};

//: graphical view of GraphIdrawComp.
class GraphIdrawView : public OverlayIdrawView {
public:
    GraphIdrawView(GraphIdrawComp* = nil);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
};

//: serialized view of GraphIdrawComp.
class GraphIdrawScript : public OverlayIdrawScript {
public:
    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
    GraphIdrawScript(GraphIdrawComp* = nil);

    static int ReadChildren(istream&, void*, void*, void*, void*);
    // read through, and construct, all sub-objects in the document;
    // for use of GraphIdrawComp istream constructor.
    virtual boolean Emit(ostream&);
    // output graphdraw document.
};

#endif





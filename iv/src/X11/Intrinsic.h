/*
* $XConsortium: Intrinsic.h,v 1.130 89/12/15 17:20:05 swick Exp $
* $oHeader: Intrinsic.h,v 1.10 88/09/01 10:33:34 asente Exp $
*/

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef _XtIntrinsic_h
#define _XtIntrinsic_h

#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/Xresource.h>
#include	<X11/Xos.h>

#define XtSpecificationRelease 4

#ifdef XTFUNCPROTO
#undef NeedFunctionPrototypes
#define NeedFunctionPrototypes 1
#else
#undef NeedFunctionPrototypes
#define NeedFunctionPrototypes 0
#undef NeedWidePrototypes
#define NeedWidePrototypes 0
#endif

#ifndef NeedFunctionPrototypes
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus)
#define NeedFunctionPrototypes 1
#else
#define NeedFunctionPrototypes 0
#endif /* __STDC__ */
#endif /* NeedFunctionPrototypes */

/* NeedVarargsPrototypes is temporary until function prototypes work everywhere */
#ifndef NeedVarargsPrototypes
#if defined(FUNCPROTO) || defined(__STDC__) || defined(__cplusplus) || defined(c_plusplus) || NeedFunctionPrototypes
#define NeedVarargsPrototypes 1
#else
#define NeedVarargsPrototypes 0
#endif /* __STDC__ */
#endif /* NeedVarargsPrototypes */

#if defined(__cplusplus) || defined(c_plusplus)
#define CONST const
/* make const String do the right thing */
#define String		char*
#else
#define CONST
#endif /* __cplusplus */

#ifndef NeedWidePrototypes
#if defined(NARROWPROTO)
#define NeedWidePrototypes 0
#else
#define NeedWidePrototypes 1		/* default to make interropt. easier */
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#ifdef VMS
#define externalref globalref
#define externaldef(psect) globaldef {"psect"} noshare
#else
#define externalref extern
#define externaldef(psect)
#endif /* VMS */

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#define XtNumber(arr)		((Cardinal) (sizeof(arr) / sizeof(arr[0])))

typedef char *String;
typedef struct _WidgetRec *Widget;
typedef Widget *WidgetList;
typedef struct _WidgetClassRec *WidgetClass;
typedef struct _CompositeRec *CompositeWidget;
typedef struct _XtActionsRec *XtActionList;
typedef struct _XtEventRec *XtEventTable;
typedef struct _XtBoundAccActionRec *XtBoundAccActions;

typedef struct _XtAppStruct *XtAppContext;
typedef unsigned long	XtValueMask;
typedef unsigned long	XtIntervalId;
typedef unsigned long	XtInputId;
typedef unsigned long	XtWorkProcId;
typedef unsigned int	XtGeometryMask;
typedef unsigned long	XtGCMask;   /* Mask of values that are used by widget*/
typedef unsigned long	Pixel;	    /* Index into colormap		*/
typedef int		XtCacheType;
#define			XtCacheNone	  0x001
#define			XtCacheAll	  0x002
#define			XtCacheByDisplay  0x003
#define			XtCacheRefCount	  0x100

/****************************************************************
 *
 * System Dependent Definitions; see spec for specific range
 * requirements.  Do not assume every implementation uses the
 * same base types!
 *
 *
 * XtArgVal ought to be a union of XtPointer, char *, long, int *, and proc *
 * but casting to union types is not really supported.
 *
 * So the typedef for XtArgVal should be chosen such that
 *
 *	sizeof (XtArgVal) >=	sizeof(XtPointer)
 *				sizeof(char *)
 *				sizeof(long)
 *				sizeof(int *)
 *				sizeof(proc *)
 *
 * ArgLists rely heavily on the above typedef.
 *
 ****************************************************************/
#ifdef CRAY
typedef long		Boolean;
typedef char*		XtArgVal;
typedef long		XtEnum;
#else
typedef char		Boolean;
typedef long		XtArgVal;
typedef unsigned char	XtEnum;
#endif

typedef unsigned int	Cardinal;
typedef unsigned short	Dimension;  /* Size in pixels			*/
typedef short		Position;   /* Offset from 0 coordinate		*/
typedef char*		XtPointer;

typedef XtPointer	Opaque;

#include <X11/Core.h>
#include <X11/Composite.h>
#include <X11/Constraint.h>
#include <X11/Object.h>
#include <X11/RectObj.h>

typedef struct _TranslationData *XtTranslations;
typedef struct _TranslationData *XtAccelerators;
typedef unsigned int Modifiers;

typedef void (*XtActionProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XEvent*		/* event */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

typedef XtActionProc* XtBoundActions;

typedef struct _XtActionsRec{
    String	 string;
    XtActionProc proc;
} XtActionsRec;

typedef enum {
/* address mode		parameter representation    */
/* ------------		------------------------    */
    XtAddress,		/* address		    */
    XtBaseOffset,	/* offset		    */
    XtImmediate,	/* constant		    */
    XtResourceString,	/* resource name string	    */
    XtResourceQuark,	/* resource name quark	    */
    XtWidgetBaseOffset,	/* offset from ancestor	    */
    XtProcedureArg	/* procedure to invoke	    */
} XtAddressMode;

typedef struct {
    XtAddressMode   address_mode;
    XtPointer	    address_id;
    Cardinal	    size;
} XtConvertArgRec, *XtConvertArgList;

typedef void (*XtConvertArgProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Cardinal*		/* size */,
    XrmValue*		/* value */
#endif
);

typedef struct {
    XtGeometryMask request_mode;
    Position x, y;
    Dimension width, height, border_width;
    Widget sibling;
    int stack_mode;   /* Above, Below, TopIf, BottomIf, Opposite, DontChange */
} XtWidgetGeometry;

/* Additions to Xlib geometry requests: ask what would happen, don't do it */
#define XtCWQueryOnly	(1 << 7)

/* Additions to Xlib stack modes: don't change stack order */
#define XtSMDontChange	5

typedef void (*XtConverter)(
#if NeedFunctionPrototypes
    XrmValue*		/* args */,
    Cardinal*		/* num_args */,
    XrmValue*		/* from */,
    Xrmvalue*		/* to */
#endif
);

typedef Boolean (*XtTypeConverter)(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XrmValue*		/* args */,
    Cardinal*		/* num_args */,
    XrmValue*		/* from */,
    XrmValue*		/* to */,
    XtPointer*		/* converter_data */,
#endif
);

typedef void (*XtDestructor)(
#if NeedFunctionPrototypes
    XtAppContext	/* app */,
    XrmValue*		/* to */,
    XtPointer 		/* converter_data */,
    XrmValue*		/* args */,
    Cardinal*		/* num_args */
#endif
);

typedef Opaque XtCacheRef;

typedef Opaque XtActionHookId;

typedef void (*XtActionHookProc)(
#if NeedFunctionPrototypes
    Widget		/* w */,
    XtPointer		/* client_data */,
    String		/* action_name */,
    XEvent*		/* event */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

typedef void (*XtKeyProc)(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    KeyCode* 		/* keycode */,
    Modifiers*		/* modifiers */,
    Modifiers*		/* modifiers_return */,
    KeySym*		/* keysym_return */
#endif
);

typedef void (*XtCaseProc)(
#if NeedFunctionPrototypes
    KeySym*		/* keysym */,
    KeySym*		/* lower_return */,
    KeySym*		/* upper_return */
#endif
);

typedef void (*XtEventHandler)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,
    XEvent*		/* event */,
    Boolean*		/* continue_to_dispatch */
#endif
);
typedef unsigned long EventMask;

typedef enum {XtListHead, XtListTail } XtListPosition;

typedef unsigned long	XtInputMask;
#define XtInputNoneMask		0L
#define XtInputReadMask		(1L<<0)
#define XtInputWriteMask	(1L<<1)
#define XtInputExceptMask	(1L<<2)

typedef void (*XtTimerCallbackProc)(
#if NeedFunctionPrototypes
    XtPointer 		/* closure */,
    XtIntervalId*	/* id */
#endif
);

typedef void (*XtInputCallbackProc)(
#if NeedFunctionPrototypes
    XtPointer 		/* closure */,
    int*		/* source */,
    XtInputId*		/* id */
#endif
);

typedef struct {
    String	name;
    XtArgVal	value;
} Arg, *ArgList;

typedef XtPointer	XtVarArgsList;

typedef void (*XtCallbackProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,	/* data the application registered */
    XtPointer 		/* call_data */	/* callback specific data */
#endif
);

typedef struct _XtCallbackRec {
    XtCallbackProc  callback;
    XtPointer	    closure;
} XtCallbackRec, *XtCallbackList;

typedef enum {
	XtCallbackNoList,
	XtCallbackHasNone,
	XtCallbackHasSome
} XtCallbackStatus;

typedef enum  {
    XtGeometryYes,	  /* Request accepted. */
    XtGeometryNo,	  /* Request denied. */
    XtGeometryAlmost,	  /* Request denied, but willing to take replyBox. */
    XtGeometryDone	  /* Request accepted and done. */
} XtGeometryResult;

typedef enum {XtGrabNone, XtGrabNonexclusive, XtGrabExclusive} XtGrabKind;

typedef struct {
    Widget  shell_widget;
    Widget  enable_widget;
} XtPopdownIDRec, *XtPopdownID;

typedef struct _XtResource {
    String	resource_name;	/* Resource name			    */
    String	resource_class;	/* Resource class			    */
    String	resource_type;	/* Representation type desired		    */
    Cardinal	resource_size;	/* Size in bytes of representation	    */
    Cardinal	resource_offset;/* Offset from base to put resource value   */
    String	default_type;	/* representation type of specified default */
    XtPointer	default_addr;	/* Address of default resource		    */
} XtResource, *XtResourceList;

typedef void (*XtErrorMsgHandler)(
#if NeedFunctionPrototypes
    String 		/* name */,
    String		/* type */,
    String		/* class */,
    String		/* defaultp */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

typedef void (*XtErrorHandler)(
#if NeedFunctionPrototypes
  String		/* msg */
#endif
);

typedef Boolean (*XtWorkProc)(
#if NeedFunctionPrototypes
    XtPointer 		/* closure */	/* data the application registered */
#endif
);

typedef struct {
    char match;
    String substitution;
} SubstitutionRec, *Substitution;

typedef Boolean (*XtFilePredicate)( /* String filename */ );

typedef XtPointer XtRequestId;

/*
 * Routine to get the value of a selection as a given type.  Returns
 * TRUE if it successfully got the value as requested, FALSE otherwise.  
 * selection is the atom describing the type of selection (e.g. 
 * primary or secondary). value is set to the pointer of the converted 
 * value, with length elements of data, each of size indicated by format.
 * (This pointer will be freed using XtFree when the selection has
 *  been delivered to the requestor.)  target is
 * the type that the conversion should use if possible; type is returned as
 * the actual type returned.  Format should be either 8, 16, or 32, and
 * specifies the word size of the selection, so that Xlib and the server can
 * convert it between different machine types. */

typedef Boolean (*XtConvertSelectionProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */,
    Atom*		/* target */,
    Atom*		/* type_return */,
    XtPointer*		/* value_return */,
    unsigned long*	/* length_return */,
    int*		/* format_return */
#endif
);

/*
 * Routine to inform a widget that it no longer owns the given selection.
 */

typedef void (*XtLoseSelectionProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */
#endif
);

/*
 * Routine to inform the selection owner when a selection requestor
 * has successfully retrieved the selection value.
 */

typedef void (*XtSelectionDoneProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */,
    Atom*		/* target */
#endif
);


/*
 * Routine to call back when a requested value has been obtained for a
 *  selection.
 */

typedef void (*XtSelectionCallbackProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,
    Atom*		/* selection */,
    Atom*		/* type */,
    XtPointer 		/* value */,
    unsigned long*	/* length */,
    int*		/* format */
#endif
);

typedef void (*XtLoseSelectionIncrProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */,
    XtPointer 		/* client_data */
#endif
);

typedef void (*XtSelectionDoneIncrProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */,
    Atom*		/* target */,
    XtRequestId*	/* receiver_id */,
    XtPointer 		/* client_data */
#endif
);

typedef Boolean (*XtConvertSelectionIncrProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */,
    Atom*		/* target */,
    Atom*		/* type */,
    XtPointer*		/* value */,
    unsigned long*	/* length */,
    int*		/* format */,
    unsigned long*	/* max_length */,
    XtPointer 		/* client_data */,
    XtRequestId*	/* receiver_id */
#endif
);

typedef void (*XtCancelConvertSelectionProc)(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom*		/* selection */,
    Atom*		/* target */,
    XtRequestId*	/* receiver_id */,
    XtPointer 		/* client_data */
#endif
);

/***************************************************************
 *
 * Exported Interfaces
 *
 ****************************************************************/

#ifdef __cplusplus			/* do not leave open across includes */
extern "C" {				/* for C++ V2.0 */
#endif

extern Boolean XtConvertAndStore(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* from_type */,
    XrmValue*		/* from */,
    CONST String 	/* to_type */,
    XrmValue*		/* to_in_out */
#endif
);

extern Boolean XtCallConverter(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XtTypeConverter 	/* converter */,
    XrmValuePtr 	/* args */,
    Cardinal 		/* num_args */,
    XrmValuePtr 	/* from */,
    XrmValue*		/* to_return */,
    XtCacheRef*		/* cache_ref_return */
#endif
);

extern Boolean XtDispatchEvent(
#if NeedFunctionPrototypes
    XEvent* 		/* event */
#endif
);

extern Boolean XtCallAcceptFocus(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Time*		/* t */
#endif
);

extern Boolean XtPeekEvent(
#if NeedFunctionPrototypes
    XEvent*		/* event */
#endif
);

extern Boolean XtAppPeekEvent(
#if NeedFunctionPrototypes
    XtAppContext 	/* appContext */,
    XEvent*		/* event */
#endif
);

extern Boolean XtIsSubclass(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    WidgetClass 	/* widgetClass */
#endif
);

extern Boolean XtIsObject(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern Boolean _XtCheckSubclassFlag( /* implementation-private */
#if NeedFunctionPrototypes
    Widget		/* object */,
#if NeedWidePrototypes
    /* XtEnum */ unsigned /* type_flag */
#else
    XtEnum		/* type_flag */
#endif /* NeedWidePrototypes */
#endif
);

extern Boolean _XtIsSubclassOf( /* implementation-private */
#if NeedFunctionPrototypes
    Widget		/* object */,
    WidgetClass		/* widget_class */,
    WidgetClass		/* flag_class */,
    XtEnum		/* type_flag */
#endif
);

extern Boolean XtIsManaged(
#if NeedFunctionPrototypes
    Widget 		/* rectobj */
#endif
);

extern Boolean XtIsRealized(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern Boolean XtIsSensitive(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

/*
 * Set the given widget to own the selection.  The convertProc should
 * be called when someone wants the current value of the selection. If it
 * is not NULL, the
 * losesSelection gets called whenever the window no longer owns the selection
 * (because someone else took it). If it is not NULL, the doneProc gets
 * called when the widget has provided the current value of the selection
 * to a requestor and the requestor has indicated that it has succeeded
 * in reading it by deleting the property.
 */

extern Boolean XtOwnSelection(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Time 		/* time */,
    XtConvertSelectionProc /* convert */,
    XtLoseSelectionProc	/* lose */,
    XtSelectionDoneProc /* done */
#endif
);

/* incremental selection interface */

extern Boolean XtOwnSelectionIncremental(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Time 		/* time */,
    XtConvertSelectionIncrProc /* convert_callback */,
    XtLoseSelectionIncrProc /* lose_callback */,
    XtSelectionDoneIncrProc /* done_callback */,
    XtCancelSelectionCallbackProc /* cancel_callback */,
    XtPointer 		/* client_data */
#endif
);

extern XtGeometryResult XtMakeResizeRequest(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
#if NeedWidePrototypes
    /* Dimension */ unsigned /* width */,
    /* Dimension */ unsigned /* height */,
#else
    Dimension		/* width */,
    Dimension 		/* height */,
#endif /* NeedWidePrototypes */
    Dimension*		/* replyWidth */,
    Dimension*		/* replyHeight */
#endif
);

extern void XtTransformCoords(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
#if NeedWidePrototypes
    /* Position */ int	/* x */,
    /* Position	*/ int	/* y */,
#else
    Position		/* x */,
    Position		/* y */,
#endif /* NeedWidePrototypes */
    Position*		/* rootx */,
    Position*		/* rooty */
#endif
);

/* %%% Caution: don't declare any functions past this point that
 * return one of the following types or take a pointer to one of
 * the following types.
 */

#if NeedWidePrototypes
#define Boolean		int
#define Dimension	unsigned int
#define Position	int
#define XtEnum		unsigned int
#endif /* NeedWidePrototypes */

extern void XtStringConversionWarning(
#if NeedFunctionPrototypes
    CONST String	/* from */,	/* String attempted to convert.	    */
    CONST String	/* toType */	/* Type attempted to convert it to. */
#endif
);

extern void XtDisplayStringConversionWarning(
#if NeedFunctionPrototypes
    Display*	 	/* dpy */,
    CONST String	/* from */,	/* String attempted to convert.	    */
    CONST String	/* toType */	/* Type attempted to convert it to. */
#endif
);

externalref XtConvertArgRec colorConvertArgs[];
externalref XtConvertArgRec screenConvertArg[];

extern void XtAppAddConverter( /* obsolete */
#if NeedFunctionPrototypes
    XtAppContext	/* app */,
    CONST String	/* from_type */,
    CONST String	/* to_type */,
    XtConverter 	/* converter */,
    XtConvertArgList	/* convert_args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtAddConverter( /* obsolete */
#if NeedFunctionPrototypes
    CONST String	/* from_type */,
    CONST String 	/* to_type */,
    XtConverter 	/* converter */,
    XtConvertArgList 	/* convert_args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtSetTypeConverter(
#if NeedFunctionPrototypes
    CONST String 	/* from_type */,
    CONST String 	/* to_type */,
    XtTypeConverter 	/* converter */,
    XtConvertArgList 	/* convert_args */,
    Cardinal 		/* num_args */,
    XtCacheType 	/* cache_type */,
    XtDestructor 	/* destructor */
#endif
);

extern void XtAppSetTypeConverter(
#if NeedFunctionPrototypes
    XtAppContext 	/* app_context */,
    CONST String 	/* from_type */,
    CONST String 	/* to_type */,
    XtTypeConverter 	/* converter */,
    XtConvertArgList 	/* convert_args */,
    Cardinal 		/* num_args */,
    XtCacheType 	/* cache_type */,
    XtDestructor 	/* destructor */
#endif
);

extern void XtConvert(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* from_type */,
    XrmValue*		/* from */,
    CONST String 	/* to_type */,
    XrmValue*		/* to_return */
#endif
);

extern void XtDirectConvert(
#if NeedFunctionPrototypes
    XtConverter 	/* converter */,
    XrmValuePtr 	/* args */,
    Cardinal 		/* num_args */,
    XrmValuePtr 	/* from */,
    XrmValue*		/* to_return */
#endif
);

/****************************************************************
 *
 * Translation Management
 *
 ****************************************************************/

extern XtTranslations XtParseTranslationTable(
#if NeedFunctionPrototypes
    CONST String	/* source */
#endif
);

extern XtAccelerators XtParseAcceleratorTable(
#if NeedFunctionPrototypes
    CONST String	/* source */
#endif
);

extern void XtOverrideTranslations(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtTranslations 	/* new */
#endif
);

extern void XtAugmentTranslations(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtTranslations 	/* new */
#endif
);

extern void XtInstallAccelerators(
#if NeedFunctionPrototypes
    Widget 		/* destination */,
    Widget		/* source */
#endif
);

extern void XtInstallAllAccelerators(
#if NeedFunctionPrototypes
    Widget 		/* destination */,
    Widget		/* source */
#endif
);

extern void XtUninstallTranslations(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtAppAddActions(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtActionList 	/* actions */,
    Cardinal 		/* num_actions */
#endif
);

extern void XtAddActions(
#if NeedFunctionPrototypes
    XtActionList 	/* actions */,
    Cardinal 		/* num_actions */
#endif
);

extern XtActionHookId XtAppAddActionHook(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtActionHookProc 	/* proc */,
    XtPointer 		/* client_data */
#endif
);

extern void XtRemoveActionHook(
#if NeedFunctionPrototypes
    XtActionHookId 	/* id */
#endif
);

extern void XtCallActionProc(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    CONST String	/* action */,
    XEvent*		/* event */,
    String*		/* params */,
    Cardinal		/* num_params */
#endif
);

extern void XtRegisterGrabAction(
#if NeedFunctionPrototypes
    XtActionProc 	/* action_proc */,
    Boolean 		/* owner_events */,
    unsigned int 	/* event_mask */,
    int			/* pointer_mode */,
    int	 		/* keyboard_mode */
#endif
);

extern void XtSetMultiClickTime(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    int 		/* milliseconds */
#endif
);

extern int XtGetMultiClickTime(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

extern KeySym XtGetActionKeysym(
#if NeedFunctionPrototypes
    XEvent*		/* event */,
    Modifiers*		/* modifiers_return */
#endif
);

/***************************************************************
 *
 * Keycode and Keysym procedures for translation management
 *
 ****************************************************************/

extern void XtTranslateKeycode(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    KeyCode 		/* keycode */,
    Modifiers 		/* modifiers */,
    Modifiers*		/* modifiers_return */,
    KeySym*		/* keysym_return */
#endif
);

extern void XtTranslateKey(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    KeyCode*		/* keycode */,
    Modifiers*		/* modifiers */,
    Modifiers*		/* modifiers_return */,
    KeySym*		/* keysym_return */
#endif
);

extern void XtSetKeyTranslator(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XtKeyProc 		/* proc */
#endif
);

extern void XtRegisterCaseConverter(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XtCaseProc 		/* proc */,
    KeySym 		/* start */,
    KeySym 		/* stop */
#endif
);

extern void XtConvertCase(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    KeySym 		/* keysym */,
    KeySym*		/* lower_return */,
    KeySym*		/* upper_return */
#endif
);

extern KeySym* XtGetKeysymTable(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    KeyCode*		/* min_keycode_return */,
    int*		/* keysyms_per_keycode_return */
#endif
);

extern void XtKeysymToKeycodeList(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    KeySym 		/* keysym */,
    KeyCode**		/* keycodes_return */,
    Cardinal*		/* keycount_return */
#endif
);

/****************************************************************
 *
 * Event Management
 *
 ****************************************************************/

#define XtAllEvents ((EventMask) -1L)

extern void XtInsertEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */,
    XtListPosition 	/* position */
#endif
);

extern void XtInsertRawEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */,
    XtListPosition 	/* position */
#endif
);

extern void XtAddEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */
#endif
);

extern void XtRemoveEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */
#endif
);

extern void XtAddRawEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */
#endif
);

extern void XtRemoveRawEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */
#endif
);

extern void XtInsertEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */,
    XtListPosition 	/* position */
#endif
);

extern void XtInsertRawEventHandler(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    EventMask 		/* eventMask */,
    Boolean 		/* nonmaskable */,
    XtEventHandler 	/* proc */,
    XtPointer 		/* closure */,
    XtListPosition 	/* position */
#endif
);

extern EventMask XtBuildEventMask(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtAddGrab(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Boolean 		/* exclusive */,
    Boolean 		/* spring_loaded */
#endif
);

extern void XtRemoveGrab(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtProcessEvent(
#if NeedFunctionPrototypes
    XtInputMask 		/* mask */
#endif
);

extern void XtAppProcessEvent(
#if NeedFunctionPrototypes
    XtAppContext 		/* app */,
    XtInputMask 		/* mask */
#endif
);

extern void XtMainLoop(
#if NeedFunctionPrototypes
    void
#endif
);


extern void XtAppMainLoop(
#if NeedFunctionPrototypes
    XtAppContext 		/* app */
#endif
);

extern void XtAddExposureToRegion(
#if NeedFunctionPrototypes
    XEvent*		/* event */,
    Region 		/* region */
#endif
);

extern void XtSetKeyboardFocus(
#if NeedFunctionPrototypes
    Widget		/* subtree */,
    Widget 		/* descendent */
#endif
);

extern Time XtLastTimestampProcessed(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

/****************************************************************
 *
 * Event Gathering Routines
 *
 ****************************************************************/


extern XtIntervalId XtAddTimeOut(
#if NeedFunctionPrototypes
    unsigned long 	/* interval */,
    XtTimerCallbackProc /* proc */,
    XtPointer 		/* closure */
#endif
);

extern XtIntervalId XtAppAddTimeOut(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    unsigned long 	/* interval */,
    XtTimerCallbackProc /* proc */,
    XtPointer 		/* closure */
#endif
);

extern void XtRemoveTimeOut(
#if NeedFunctionPrototypes
    XtIntervalId 	/* timer */
#endif
);

extern XtInputId XtAddInput(
#if NeedFunctionPrototypes
    int 		/* source */,
    XtPointer 		/* condition */,
    XtInputCallbackProc /* proc */,
    XtPointer 		/* closure */
#endif
);

extern XtInputId XtAppAddInput(
#if NeedFunctionPrototypes
    XtAppContext       	/* app */,
    int 		/* source */,
    XtPointer 		/* condition */,
    XtInputCallbackProc /* proc */,
    XtPointer 		/* closure */
#endif
);

extern void XtRemoveInput(
#if NeedFunctionPrototypes
    XtInputId 		/* id */
#endif
);

extern void XtNextEvent(
#if NeedFunctionPrototypes
    XEvent* 		/* event */
#endif
);

extern void XtAppNextEvent(
#if NeedFunctionPrototypes
    XtAppContext 	/* appContext */,
    XEvent*		/* event */
#endif
);

#define XtIMXEvent		1
#define XtIMTimer		2
#define XtIMAlternateInput	4
#define XtIMAll (XtIMXEvent | XtIMTimer | XtIMAlternateInput)

extern XtInputMask XtPending(
#if NeedFunctionPrototypes
    void
#endif
);

extern XtInputMask XtAppPending(
#if NeedFunctionPrototypes
    XtAppContext 	/* appContext */
#endif
);

/****************************************************************
 *
 * Random utility routines
 *
 ****************************************************************/

#define XtIsRectObj(object)	(_XtCheckSubclassFlag(object, (XtEnum)0x02))
#define XtIsWidget(object)	(_XtCheckSubclassFlag(object, (XtEnum)0x04))
#define XtIsComposite(widget)	(_XtCheckSubclassFlag(widget, (XtEnum)0x08))
#define XtIsConstraint(widget)	(_XtCheckSubclassFlag(widget, (XtEnum)0x10))
#define XtIsShell(widget)	(_XtCheckSubclassFlag(widget, (XtEnum)0x20))
#define XtIsOverrideShell(widget) \
    (_XtIsSubclassOf(widget, (WidgetClass)overrideShellWidgetClass, \
		     (WidgetClass)shellWidgetClass, (XtEnum)0x20))
#define XtIsWMShell(widget)	(_XtCheckSubclassFlag(widget, (XtEnum)0x40))
#define XtIsVendorShell(widget)	\
    (_XtIsSubclassOf(widget, (WidgetClass)vendorShellWidgetClass, \
		     (WidgetClass)wmShellWidgetClass, (XtEnum)0x40))
#define XtIsTransientShell(widget) \
    (_XtIsSubclassOf(widget, (WidgetClass)transientShellWidgetClass, \
		     (WidgetClass)wmShellWidgetClass, (XtEnum)0x40))
#define XtIsTopLevelShell(widget) (_XtCheckSubclassFlag(widget, (XtEnum)0x80))
#define XtIsApplicationShell(widget) \
    (_XtIsSubclassOf(widget, (WidgetClass)applicationShellWidgetClass, \
		     (WidgetClass)topLevelShellWidgetClass, (XtEnum)0x80))

extern void XtRealizeWidget(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

void XtUnrealizeWidget(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtDestroyWidget(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtSetSensitive(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Boolean 		/* sensitive */
#endif
);

extern void XtSetMappedWhenManaged(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Boolean 		/* mappedWhenManaged */
#endif
);

extern Widget XtNameToWidget(
#if NeedFunctionPrototypes
    Widget 		/* root */,
    CONST String	/* name */
#endif
);

extern Widget XtWindowToWidget(
#if NeedFunctionPrototypes
    Display*		/* display */,
    Window 		/* window */
#endif
);

/***************************************************************
 *
 * Arg lists
 *
 ****************************************************************/


#define XtSetArg(arg, n, d) \
    ((void)( (arg).name = (n), (arg).value = (XtArgVal)(d) ))

extern ArgList XtMergeArgLists(
#if NeedFunctionPrototypes
    ArgList 		/* args1 */,
    Cardinal 		/* num_args1 */,
    ArgList 		/* args2 */,
    Cardinal 		/* num_args2 */
#endif
);

/***************************************************************
 *
 * Vararg lists
 *
 ****************************************************************/

#define XtVaNestedList  "XtVaNestedList"
#define XtVaTypedArg    "XtVaTypedArg"

extern XtVarArgsList XtVaCreateArgsList(
#if NeedVarargsPrototypes
    XtPointer		/*unused*/, ...
#endif
);

/*************************************************************
 *
 * Information routines
 *
 ************************************************************/

#ifndef _XtIntrinsicP_h

/* We're not included from the private file, so define these */

extern Display *XtDisplay(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern Display *XtDisplayOfObject(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern Screen *XtScreen(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern Screen *XtScreenOfObject(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern Window XtWindow(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern Window XtWindowOfObject(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern String XtName(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern WidgetClass XtSuperclass(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern WidgetClass XtClass(
#if NeedFunctionPrototypes
    Widget 		/* object */
#endif
);

extern Widget XtParent(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

#endif /*_XtIntrinsicP_h*/

#define XtMapWidget(widget)	XMapWindow(XtDisplay(widget), XtWindow(widget))
#define XtUnmapWidget(widget)	\
		XUnmapWindow(XtDisplay(widget), XtWindow(widget))

extern void XtAddCallback(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* callback_name */,
    XtCallbackProc 	/* callback */,
    XtPointer 		/* closure */
#endif
);

extern void XtRemoveCallback(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* callback_name */,
    XtCallbackProc 	/* callback */,
    XtPointer 		/* closure */
#endif
);

extern void XtAddCallbacks(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String	/* callback_name */,
    XtCallbackList 	/* callbacks */
#endif
);

extern void XtRemoveCallbacks(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* callback_name */,
    XtCallbackList 	/* callbacks */
#endif
);

extern void XtRemoveAllCallbacks(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* callback_name */
#endif
);


extern void XtCallCallbacks(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* callback_name */,
    XtPointer 		/* call_data */
#endif
);

extern void XtCallCallbackList(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    XtCallbackList 	/* callbacks */,
    XtPointer 		/* call_data */
#endif
);

extern XtCallbackStatus XtHasCallbacks(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    CONST String 	/* callback_name */
#endif
);

/****************************************************************
 *
 * Geometry Management
 *
 ****************************************************************/


extern XtGeometryResult XtMakeGeometryRequest(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtWidgetGeometry*	/* request */,
    XtWidgetGeometry*	/* reply_return */
#endif
);

extern XtGeometryResult XtQueryGeometry(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtWidgetGeometry*	/* intended */,
    XtWidgetGeometry*	/* reply_return */
#endif
);

extern Widget XtCreatePopupShell(
#if NeedFunctionPrototypes
    CONST String	/* name */,
    WidgetClass 	/* widgetClass */,
    Widget 		/* parent */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern Widget XtVaCreatePopupShell(
#if NeedVarargsPrototypes
    CONST String	/* name */,
    WidgetClass		/* widgetClass */,
    Widget		/* parent */,
    ...
#endif
);

extern void XtPopup(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtGrabKind 		/* grab_kind */
#endif
);

extern void XtPopupSpringLoaded(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtCallbackNone(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,
    XtPointer 		/* call_data */
#endif
);

extern void XtCallbackNonexclusive(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,
    XtPointer 		/* call_data */
#endif
);

extern void XtCallbackExclusive(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,
    XtPointer 		/* call_data */
#endif
);

extern void XtPopdown(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern void XtCallbackPopdown(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,
    XtPointer 		/* call_data */
#endif
);

extern void XtMenuPopupAction(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XEvent*		/* event */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

extern Widget XtCreateWidget(
#if NeedFunctionPrototypes
    CONST String 	/* name */,
    WidgetClass 	/* widget_class */,
    Widget 		/* parent */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern Widget XtCreateManagedWidget(
#if NeedFunctionPrototypes
    CONST String 	/* name */,
    WidgetClass 	/* widget_class */,
    Widget 		/* parent */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern Widget XtVaCreateWidget(
#if NeedVarargsPrototypes
    CONST String	/* name */,
    WidgetClass		/* widget */,
    Widget		/* parent */,
    ...
#endif
);

extern Widget XtVaCreateManagedWidget(
#if NeedVarargsPrototypes
    CONST String	/* name */,
    WidgetClass		/* widget_class */,
    Widget		/* parent */,
    ...
#endif
);

extern Widget XtCreateApplicationShell(
#if NeedFunctionPrototypes
    CONST String 	/* name */,
    WidgetClass 	/* widget_class */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern Widget XtAppCreateShell(
#if NeedFunctionPrototypes
    CONST String	/* name */,
    CONST String	/* class */,
    WidgetClass 	/* widget_class */,
    Display*		/* display */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern Widget XtVaAppCreateShell(
#if NeedVarargsPrototypes
    CONST String	/* name */,
    CONST String	/* class */,
    WidgetClass		/* widget_class */,
    Display*		/* display */,
    ...
#endif
);

/****************************************************************
 *
 * Toolkit initialization
 *
 ****************************************************************/

extern void XtToolkitInitialize(
#if NeedFunctionPrototypes
    void
#endif
);

extern void XtDisplayInitialize(
#if NeedFunctionPrototypes
    XtAppContext 	/* appContext */,
    Display*		/* dpy */,
    CONST String	/* name */,
    CONST String	/* class */,
    XrmOptionDescRec* 	/* options */,
    Cardinal 		/* num_options */,
    Cardinal*		/* argc */,
    char**		/* argv */
#endif
);

extern Widget XtAppInitialize(
#if NeedFunctionPrototypes
    XtAppContext*	/* app_context_return */,
    CONST String		/* application_class */,
    XrmOptionDescList 	/* options */,
    Cardinal 		/* num_options */,
    Cardinal*		/* argc_in_out */,
    String*		/* argv_in_out */,
    CONST String*	/* fallback_resources */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern Widget XtVaAppInitialize(
#if NeedVarargsPrototypes
    XtAppContext*	/* app_context_return */,
    CONST String	/* application_class */,
    XrmOptionDescList	/* options */,
    Cardinal		/* num_options */,
    Cardinal*		/* argc_in_out */,
    String*		/* argv_in_out */,
    CONST String*	/* fallback_resources */,
    ...
#endif
);

extern Widget XtInitialize(
#if NeedFunctionPrototypes
    CONST String 	/* name */,
    CONST String 	/* class */,
    XrmOptionDescRec 	/* options */,
    Cardinal 		/* num_options */,
    Cardinal*		/* argc */,
    char**		/* argv */
#endif
);

extern Display *XtOpenDisplay(
#if NeedFunctionPrototypes
    XtAppContext 	/* appContext */,
    CONST String	/* displayName */,
    CONST String	/* applName */,
    CONST String	/* className */,
    XrmOptionDescRec*	/* urlist */,
    Cardinal 		/* num_urs */,
    Cardinal*		/* argc */,
    char**		/* argv */
#endif
);

extern XtAppContext XtCreateApplicationContext(
#if NeedFunctionPrototypes
    void
#endif
);

extern void XtAppSetFallbackResources(
#if NeedFunctionPrototypes
    XtAppContext 	/* app_context */,
    CONST String* 	/* specification_list */
#endif
);

extern void XtDestroyApplicationContext(
#if NeedFunctionPrototypes
    XtAppContext 	/* appContext */
#endif
);

extern void XtInitializeWidgetClass(
#if NeedFunctionPrototypes
    WidgetClass 	/* widget_class */
#endif
);

extern XtAppContext XtWidgetToApplicationContext(
#if NeedFunctionPrototypes
    Widget 		/* widget */
#endif
);

extern XtAppContext XtDisplayToApplicationContext(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

extern XrmDatabase XtDatabase(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

extern void XtCloseDisplay(
#if NeedFunctionPrototypes
    Display*		/* dpy */
#endif
);

extern void XtCopyFromParent(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    int			/* offset */,
    XrmValue*		/* value */
#endif
);

extern void XtCopyDefaultDepth(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    int			/* offset */,
    XrmValue*		/* value */
#endif
);

extern void XtCopyDefaultColormap(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    int			/* offset */,
    XrmValue*		/* value */
#endif
);

extern void XtCopyAncestorSensitive(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    int			/* offset */,
    XrmValue*		/* value */
#endif
);

extern void XtCopyScreen(
#if NeedFunctionPrototypes
    Widget		/* widget */,
    int			/* offset */,
    XrmValue*		/* value */
#endif
);

extern void XrmCompileResourceList(
#if NeedFunctionPrototypes
    XtResourceList 	/* resources */,
    Cardinal 		/* num_resources */
#endif
);

extern void XtGetApplicationResources(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* base */,
    XtResourceList 	/* resources */,
    Cardinal 		/* num_resources */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtVaGetApplicationResources(
#if NeedVarargsPrototypes
    Widget		/* widget */,
    XtPointer		/* base */,
    XtResourceList	/* resources */,
    Cardinal		/* num_resources */,
    ...
#endif
);

extern void XtGetSubresources(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* base */,
    CONST String 	/* name */,
    CONST String 	/* class */,
    XtResourceList 	/* resources */,
    Cardinal 		/* num_resources */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtVaGetSubresources(
#if NeedVarargsPrototypes
    Widget		/* widget */,
    XtPointer		/* base */,
    CONST String	/* name */,
    CONST String	/* class */,
    XtResourceList	/* resources */,
    Cardinal		/* num_resources */,
    ...
#endif
);

extern void XtSetValues(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtVaSetValues(
#if NeedVarargsPrototypes
    Widget		/* widget */,
    ...
#endif
);

extern void XtGetValues(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtVaGetValues(
#if NeedVarargsPrototypes
    Widget		/* widget */,
    ...
#endif
);

extern void XtSetSubvalues(
#if NeedFunctionPrototypes
    XtPointer 		/* base */,
    XtResourceList 	/* resources */,
    Cardinal 		/* num_resources */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtVaSetSubvalues(
#if NeedVarargsPrototypes
    XtPointer		/* base */,
    XtResourceList	/* resources */,
    Cardinal		/* num_resources */,
    ...
#endif
);

extern void XtGetSubvalues(
#if NeedFunctionPrototypes
    XtPointer 		/* base */,
    XtResourceList 	/* resources */,
    Cardinal 		/* num_resources */,
    ArgList 		/* args */,
    Cardinal 		/* num_args */
#endif
);

extern void XtVaGetSubvalues(
#if NeedVarargsPrototypes
    XtPointer		/* base */,
    XtResourceList	/* resources */,
    Cardinal		/* num_resources */,
    ...
#endif
);

extern void XtGetResourceList(
#if NeedFunctionPrototypes
    WidgetClass 	/* widget_class */,
    XtResourceList*	/* resources_return */,
    Cardinal*		/* num_resources_return */
#endif
);

extern void XtGetConstraintResourceList(
#if NeedFunctionPrototypes
    WidgetClass 	/* widget_class */,
    XtResourceList*	/* resources_return */,
    Cardinal*		/* num_resources_return */
#endif
);

#define XtUnspecifiedPixmap	((Pixmap)2)
#define XtUnspecifiedShellInt	(-1)
#define XtUnspecifiedWindow	((Window)2)
#define XtUnspecifiedWindowGroup ((Window)3)
#define XtDefaultForeground	"XtDefaultForeground"
#define XtDefaultBackground	"XtDefaultBackground"
#define XtDefaultFont		"XtDefaultFont"

#ifdef CRAY
#ifdef CRAY2

#define XtOffset(p_type,field) \
	(sizeof(int)*((unsigned int)&(((p_type)NULL)->field)))

#else	/* !CRAY2 */

#define XtOffset(p_type,field) ((unsigned int)&(((p_type)NULL)->field))

#endif	/* !CRAY2 */
#else	/* !CRAY */

#define XtOffset(p_type,field) \
	((Cardinal) (((char *) (&(((p_type)NULL)->field))) - ((char *) NULL)))

#endif /* !CRAY */

#define XtOffsetOf(s_type,field) XtOffset(s_type*,field)

#ifdef notdef
/* this doesn't work on picky compilers */
#define XtOffset(p_type,field)	((unsigned int)&(((p_type)NULL)->field))
#endif

/*************************************************************
 *
 * Error Handling
 *
 ************************************************************/

extern XtErrorMsgHandler XtAppSetErrorMsgHandler(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtErrorMsgHandler 	/* handler */
#endif
);

extern void XtSetErrorMsgHandler(
#if NeedFunctionPrototypes
    XtErrorMsgHandler 	/* handler */
#endif
);

extern XtErrorMsgHandler XtAppSetWarningMsgHandler(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtErrorMsgHandler 	/* handler */
#endif
);

extern void XtSetWarningMsgHandler(
#if NeedFunctionPrototypes
    XtErrorMsgHandler 	/* handler */
#endif
);

extern void XtAppErrorMsg(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    CONST String 	/* name */,
    CONST String	/* type */,
    CONST String	/* class */,
    CONST String	/* defaultp */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

extern void XtErrorMsg(
#if NeedFunctionPrototypes
    CONST String 	/* name */,
    CONST String	/* type */,
    CONST String	/* class */,
    CONST String	/* defaultp */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

extern void XtAppWarningMsg(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    CONST String 	/* name */,
    CONST String 	/* type */,
    CONST String 	/* class */,
    CONST String 	/* defaultp */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

extern void XtWarningMsg(
#if NeedFunctionPrototypes
    CONST String	/* name */,
    CONST String	/* type */,
    CONST String	/* class */,
    CONST String	/* defaultp */,
    String*		/* params */,
    Cardinal*		/* num_params */
#endif
);

extern XtErrorHandler XtAppSetErrorHandler(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtErrorHandler 	/* handler */
#endif
);

extern void XtSetErrorHandler(
#if NeedFunctionPrototypes
    XtErrorHandler 	/* handler */
#endif
);

extern XtErrorHandler XtAppSetWarningHandler(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtErrorHandler 	/* handler */
#endif
);

extern void XtSetWarningHandler(
#if NeedFunctionPrototypes
    XtErrorHandler 	/* handler */
#endif
);

extern void XtAppError(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    CONST String	/* message */
#endif
);

extern void XtError(
#if NeedFunctionPrototypes
    CONST String	/* message */
#endif
);

extern void XtAppWarning(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    CONST String	/* message */
#endif
);

extern void XtWarning(
#if NeedFunctionPrototypes
    CONST String	/* message */
#endif
);

extern XrmDatabase *XtAppGetErrorDatabase(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */
#endif
);

extern XrmDatabase *XtGetErrorDatabase(
#if NeedFunctionPrototypes
    void
#endif
);

extern void XtAppGetErrorDatabaseText(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    CONST String	/* name */,
    CONST String	/* type */,
    CONST String	/* class */,
    CONST String 	/* defaultp */,
    String 		/* buffer */,
    int 		/* nbytes */,
    XrmDatabase 	/* database */
#endif
);

extern void XtGetErrorDatabaseText(
#if NeedFunctionPrototypes
    CONST String	/* name */,
    CONST String	/* type */,
    CONST String	/* class */,
    CONST String 	/* defaultp */,
    String 		/* buffer */,
    int 		/* nbytes */
#endif
);

/****************************************************************
 *
 * Memory Management
 *
 ****************************************************************/

#define XtNew(type) ((type *) XtMalloc((unsigned) sizeof(type)))
#define XtNewString(str) \
    ((str) == NULL ? NULL : (strcpy(XtMalloc((unsigned)strlen(str) + 1), str)))

extern char *XtMalloc(
#if NeedFunctionPrototypes
    Cardinal 		/* size */
#endif
);

extern char *XtCalloc(
#if NeedFunctionPrototypes
    Cardinal		/* num */,
    Cardinal 		/* size */
#endif
);

extern char *XtRealloc(
#if NeedFunctionPrototypes
    char* 		/* ptr */,
    Cardinal 		/* num */
#endif
);

extern void XtFree(
#if NeedFunctionPrototypes
    char*		/* ptr */
#endif
);


/*************************************************************
 *
 *  Work procs
 *
 **************************************************************/

extern XtWorkProcId XtAddWorkProc(
#if NeedFunctionPrototypes
    XtWorkProc 		/* proc */,
    XtPointer 		/* closure */
#endif
);

extern XtWorkProcId XtAppAddWorkProc(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    XtWorkProc 		/* proc */,
    XtPointer 		/* closure */
#endif
);

extern void  XtRemoveWorkProc(
#if NeedFunctionPrototypes
    XtWorkProcId 	/* id */
#endif
);


/****************************************************************
 *
 * Graphic Context Management
 *****************************************************************/

extern GC XtGetGC(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtGCMask 		/* valueMask */,
    XGCValues* 		/* values */
#endif
);

extern void XtDestroyGC(
#if NeedFunctionPrototypes
    GC 			/* gc */
#endif
);

extern void XtReleaseGC(
#if NeedFunctionPrototypes
    Widget 		/* object */,
    GC 			/* gc */
#endif
);



extern void XtReleaseCacheRef(
#if NeedFunctionPrototypes
    XtCacheRef*		/* cache_ref */
#endif
);

extern void XtCallbackReleaseCacheRef(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,	/* XtCacheRef */
    XtPointer 		/* call_data */
#endif
);

extern void XtCallbackReleaseCacheRefList(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtPointer 		/* closure */,	/* XtCacheRef* */
    XtPointer 		/* call_data */
#endif
);

extern void XtSetWMColormapWindows(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Widget*		/* list */,
    Cardinal		/* count */
#endif
);

extern String XtFindFile(
#if NeedFunctionPrototypes
    CONST String	/* path */,
    Substitution	/* substitutions */,
    Cardinal 		/* num_substitutions */,
    XtFilePredicate	/* predicate */
#endif
);

extern String XtResolvePathname(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    CONST String	/* type */,
    CONST String	/* filename */,
    CONST String	/* suffix */,
    CONST String	/* path */,
    XtFilePredicate 	/* predicate */
#endif
);


/****************************************************************
 *
 * Selections
 *****************************************************************/

#define XT_CONVERT_FAIL (Atom)0x80000001
    

/*
 * The given widget no longer wants the selection.  If it still owns it, then
 * the selection owner is cleared, and the window's losesSelection is called.
 */

extern void XtDisownSelection(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Time 		/* time */
#endif
);

/*
 * Get the value of the given selection.  
 */

extern void XtGetSelectionValue(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Atom 		/* target */,
    XtSelectionCallbackProc /* callback */,
    XtPointer 		/* closure */,
    Time 		/* time */
#endif
);

extern void XtGetSelectionValues(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Atom*		/* targets */,
    int 		/* count */,
    XtSelectionCallbackProc /* callback */,
    XtPointer*		/* closures */,
    Time 		/* time */
#endif
);


/* Set the selection timeout value, in units of milliseconds */

extern void XtAppSetSelectionTimeout(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */,
    unsigned long 	/* timeout */
#endif
);

extern void XtSetSelectionTimeout(
#if NeedFunctionPrototypes
    unsigned long 	/* timeout */
#endif
);

 /* Return the selection timeout value, in units of milliseconds */

extern unsigned int XtAppGetSelectionTimeout(
#if NeedFunctionPrototypes
    XtAppContext 	/* app */
#endif
);

extern unsigned int XtGetSelectionTimeout(
#if NeedFunctionPrototypes
    void
#endif
);

extern XSelectionRequestEvent *XtGetSelectionRequest(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    XtRequestId 	/* request_id */
#endif
);

extern void XtGetSelectionValueIncremental(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Atom 		/* target */,
    XtSelectionCallbackProc /* selection_callback */,
    XtPointer 		/* client_data */,
    Time 		/* time */
#endif
);

extern void XtGetSelectionValuesIncremental(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Atom 		/* selection */,
    Atom*		/* targets */,
    int 		/* count */,
    XtSelectionCallbackProc /* callback */,
    XtPointer*		/* client_data */,
    Time 		/* time */
#endif
);

extern void XtGrabKey(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    KeySym 		/* keysym */,
    Modifiers	 	/* modifiers */,
    Boolean 		/* owner_events */,
    int 		/* pointer_mode */,
    int 		/* keyboard_mode */
#endif
);

extern void XtUngrabKey(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    KeySym 		/* keysym */,
    Modifiers	 	/* modifiers */
#endif
);

extern int XtGrabKeyboard(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Boolean 		/* owner_events */,
    int 		/* pointer_mode */,
    int 		/* keyboard_mode */,
    Time 		/* time */
#endif
);

extern void XtUngrabKeyboard(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Time 		/* time */
#endif
);

extern void XtGrabButton(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    int 		/* button */,
    Modifiers	 	/* modifiers */,
    Boolean 		/* owner_events */,
    unsigned int	/* event_mask */,
    int 		/* pointer_mode */,
    int 		/* keyboard_mode */,
    Window 		/* confine_to */,
    Cursor 		/* cursor */
#endif
);

extern void XtUngrabButton(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    unsigned int	/* button */,
    Modifiers	 	/* modifiers */
#endif
);

extern int XtGrabPointer(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Boolean 		/* owner_events */,
    unsigned int	/* event_mask */,
    int 		/* pointer_mode */,
    int 		/* keyboard_mode */,
    Window 		/* confine_to */,
    Cursor 		/* cursor */,
    Time 		/* time */
#endif
);

extern void XtUngrabPointer(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    Time 		/* time */
#endif
);

extern void XtGetApplicationNameAndClass(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    String*		/* name_return */,
    String*		/* class_return */
#endif
);

#ifdef __cplusplus
}						/* for C++ V2.0 */
#endif

#if NeedWidePrototypes
#undef Boolean
#undef Dimension
#undef Position
#undef XtEnum
#endif /* NeedWidePrototypes */

#undef String
#undef CONST

#endif /*_XtIntrinsic_h*/
/* DON'T ADD STUFF AFTER THIS #endif */

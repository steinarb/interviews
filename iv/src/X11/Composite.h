/* $XConsortium: Composite.h,v 1.10 89/12/12 20:08:44 swick Exp $ */
/* $oHeader: Composite.h,v 1.2 asente Exp $ */
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

#ifndef _XtComposite_h
#define _XtComposite_h

typedef struct _CompositeClassRec *CompositeWidgetClass;

#ifdef __cplusplus			/* do not leave open across includes */
extern "C" {					/* for C++ V2.0 */
#endif

extern void XtManageChildren(
#if NeedFunctionPrototypes
    WidgetList 		/* children */,
    Cardinal 		/* num_children */
#endif /* NeedFunctionPrototypes */
);

extern void XtManageChild(
#if NeedFunctionPrototypes
    Widget 		/* child */
#endif /* NeedFunctionPrototypes */
);

extern void XtUnmanageChildren(
#if NeedFunctionPrototypes
    WidgetList 		/* children */,
    Cardinal 		/* num_children */
#endif /* NeedFunctionPrototypes */
);

extern void XtUnmanageChild(
#if NeedFunctionPrototypes
    Widget 		/* child */
#endif /* NeedFunctionPrototypes */
);

#ifdef __cplusplus
}						/* for C++ V2.0 */
#endif

#ifndef COMPOSITE
externalref WidgetClass compositeWidgetClass;
#endif

#endif /* _XtComposite_h */
/* DON'T ADD STUFF AFTER THIS #endif */

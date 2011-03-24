.nr PO 1.25in
.DA "29 November 1989"
.TL
.ps +6
Changes from Version 2.5 to Version 2.6
.ps -6
.SH
.ps +2
Overview
.ps -2
.PP
This note describes the changes made to InterViews since the release
of Version 2.5 in May 1989.  The major changes in 2.6 are: cfront 2.0
compatibility; integration of GNU C++ 1.36.2, including support for
the DECstation 3100; a new Menu class with subclasses for pulldown and
popup menus; StringBrowser, FileBrowser, StringChooser, and
FileChooser classes for browsing and selecting from lists of strings
or file names; a simple class browser (iclass); and discontinuation of
the structured text library (libtext), the X10 version of
libInterViews, and iwm (old window manager).
.PP
The new Menu class and subclasses are a significant improvement over
the 2.5 Menu class they replace; however, code that uses 2.5 menus
must be modified to take advantage of the new classes.  The 2.6 menu
classes support popups, pulldowns, and pullrights, and we felt it
would be too confusing to retain the old Menu class, which handled
only popups.
.PP
The structured text library, X10 libInterViews, and iwm application
are obsolete, and we can no longer maintain them.  We believe the 2.5
source for these components will work with the 2.6 distribution, but
we have not tested them.  Because MIT will distribute the twm window
manager with X11R4, the InterViews distribution no longer includes a
version of twm.  The version of twm distributed with InterViews 2.5 is
compatible with 2.6 applications.
.SH
.ps +2
g++ 1.36.2
.ps -2
.PP
Version 2.6 includes a copy of g++\ 1.36.2 with modifications to
support the Mips architecture in general and the DECstation\ 3100 in
particular.  2.6 also works with both AT&T cfront 1.2 and 2.0.  Future
InterViews distributions will not be compatible with the C++ 1.2
language definition.
.PP
Because none of the InterViews code uses libg++ (the GNU C++ library),
an application written with InterViews and compiled with g++ (or
cfront) is not considered to be derived from GNU software.  Thus there
continues to be no licensing restrictions on InterViews-based
applications.
.PP
To allow usage in different compilation environments, InterViews
includes a compiler driver (CC command), static object analyzer
(nmunch), and library support for initialization (_main, exit).  See
the release notes for information on using a local C++ compiler
command.
.SH
.ps +2
Destructors and class Resource
.ps -2
.PP
As we compiled InterViews with cfront 2.0, we discovered that it is
not legal C++ to use ``delete'' to decrement the reference count of a
resource object.  The 2.0 translator changes an object's vtbl pointer
along the destructor chain, thus making it unsafe to continue to use
the object.  In 2.6, it is therefore necessary to use the global
function Unref to delete a resource.
.SH
.ps +2
Menus
.ps -2
.PP
The new Menu class is a subclass of another new class, Control.  A
control is a monoscene that allows an action to be selected and
executed.  Controls use ControlStates, subjects that coordinate a
group of related controls.  A menu contains a scene of other controls,
called the menu's \fIbody\fP.  When a menu is opened, it inserts the
body into the world with a drop shadow.  The appearance of the menu
and its body are defined by two other interactors.  For example, a
pulldown menu in a menu bar might use a message to display the string
``File''; when the pulldown menu is opened, it inserts a composition
of menu items such as ``Save'' and ``Quit'' that defines its body.
MenuItem is a subclass of Control that defines the leaves of a menu
hierarchy.  Normally, an application will provide a subclass of
MenuItem that implements the Do virtual function to carry out the
user's intent.
.PP
PopupMenu, PulldownMenu, and PullrightMenu are subclasses of Menu that
implement common menu styles.  MenuBar is a subclass of HBox that
manages the control state associated with a set of menus.  PopupMenu
has no appearance; it inserts its body into the world in response to
an input event.  PulldownMenu and PullrightMenu open the menu body
below and to the right of the menu, respectively.  These classes allow
you to use menus in an application without creating or passing control
states explicitly.
.SH
.ps +2
StringBrowser, StringChooser, FileBrowser, and FileChooser
.ps -2
.PP
StringBrowser is an interactor that provides a browsing and selection
interface to a list of strings.  StringBrowsers are often used in
dialog boxes to let the user browse and select from an unbounded set
of textual entries.
.PP
StringChooser is a dialog that manages keyboard focus between a
StringBrowser and a StringEditor to provide a convenient interface to
selecting strings.  The user can select strings either by selecting
them in the StringBrowser or by typing them in the StringEditor. Like
Dialog, StringChooser is an abstract class; programmers must derive
from StringChooser to add push buttons, etc. and to compose them into
custom arrangements.
.PP
FileBrowser is a StringBrowser that displays file names. In addition
to the standard StringBrowser constructor parameters, the FileBrowser
constructor takes a directory name as an argument.  The FileBrowser
scans the corresponding directory, appending the file names to its
list of strings.
.PP
FileChooser is a StringChooser that provides a complete dialog box
interface to selecting a file name.  The interface consists of two
lines of descriptive text, a StringEditor for entering the file name
explicitly, a FileBrowser, and two PushButtons, one for accepting and
one for canceling the dialog.
.SH
.ps +2
Other new features
.ps -2
.IP Interactor
.br
The Interactor class has two new member functions: GetCanvas returns
the Interactor's canvas, and Highlight(boolean) tells an interactor to
turn highlighting on or off if the parameter is true or false,
respectively.  Scenes implement Highlight by calling it recursively on
their components.
.IP MarginFrame
.br
A new frame subclass, MarginFrame, surrounds its component with
horizontal and vertical glue of specified sizes.
.IP Scroller
.br
The scroller behavior is now identical for all three buttons: they all
replicate the original behavior of the middle button.  Also, Scroller
no longer sets its cursor.
.IP Event
.br
We have added two new events, FocusInEvent and FocusOutEvent, to
support focus events correctly.  EnterEvent and LeaveEvent specify
pointer motion into or out of an interactor.  OnEvent and OffEvent are
obsolete, but for backward compatibility they are equivalent to
EnterEvent and LeaveEvent.
.IP Message
.br
The Message class now supports an optional argument that specifies an
amount of white space to leave around the text.  This feature is
useful for items in menus.
.IP Rubberband
.br
Several new rubberband subclasses have been added for additional kinds
of interactive feedback, including ScalingLine, ScalingLineList,
RotatingLineList, and SlidingEllipse.
.IP "StringEditor, TextEditor, TextDisplay"
.br
These classes now support horizontal scrolling.  Also, StringEditor
uses a more flexible strategy to decide when to return input control
to the caller.  TextEditor implements grab scrolling (scroll in
response to mouse motion) and rate scrolling (scroll by amount
corresponding to the differential between the current and initial
mouse positions, creating a joystick-like interface).
.IP Transformer
.br
Boolean functions such as Scaled now take an optional floating point
tolerance for comparing matrix values with a constant such as zero or
one.  New member functions have been added to transform and
inverse-transform a rectangle.
.IP Alignment and boolean types
.br
The Alignment and boolean types are now \fBunsigned\fP instead of
enumerations.  This change was necessary to allow bitfields of these
types under C++ 2.0.
.SH
.ps +2
Applications
.ps -2
.IP CC
.br
We have parameterized the CC command so that the same executable can
invoke cfront 1.2, cfront 2.0, or g++ via a run-time switch.  We have
also added run-time library support for C++ static objects to let
applications work in any compilation environment.
.IP genclass
.br
The genclass command has been reimplemented as a script, and we have
renamed it ``mkclass'' to avoid conflicting with the genclass command
used by libg++.
.IP idraw
.br
The drawing editor now provides a FileChooser dialog for opening files
and navigating through directories.  Other new features include more
intuitive landscape mode behavior, a more accurate grid, and an
enhanced Reshape tool that supports editing transformed text in place.
.IP iclass
.br
Iclass is a simple class browser that lets the user traverse C++ class
hierarchies and view the definition of a particular class.
.IP ifb
.br
Ifb displays a sample string in a font that is specified by font
family, style, and size.  Ifb writes the full X font name of the
chosen font to standard output when it exits.
.IP ifc
.br
Ifc is an interactive file chooser that lets the user
browse through directories and select a file name.
.IP logo
.br
Logo simply displays the InterViews logo (or any bitmap) in rainbow colors.

.SH
.ps +2
Configuration management
.ps -2
.PP
To simplify the file organization, the \fIconfig\fP, \fIdoc\fP, and
\fIman\fP subdirectories have been moved under \fIsrc\fP, and the
\fIbin\fP and \fIlib\fP subdirectories have been moved under
\fIinstalled\fP.  We use X11R4 imake, so you will need our version of
imake or the X11R4 version; the X11R3 version will not work.

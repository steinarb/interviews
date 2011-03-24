.nr PO 1.25in
.DA "29 November 1989"
.TL
.ps +6
Release Notes for InterViews 2.6
.ps -6
.PP
The InterViews distribution is split into two main subdirectories,
one named \fIiv\fP, which contains InterViews source and associated files, and
one named \fIg++\fP, which contains the GNU C++ compiler.
The InterViews libraries and applications do not require g++;
you can use a different C++ compiler if you wish.
You can specify the compiler and other site definitions
by modifying the file \fIiv/src/config/IVlocal.def\fP.
.PP
The file \fIiv/README\fP describes how to build InterViews.  Under
\fIiv/src\fP, the directory \fIInterViews\fP contains include files,
\fIbin\fP contains applications (each in its own subdirectory),
\fIlibInterViews\fP contains the base library classes,
\fIlibgraphic\fP contains the structured graphics library,
\fIconfig\fP contains configuration-specific files, \fIman\fP contains
troff sources for the InterViews release notes and manual pages, and
\fIdoc\fP contains LaTeX and PostScript for three papers and a
one-page overview of InterViews.  Under \fIdoc\fP, the
\fIcomposition\fP subdirectory contains the paper titled ``Composing
User Interfaces with InterViews'' that was published in the February
1989 issue of \fIComputer\fP; the \fIlibgraphic\fP subdirectory
contains the paper titled ``Applying Object-Oriented Design to
Structured Graphics'' that was published in the 1988 C++ Conference
proceedings; and the \fItoolkit\fP subdirectory contains the paper
titled ``InterViews: A C++ Graphical Interface Toolkit,'' which is a
revised version of the paper published in the 1987 C++ Workshop
proceedings.
.PP
Please address questions or comments about InterViews to
.in +5n
.nf
.sp 1
Mark Linton
Center for Integrated Systems, Room 213
Stanford University
Stanford, CA 94305
.sp 1
Internet: linton@interviews.stanford.edu

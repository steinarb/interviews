# Makefile generated by imake - do not edit!
# $XConsortium: imake.c,v 1.45.1 89/08/28 17:30:07 interran Exp $

#
# "IVImake.tmpl" - generic imake template customized for InterViews
#
# Change platform-specific parameters in the appropriate .cf files.
# Define local site-specific parameters in IVlocal.def.  You may
# need a full rebuild after changing some parameters.
#

# architecture:  MIPSEL

# platform:  $XConsortium: ultrix.cf,v 1.12 89/07/18 15:39:38 jim Exp $
# operating system:             Ultrix-32 3.0

#
# generic make variables
#

            SHELL = /bin/sh

              TOP = iv
      CURRENT_DIR = ..
              SRC = .
              GEN = Generated
                M = MIPSEL
                X = X11

             AOUT = a.out
     AFTERINSTALL = $(RM)
               AR = ar cq
  BOOTSTRAPCFLAGS =
  BOOTSTRAPCPUCMD = /bin/sh $(SCRIPTSRC)/cpu.sh $(CONFIGSRC)
               CC = CC
              CPP = /lib/cpp $(STD_CPP_DEFINES)
    PREPROCESSCMD = cc -E $(STD_CPP_DEFINES)
           DEPEND = makedepend
     DEPENDCFLAGS = $(ALLDEFINES) -I/usr/include/CC
         GENCLASS = mkclass
            IMAKE = imake
        IMAKETMPL = "IVImake.tmpl"
        IMAKEOPTS = -T $(IMAKETMPL) -I$(CONFIGSRC) -I$(XCONFIGDIR)
          INSTALL = install
               LD = CC
             LINT = lint
      LINTLIBFLAG = -C
         LINTOPTS = -axz
               LN = ln -s
             MAKE = make X=$X $(MFLAGS)
        MKDIRHIER = /bin/sh $(SCRIPTSRC)/mkdirhier.sh
               MV = mv
               CP = cp
           RANLIB = ranlib
  RANLIBINSTFLAGS =
               RM = rm -f
             TAGS = ctags
            TROFF = psroff
     STD_INCLUDES =
  STD_CPP_DEFINES =
      STD_DEFINES =
 EXTRA_LOAD_FLAGS =
  EXTRA_LIBRARIES =

     INSTPGMFLAGS = -s
     INSTBINFLAGS = -m 0755
     INSTUIDFLAGS = -m 4755
     INSTLIBFLAGS = -m 0664
     INSTINCFLAGS = -m 0444
     INSTMANFLAGS = -m 0444
     INSTDATFLAGS = -m 0444
    INSTKMEMFLAGS = -m 4755

         INCLUDES = -I$(SRC) -I$(GEN) -I$(INCLUDESRC)/Std -I$(IVSRC)

      CDEBUGFLAGS =
       ALLDEFINES = $(STD_INCLUDES) $(INCLUDES) $(STD_DEFINES) $(DEFINES)
           CFLAGS = $(CDEBUGFLAGS) $(ALLDEFINES) $(EXTRA_CFLAGS)
        LINTFLAGS = $(LINTOPTS) -DLINT $(ALLDEFINES)
          LDFLAGS = $(CDEBUGFLAGS) $(SYS_LIBRARIES) $(EXTRA_LIBRARIES)

        IMAKE_CMD = $(IMAKE) $(IMAKEOPTS) $(EXTRA_IMAKEOPTS)
           RM_CMD = $(RM) .emacs_* *.CKP *.ln *.BAK *.bak *.a *.o *..c ,* *~ \
			a.out core errs tags TAGS make.log make.out

#
# project-specific make variables
#

        USRLIBDIR = $(TOP)/installed/lib/$$CPU
           BINDIR = $(TOP)/installed/bin/$$CPU.$X
      LOCALBINDIR = $(TOP)/installed/bin/$$CPU
           INCDIR = $(TOP)/installed/include
     TEMPLATESDIR = $(INCDIR)/Templates
        CONFIGDIR = $(TOP)/installed/config
       XCONFIGDIR = $(DESTDIR)/usr/lib/X11/config
          MANPATH = $(DESTDIR)/usr/man
    MANSOURCEPATH = $(MANPATH)/man
           MANDIR = $(MANSOURCEPATH)n
        LIBMANDIR = $(MANSOURCEPATH)n
   RELATIVEMANDIR = $(TOP)/installed/man

            IVSRC = $(TOP)/src
        CONFIGSRC = $(IVSRC)/config
       INCLUDESRC = $(IVSRC)/InterViews
     TEMPLATESSRC = $(INCLUDESRC)/Templates
           BINSRC = $(IVSRC)/bin
         IMAKESRC = $(BINSRC)/imake
        SCRIPTSRC = $(BINSRC)/scripts
 LIBINTERVIEWSSRC = $(IVSRC)/libInterViews
    LIBGRAPHICSRC = $(IVSRC)/libgraphic
       LIBTEXTSRC = $(IVSRC)/libtext
           MANSRC = $(IVSRC)/man

        TEMPLATES = $(TEMPLATESSRC)
    LIBINTERVIEWS = $(LIBINTERVIEWSSRC)/$M/libInterViews$X.a
       LIBGRAPHIC = $(LIBGRAPHICSRC)/$M/libgraphic.a
          LIBTEXT = $(LIBTEXTSRC)/$M/libtext.a

#
# Make for InterViews
#

ALL = iv.World

all:: bootstrapped ${ALL}

bootstrapped:
	@echo "You must 'make bootstrap' first."
	@echo 'Make sure iv/installed/bin/$$CPU is in your path.'
	@false

bootstrap:
	cd iv; make bootstrap
	${MAKE} Makefile
	@touch bootstrapped

g++.all:
	cd g++; make Makefile; make

iv.World:
	cd iv; make World

#
# common rules for all Makefiles
#

clean::
	$(RM_CMD) \#*

Makefile::
	-@if [ -f Makefile ]; then \
		$(RM) Makefile.bak; \
		$(MV) Makefile Makefile.bak; \
	else exit 0; fi
	$(IMAKE_CMD) -DTOPDIR=$(TOP) -DCURDIR=$(CURRENT_DIR)

install::
Makefiles::

#
# dependencies generated by makedepend
#


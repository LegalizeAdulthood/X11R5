#
#	@(#)Makefile	90/11/01 50.15 SMI
#
WHOAMI=		misc
HDRSPUBLIC=	cms_grays.h cms_mono.h cmsrainbow.h cms_rgb.h sun.h \
		window_hs.h expandname.h xv_c_types.h \
		xv_i18n.h charset.h db.h
HDRSPRIVATE=	bitmask.h hashfn.h gettext.h xv_i18n_impl.h db_impl.h
CFILESLIB=	demorandom.c \
		getlogindr.c setenv.c bitmask.c hashfn.c \
		expandname.c expandpath.c bitmask.c hashfn.c drop.c \
		gettext.c ctstowcs.c xv_i18n.c wcstocts.c \
		convdup.c xv_wstol.c quark.c db_conv.c xv_path.c \
		wchar_type.c
SPCFILESLIB=	demorandom.c expandname.c

OBJS =		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)


clean:		master.clean

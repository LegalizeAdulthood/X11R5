#
#	@(#)Makefile  90/07/16 50.3 SMI
#
WHOAMI=		text
ENTITYHDRS= 	ei.h es.h ev.h ev_impl.h
MISCHDRS=	primal.h ps_impl.h finger_tbl.h
TEXTHDRS=	txt_impl.h
HDRSPRIVATE=	$(ENTITYHDRS) $(MISCHDRS) $(TEXTHDRS)
HDRSPUBLIC=	textsw.h text.h

ESCFILESLIB=	es_file.c      es_mem.c       es_util.c     es_attr.c    \
		ps_impl.c
EVCFILESLIB=	ev_display.c   ev_op_bdry.c   ev_edit.c     ev_once.c    \
		ev_attr.c      ev_field.c     ev_update.c
MISCCFILESLIB=	es_cp_file.c   ei_text.c      ei_attr.c     finger_tbl.c
TEXTCFILESLIB=	txt_data.c     txt_once.c     txt_input.c   txt_edit.c   \
		txt_menu.c     txt_disp.c     txt_event.c   txt_find.c   \
		txt_file.c     txt_sel.c      txt_selsvc.c  txt_again.c  \
		txt_getkey.c   txt_putkey.c   txt_filter.c  text.c       \
		txt_attr.c     txt_dbx.c      txt_tool.c    txt_scroll.c \
		txt_field.c    txt_caret.c    txt_search.c  txt_e_menu.c \
		txt_move.c     txt_view.c     txt_popup.c   txt_store.c  \
		txt_match.c    txt_load.c     txt_line.c    txt_incl.c   \
		txt_cb.c
		
CFILESLIB=	${TEXTCFILESLIB} ${ESCFILESLIB} ${EVCFILESLIB} ${MISCCFILESLIB}
SPCFILESLIB=	ei_text.c      es_file.c      es_mem.c      ev_once.c    \
		txt_again.c    txt_attr.c     txt_event.c   txt_field.c  \
		txt_filter.c   txt_input.c    txt_once.c    txt_search.c \
		txt_selsvc.c   ps_impl.c      txt_file.c    txt_menu.c   \
		text.c         txt_view.c     txt_popup.c   txt_incl.c   \
		txt_store.c    txt_match.c    txt_load.c    txt_line.c   \
		txt_data.c     txt_cb.c
NO_XSTR_FILES=  txt_data.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master
DBX=

.KEEP_STATE:


xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean

#
# @(#)Makefile 50.2 90/12/12
#
WHOAMI=		panel
HDRSPUBLIC=	panel.h
HDRSPRIVATE=	panel_impl.h item_impl.h p_lst_impl.h

# 		Dynamic functions initialized to panel_nullproc p_utl.c
PANEL_CFILES=	p_utl.c      p_utl_data.c   panel.c	  p_set.c      \
		p_get.c	     p_event.c 	    panel_seln.c  p_scroll.c   \
		p_select.c   p_paint.c      p_image.c     p_public.c   \
		p_compat.c   p_view.c       panel_data.c
		
ITEM_CFILES=	item.c	     item_set.c	    item_get.c    p_txt.c     \
		p_choice.c   p_btn.c        p_msg.c       p_slider.c   \
		p_list.c     item_data.c    p_btn_data.c  p_chcedata.c \
		p_txt_data.c p_msg_data.c   p_lst_data.c  p_num_txt.c  \
		p_ntx_data.c p_gauge.c      p_ambtn.c     p_amb_data.c \
		p_callback.c

SPCFILESLIB=	p_utl.c      p_txt.c        panel.c       p_btn.c      \
		p_choice.c   p_msg.c        p_slider.c    p_view.c     \
		p_list.c     p_utl_data.c   p_btn_data.c  p_chcedata.c \
		p_txt_data.c p_msg_data.c   p_lst_data.c  item_data.c  \
		p_num_txt.c  p_ntx_data.c   p_gauge.c	  p_ambtn.c    \
		p_amb_data.c
		
NO_XSTR_FILES=	p_utl_data.c p_btn_data.c   p_chcedata.c  p_txt_data.c \
		p_msg_data.c p_lst_data.c   item_data.c   panel_data.c \
		p_ntx_data.c p_amb_data.c

CFILESLIB=      ${PANEL_CFILES} ${ITEM_CFILES}

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:


xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean

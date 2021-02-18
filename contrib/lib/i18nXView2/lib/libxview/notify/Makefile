#
# @(#)Makefile 20.13 90/02/26
#
WHOAMI=	notify
HDRSPUBLIC=notify.h
HDRSPRIVATE=ndet.h ndis.h nint.h ntfy.h ultrix_cpt.h ntfy_ctbl.h
#	Can change ndis_default_scheduler in ndisdispch.c
#	Can change nint_destroy_interposer in nintideath.c
CFILESLIB_AR= \
	ndet_auto.c	ndetgitimr.c	ndetpevent.c	ndet_s_out.c \
	ndet_event.c	ndet_g_out.c	ndet_read.c	ndet_s_pri.c \
	ndet_fcntl.c	ndet_g_pri.c	ndetremove.c	ndet_s_sig.c \
	ndet_fd.c	ndet_g_sig.c	ndetsdeath.c	ndets_wait.c \
	ndetgdeath.c	ndetg_wait.c	ndetsevent.c	ndetselect.c \
	ndetgevent.c	ndetgetfnc.c	ndetsexcep.c	ndet_sig.c   \
	ndetgexcep.c	ndetitimer.c	ndet_s_fd.c	ndet_value.c \
	ndet_g_fd.c	ndet_loop.c	ndet_s_in.c	ndet_wait.c  \
	ndet_g_in.c	ndetpdeath.c	ndetsitimr.c	ndet_death.c \
	ndet_die.c	ndet_veto.c	ndet_dodis.c	ndet_nodis.c \
	ndetsingle.c 						     \
	ndis_d_pri.c	ndisdsched.c	ndisgsched.c	ndisssched.c \
	ndisd_wait.c						     \
	ntfyclient.c	ntfy_cond.c	ntfy_debug.c	ntfy_list.c  \
	ntfy_node.c	ntfyprotec.c	ntfyperror.c	ntfy_dump.c  \
	sys_select.c	sys_read.c	sys_fcntl.c 		     \
	nint_copy.c	nint_i_sig.c	nint_n_out.c	nintritimr.c \
	nint_get.c	ninti_wait.c	nint_n_sig.c	nint_r_out.c \
	nint_inter.c	nintn_wait.c	nint_r_sig.c		     \
	nintievent.c	nintndeath.c	nint_next.c	nintr_wait.c \
	nintiexcpt.c	nintnevent.c	nintrdeath.c	nintremove.c \
	nint_i_fd.c	nintnexcpt.c	nintrevent.c	nint_set.c   \
	nint_i_in.c	nint_n_fd.c	nintrexcpt.c	nint_stack.c \
	nintiitimr.c	nint_n_in.c	nint_r_fd.c		     \
	nint_i_out.c	nintnitimr.c	nint_r_in.c	ntfy_fd_op.c \
	notifydata.c	ntfy_ctbl.c
CFILESLIB_NO_AR= \
	ndisdispch.c	nintideath.c	nint_stack.c		     \
	ntfyclient.c	ntfyprotec.c 	ntfy_node.c 		     \
	ndet_auto.c 	ndet_loop.c	ndet_read.c		     \
	ndetselect.c 	notifydata.c
SPCFILESLIB= \
	ndisdispch.c	nintideath.c	nint_stack.c		     \
	ntfyclient.c	ntfyprotec.c 	ntfy_node.c		     \
	ndet_auto.c 	ndet_loop.c	ndet_read.c		     \
	ndetselect.c 	notifydata.c
	
CFILESLIB= ${CFILESLIB_AR} ${CFILESLIB_NO_AR}
	
NO_XSTR_FILES= notifydata.c
	
OBJS = $(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%)

all :		xall

include ../Makefile.master

.KEEP_STATE:


xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean

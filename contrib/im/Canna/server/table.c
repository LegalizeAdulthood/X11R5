static char sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) table.c 1.5 90/11/29 18:05:40";
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/server/RCS/table.c,v 1.4 91/05/22 16:32:33 satoko Exp $";

/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

#include "IRproto.h"

extern int
    ir_error() ,
    ir_initialize(), ir_finalize(), ir_create_context(),
    ir_duplicate_context(), ir_close_context(), ir_get_yomi(),
    ir_dictionary_list(), ir_define_dic(), ir_delete_dic(), ir_get_dir_list(),
    ir_mount_dictionary(), ir_umount_dictionary(), ir_mount_list(), ir_set_dic_path(),
    ir_convert(), ir_convert_end(), ir_get_kanjilist(), ir_resize(),
    ir_rmount_dictionary(), ir_store_yomi(), ir_get_lex(), ir_get_stat(),
    ir_server_stat(), ir_server_stat2(), ir_host_ctl(), ir_query_extension() ;

#ifdef EXTENSION
extern int
    ir_list_dictionary(), ir_create_dictionary(), ir_remove_dictionary(),
    ir_rename_dictionary(), ir_get_word_text_dic() ;
#endif /* EXTENSION */

extern int
    ProcReq0(),ProcReq1(),ProcReq2(),ProcReq3(),ProcReq4(),ProcReq5(),
    ProcReq6(),ProcReq7(),ProcReq8(),ProcReq9(),ProcReq10(),ProcReq11(),
    ProcReq12() ;

int (* Vecter[][2]) () =
{
/* 0x00 */	{ ir_error,		   ProcReq0 },
/* 0x01 */	{ ir_initialize,	   ProcReq2 },
/* 0x02 */	{ ir_finalize,		   ProcReq0 },
/* 0x03 */	{ ir_create_context,	   ProcReq0 },
/* 0x04 */	{ ir_duplicate_context,    ProcReq1 },	
/* 0x05 */	{ ir_close_context,	   ProcReq1 },	
/* 0x06 */	{ ir_dictionary_list,	   ProcReq3 },	
/* 0x07 */	{ ir_get_yomi,		   ProcReq5 },	
/* 0x08 */	{ ir_define_dic,	   ProcReq7 },	
/* 0x09 */	{ ir_delete_dic,	   ProcReq7 },	
/* 0x0a */	{ ir_set_dic_path,	   ProcReq8 },	
/* 0x0b */	{ ir_get_dir_list,	   ProcReq3 },	
/* 0x0c */	{ ir_mount_dictionary,	   ProcReq8 },	
/* 0x0d */	{ ir_umount_dictionary,    ProcReq8 },	
/* 0x0e */	{ ir_rmount_dictionary,    ProcReq9 },	
/* 0x0f */	{ ir_mount_list,	   ProcReq3 },	
/* 0x10 */	{ ir_convert,		   ProcReq8 },	
/* 0x11 */	{ ir_convert_end,	   ProcReq4 },	
/* 0x12 */	{ ir_get_kanjilist,	   ProcReq5 },	
/* 0x13 */	{ ir_resize,		   ProcReq5 },	
/* 0x14 */	{ ir_store_yomi,	   ProcReq9 },	
/* 0x15 */	{ ir_get_lex,		   ProcReq11 }, 
/* 0x16 */	{ ir_get_stat,		   ProcReq5 },	
/* 0x17 */	{ ir_server_stat,	   ProcReq0 },		
/* 0x18 */	{ ir_server_stat2,	   ProcReq0 },		
/* 0x19 */	{ ir_host_ctl,		   ProcReq0 },
/* 0x1a */	{ ir_query_extension,	   ProcReq12 }

} ;

char *ExtensionName[][2] = {
    /* Request Name		Start Protocol Number */					
#ifdef EXTENSION
    { REMOTE_DIC_UTIL,		"65536" }, /* 0x10000 */
#endif /* EXTENSION */
    { "",		      "" }
} ;

#ifdef EXTENSION
int (* ExtensionVecter[][2]) () =
{
/* 0x00 */	{ ir_list_dictionary,	   ProcReq9 },
/* 0x01 */	{ ir_create_dictionary,    ProcReq8 },
/* 0x02 */	{ ir_remove_dictionary,   ProcReq8 },
/* 0x03 */	{ ir_rename_dictionary,    ProcReq10 },
/* 0x04 */	{ ir_get_word_text_dic,    ProcReq10 }
} ;
#endif /* EXTENSION */


char *ProtoName[] = {
    "IR_INIT",
    "IR_FIN",	
    "IR_CRE_CON",	
    "IR_DUP_CON",
    "IR_CLO_CON",
    "IR_DIC_LIST",	
    "IR_GET_YOMI",
    "IR_DEF_DIC",	
    "IR_UNDEF_DIC",	
    "IR_DIC_PATH",	
    "IR_DIR_LIST",	
    "IR_MNT_DIC",	
    "IR_UMNT_DIC",
    "IR_RMNT_DIC",
    "IR_MNT_LIST",
    "IR_CONVERT",	
    "IR_CONV_END",	
    "IR_KAN_LST",	
    "IR_RESIZE",
    "IR_STO_YOMI",		
    "IR_GET_LEX",	
    "IR_GET_STA",	
    "IR_SER_STAT",	
    "IR_SER_STAT2",	
    "IR_HOST_CTL",
    "IR_QUREY_EXT"
} ;			

#ifdef DEBUG
char *DebugProc[][2] = {
    { "ir_null",		      "ProcReq0" } ,
    { "ir_initialize",		      "ProcReq2" } ,
    { "ir_finalize",		      "ProcReq0" } ,
    { "ir_create_context",	      "ProcReq0" } ,
    { "ir_duplicate_context",	      "ProcReq1" } ,
    { "ir_close_context",	      "ProcReq1" } ,
    { "ir_dictionary_list",	      "ProcReq3" } ,
    { "ir_get_yomi",		      "ProcReq5" } ,
    { "ir_define_dic",		      "ProcReq7" } ,
    { "ir_delete_dic",		      "ProcReq7" } ,
    { "ir_set_dic_path",	      "ProcReq8" } ,
    { "ir_get_dir_list",	      "ProcReq3" } ,
    { "ir_mount_dictionary",	      "ProcReq8" } ,
    { "ir_umount_dictionary",	      "ProcReq8" } ,
    { "ir_rmount_dictionary",	      "ProcReq9" } ,
    { "ir_mount_list",		      "ProcReq3" } ,
    { "ir_convert",		      "ProcReq8" } ,
    { "ir_convert_end", 	      "ProcReq4" } ,
    { "ir_get_kanjilist",	      "ProcReq5" } ,
    { "ir_resize",		      "ProcReq5" } ,
    { "ir_store_yomi",		      "ProcReq9" } ,
    { "ir_get_lex",		      "ProcReq11"} ,
    { "ir_get_stat",		      "ProcReq5" } ,
    { "ir_server_stat", 	      "ProcReq0" } ,
    { "ir_server_stat2",	      "ProcReq0" } ,
    { "ir_host_ctl",		      "ProcReq0" } ,
    { "ir_query_extension",	      "ProcReq12" }
} ;			
#endif

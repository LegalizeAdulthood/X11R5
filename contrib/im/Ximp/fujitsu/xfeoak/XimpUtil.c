/* @(#)XimpUtil.c	2.2 91/07/05 17:03:09 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

/*  Xfeoak : Input Server (kana-kanji covertion front end)	*/
/*         : Version 2.0					*/

#ifndef lint
static char sccs_id[] = "@(#)XimpUtil.c	2.2 91/07/05 17:03:09 FUJITSU LIMITED.";
#endif

#include "Xfeoak.h"

typedef struct  _FXimp_ClientRec {
	FXimp_Client		client;
	Ximp_PreeditPropRec 	preedit;
	Ximp_StatusPropRec 	status;
	FXimpFont   		preedit_font;
	FXimpFont   		status_font;
	unsigned short		output_buff[FXIMP_LINEBUFF_LENGTH];
	unsigned short		input_buff[ FXIMP_LINEBUFF_LENGTH];
	} FXimp_ClientRec;

FXimp_ClientRec	*fximp_table = 0;
int		fximp_table_max_no;
int		fximp_table_used_max_no;
int		fximp_table_used_no;

int
InitialClientTable( table_no )
     int  table_no;
     {
     int  i, size;

     fximp_table_max_no = table_no;
     if( fximp_table ) {
	 return(-1);
	 }
     size = sizeof(FXimp_ClientRec) * fximp_table_max_no;
     fximp_table = (FXimp_ClientRec *)malloc( size);
     if(!fximp_table) {
	 ErrorMessage( 070, size );
	 return(-1);
	 }
     bzero(fximp_table, size);
     fximp_table_used_max_no =  0;
     fximp_table_used_no = 0;
     return(0);
     }

int
ExitClientTable()
     {
     if(!fximp_table ) {
	 return(-1);
	 }
     free(fximp_table);
     fximp_table = 0;
     fximp_table_max_no = 0;
     return(0);
     }

FXimp_Client *
NewClient(window)
     Window  window;
     {
     int  i;
     FXimp_ClientRec	*client_rec;
     if(fximp_table_used_no >= fximp_table_max_no) {
	 return((FXimp_Client *)0);
	 }
     client_rec = fximp_table;
     for( i=0; i<fximp_table_max_no; i++, client_rec++) {
	 if( client_rec->client.window == 0) {
	     if( i >= fximp_table_used_max_no ) fximp_table_used_max_no = i+1;
	     fximp_table_used_no++;
	     bzero( client_rec, sizeof(FXimp_ClientRec));
	     client_rec->client.window 	     = window;
	     client_rec->client.preedit	     = &(client_rec->preedit);
	     client_rec->client.status 	     = &(client_rec->status);
	     client_rec->client.preedit_font = &(client_rec->preedit_font);
	     client_rec->client.status_font  = &(client_rec->status_font);
	     client_rec->client.output_buff  = client_rec->output_buff;
	     client_rec->client.input_buff   = client_rec->input_buff;
	     return((FXimp_Client *)&(client_rec->client));
	     }
	 }
     return((FXimp_Client *)0);
     }

int 
DeleteClient(icid)
     FXimp_ICID  icid;
     {
     int  i;
     if( icid == (FXimp_ICID)current_client ) {
	 current_client = root_client;
	 current_preedit_font = root_client->preedit_font;
	 }

     for (i=0; i<fximp_table_used_max_no; i++) {
	 if( (FXimp_ICID)&(fximp_table[i].client) == icid ) {
	     if( i == fximp_table_used_max_no ) fximp_table_used_max_no--;
	     fximp_table_used_no--;
	     fximp_table[i].client.window = (Window)0;
	     return(0);
	     }
	 }
     return(-1);
     }

FXimp_Client *
FindClientByICID(icid)
     FXimp_ICID  icid;
     {
     int  i;
     for (i=0; i<fximp_table_used_max_no; i++) {
	 if( (FXimp_ICID)&(fximp_table[i].client) == icid ) {
	     return((FXimp_Client *)&(fximp_table[i].client));
	     }
	 }
     return((FXimp_Client *)0);	/* error return */
     }

FXimp_Client *
FindClient(window)
     Window  window;
     {
     int  i;
     for (i=0; i<fximp_table_used_max_no; i++) {
	 if(fximp_table[i].client.window == window) {
	     return((FXimp_Client *)&(fximp_table[i].client));
	     }
	 }
     return((FXimp_Client *)0);	/* error return */
     }

FXimp_Client *
FindClientByFocusWindow(window)
     Window  window;
     {
     int  i;
     for (i=0; i<fximp_table_used_max_no; i++) {
	 if(fximp_table[i].client.focus_window == window) {
	     return((FXimp_Client *)&(fximp_table[i].client));
	     }
	 }
     return((FXimp_Client *)0);	/* error return */
     }

FXimp_Client *
FindClientByPreeditOrStatusWindow(window)
     Window  window;
     {
     int  i;
     for (i=0; i<fximp_table_used_max_no; i++) {
	 if(fximp_table[i].client.preedit_window == window
	 || fximp_table[i].client.status_window == window) {
	     return((FXimp_Client *)&(fximp_table[i].client));
	     }
	 }
     return((FXimp_Client *)0);	/* error return */
     }

int
SetCurrentClient(window)
    Window  window;
    {
    FXimp_Client	     *ximp_client;
    ximp_client = FindClientByFocusWindow(window);
    if( !ximp_client ) {
        ximp_client = FindClient(window);
        if( !ximp_client ) {
	    WarningMessage( 191 );
            current_client = root_client;
            current_preedit_font = current_client->preedit_font;
	    return(-1);
            }
        }
    current_client = ximp_client;
    current_preedit_font = current_client->preedit_font;
    return(0);
    }

DestroyCurrentClient(event)
    XEvent        *event;
    {
    if( current_client->prot_type == FPRT_XIM ) {
	current_client->focus_window = 0;
	XimpDestroyClient( current_client, 1 );
	}
#ifdef XJP
    else {
	current_client->focus_window = 0;
	XjpDestroyClient( current_client, 1 );
	}
#endif XJP
    }


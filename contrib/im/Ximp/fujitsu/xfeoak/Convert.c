/* @(#)Convert.c	2.4 91/09/17 14:18:54 FUJITSU LIMITED. */
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
static char sccs_id[] = "@(#)Convert.c	2.4 91/09/17 14:18:54 FUJITSU LIMITED.";
#endif

#include <ctype.h>			/* islower	*/
#ifndef	_toupper
#define _toupper(c)    ((c)-'a'+'A')
#endif
#include "sxc/slcode.h"
#include "Xfeoak.h"
#include "KanjiTbl.h"
#include "Henkan.h"

int
fj_PutKana( string, length)
	unsigned char	*string;
	int 		length;
	{
	Kakutei(0);
	CancelRomaji();
	for(; length>0; length--) {
	    SetOutputBuff( *string++ );
	    }
	DisplayOutputBuff();
	DisplayCursor();
	return( 0);
	}

int
fj_PutAscii( string, length)
	unsigned char	*string;
	int		length;
	{
	Kakutei(0);
	if( (current_client->siftmode & FS_KANA )
	&&  (current_client->romaji_mode) )
	    RomajiHenkan( string );	
	else {
	    for(; length>0; length--) {
	        SetOutputBuff( *string++ );
		}
	    }
	DisplayOutputBuff();
	DisplayCursor();
	return( 0);
	}

Static int
RomajiHenkan( string )
	unsigned char *string;
	{
	unsigned char	ac, ec;
	int		num, j;

	if( current_client->slcrtok_input_no == 0) {
	    current_client->romaji_colums = current_client->current_colums;
	    bzero( current_client->slcrtok_inbuf,  4 );
	    bzero( current_client->slcrtok_outbuf, 4 );
	    if( current_client->last_colums > current_client->current_colums ) {
		CopyUshort( current_client->input_buff,
		  current_client->output_buff, current_client->last_colums); 
		}
	    }
	while( ac= *string++ ) {
	    if( islower(ac) ) ac = _toupper( (int)ac); 
	    ec = slcatoe( ac, C_JPNKAT);
	    num = slcrtok( ec, current_client->slcrtok_inbuf,
			       current_client->slcrtok_outbuf,
			  (int)current_client->slcrtok_nhtype);
	    if( num != 0 ) {
		if( num < 0 ) num = -num;
		if( (int)current_client->slcrtok_input_no >= num ) {
		    int old_num = (int)current_client->slcrtok_input_no;
		    if( current_client->romaji_colums + old_num
		     == current_client->last_colums )
			current_client->last_colums = current_client->romaji_colums;
		    else {	
			int	num2 = current_client->romaji_colums + num;
			int	length  = old_num - num;
			if( num > 1 ) {
			    num2--;
			    length++;
			    }
			CopyUshort( &current_client->output_buff[num2],
			  	    &current_client->input_buff[num2], length );
		        }
		    }
		current_client->current_colums = current_client->romaji_colums;
		for( j=0; j<num; j++) {
		    ec = current_client->slcrtok_outbuf[j];
		    ac = slcetoa( ec, C_JPNKAT);	
		    if( ac > 0)
		        SetOutputBuff( ac );	
		    }
	        bzero( current_client->slcrtok_outbuf, 4 );
	        current_client->romaji_colums = current_client->current_colums; 
		ec = current_client->slcrtok_inbuf[0];
		if( ec == 0 ) {
		    current_client->slcrtok_input_no = 0;
		    }
		else {
		    current_client->slcrtok_input_no = 1;	
		    ac = slcetoa( ec, C_JPNKAT);
		    if( ac > 0)
		        SetOutputBuff( ac );
		    }
		}
	    else {
	        current_client->slcrtok_input_no++;
		ac = slcetoa( ec, C_JPNKAT);
		if( ac > 0)
		    SetOutputBuff( ac ); 
		}
	    }
	return( 0);
	}

Static int
CancelRomaji()
	{
	if( current_client->slcrtok_input_no != 0) {
	    current_client->slcrtok_input_no = 0;
	    bzero( current_client->slcrtok_inbuf, 4);
	    bzero( current_client->slcrtok_outbuf, 4);
	    }
	current_client->romaji_colums = current_client->last_colums;
	return( 0);
	}

Static int
FlashRomaji()
	{
	unsigned char	ac, ec = 0xfe;
	int	num, j;
	if( current_client->slcrtok_input_no != 0) {
	    num = slcrtok( ec, current_client->slcrtok_inbuf,
			       current_client->slcrtok_outbuf,
			  (int)current_client->slcrtok_nhtype);
	    if( num != 0 ) {
		if( num < 0 ) num = -num;
		num--;
		if( (int)current_client->slcrtok_input_no >= num ) {
		    int old_num = current_client->slcrtok_input_no;
		    if( current_client->romaji_colums + old_num
		     == current_client->last_colums )
			current_client->last_colums = current_client->romaji_colums;
		    else {	
			int	num2 = current_client->romaji_colums + num;
			int	length  = old_num - num;
			if( num > 1 ) {
			    num2--;
			    length++;
			    }
			CopyUshort( &current_client->output_buff[num2],
				    &current_client->input_buff[num2], length );
		        }
		    }
		current_client->current_colums = current_client->romaji_colums;
		for( j=0; j<num; j++) {
		    ec = current_client->slcrtok_outbuf[j];
		    ac = slcetoa( ec, C_JPNKAT );	
		    if( ac > 0)
		        SetOutputBuff( ac );	
		    }
		}
	    current_client->slcrtok_input_no = 0;
	    bzero( current_client->slcrtok_inbuf,  4 );
	    bzero( current_client->slcrtok_outbuf, 4 );
	    }
	current_client->romaji_colums = current_client->last_colums;
	return( 0);
	}

Static int
SetOutputBuff( ac )
	unsigned char ac;
	{
	ushort	buff, *bwc;
	int	cnv_kata = 0xa5a1 - 0xa4a1;
	int	kana = ac & 0x80;
	int	CheckKanaDakuten();

	if( current_client->current_colums >= FXIMP_LINEBUFF_LENGTH - 2 ) {
		Beep();
		return(-1);
		}
	ac &= 0xff;
	if( current_client->hankakumode == 1 ) {
	    if( (kana != 0) && (current_client->siftmode == FS_HIRAGANA)) 
	        buff = Hankaku_To_ZenkakuAsciiHiragana( ac ); 
	    else
	        buff = ac;
	    }
	else {
	    buff = Hankaku_To_ZenkakuAsciiHiragana( ac );     
	    if( (kana != 0) && (current_client->siftmode == FS_KATAKANA)) 
		if( ac > 0xa5 && ac < 0xde && ac != 0xb0 )
	    	    buff += cnv_kata;
	    }
	current_client->input_flag = FINF_MODY;
	if( current_client->current_colums > 0 ) {
	    if( buff == 0xA1AB || 
	        buff == 0xA1AC    ) {
	    	bwc = &(current_client->output_buff[current_client->current_colums-1]);
	        if( CheckKanaDakuten(bwc,buff) == 0 )
	    	    return(0);
		}
	    }
	current_client->output_buff[current_client->current_colums++] = buff;
	if(current_client->current_colums > current_client->last_colums) {
	    current_client->last_colums = current_client->current_colums;
	    current_client->input_flag = FINF_ADD;
	    }
	return( 0);
	}

int
fj_Muhenkan()
	{
	int		num, rest_colums;
	ushort		*yomi, *out_buff, save;
	static int	muhenkan_count = 0;

	if( current_client->current_colums <= 0 )
	    return(0);

	yomi     = current_client->input_buff;
	out_buff = current_client->output_buff;
	if( current_client->convert_count == 0 ) {
	    FlashRomaji();
	    CopyUshort( yomi, out_buff, current_client->last_colums );
	    current_client->input_colums = current_client->last_colums;
	    current_client->convert_colums = current_client->current_colums;
	    muhenkan_count = 0;
	    }
	else
	if( current_client->convert_count > 0 ) {
	    muhenkan_count = 0;
	    Dic_HenkanCancel();
	    }
	current_client->convert_count = -1;
	
	save = yomi[current_client->convert_colums];
	yomi[current_client->convert_colums] = 0;
	if( muhenkan_count == 0 ) {
	    muhenkan_count = 1;
	    num = CopyUshort( out_buff, yomi, FXIMP_LINEBUFF_LENGTH );
	    }
	else if( muhenkan_count == 1 ) {
	    muhenkan_count = 2;
	    num = Hiragana_To_Katakana( out_buff, yomi, FXIMP_LINEBUFF_LENGTH );
	    }
	else {
	    muhenkan_count = 0;
	    num = ZenkakuAsciiKana_To_Hankaku( out_buff, yomi, FXIMP_LINEBUFF_LENGTH, 1 );
	    }
	yomi[current_client->convert_colums] = save;
	
	rest_colums = current_client->input_colums - current_client->convert_colums;
	CopyUshort( &out_buff[num], &yomi[current_client->convert_colums], rest_colums );
	current_client->current_colums = num;
	current_client->reverse_colums = num;
	current_client->last_colums = num + rest_colums;
	current_client->input_flag = FINF_MODY;
	DisplayOutputBuff();
	DisplayCursor();       
	return( 0);
	}

int
fj_Henkan()
	{
	return Henkan(FKK_HENKAN);
	}

int
fj_Maekouho()
	{
	return Henkan(FKK_MAE);
	}

Static int
Henkan(fkk_type)
	unsigned char	fkk_type;
	{
	int	num;
	ushort	*yomi, *out_buff;

	if( current_client->current_colums <= 0 )
	    return(0);

	yomi     = current_client->input_buff;
	out_buff = current_client->output_buff;
	if( current_client->convert_count == 0 ) {
	    FlashRomaji();
	    CopyUshort( yomi, out_buff, current_client->last_colums );
	    current_client->input_colums = current_client->last_colums;
	    current_client->convert_colums = current_client->current_colums;
	    }
	else if( current_client->convert_count < 0 )
	    current_client->convert_count = 0;
	num = Dic_Henkan( fkk_type, current_client->convert_count, yomi,
			  current_client->convert_colums,
			  out_buff, FXIMP_LINEBUFF_LENGTH);
	if( num == -1 ) {
	    Muhenkan();
	    current_client->current_colums = 0;
	    Beep();
	    }
	else {
	    int   rest_colums;
	    if( current_client->hankakumode ) {
		current_client->output_buff[num] = 0;
		num = ZenkakuAsciiKana_To_Hankaku( current_client->output_buff,
			current_client->output_buff, FXIMP_LINEBUFF_LENGTH, 0);
		}
	    rest_colums = current_client->input_colums - current_client->convert_colums;
	    CopyUshort( &out_buff[num], &yomi[current_client->convert_colums], rest_colums );
	    current_client->current_colums = num;
	    current_client->reverse_colums = num;
	    current_client->last_colums = num + rest_colums;
	    if( fkk_type == FKK_HENKAN )
	        current_client->convert_count++;
	    else if( fkk_type == FKK_MAE ) {
		if( --(current_client->convert_count) <= 0)
		    current_client->convert_count = 1;
	        }
	    else    current_client->convert_count = 1;    
	    current_client->input_flag = FINF_MODY;
	    }
	DisplayOutputBuff();
	DisplayCursor();	
	return( 0);
	}

int
Muhenkan()
	{
	Dic_HenkanCancel();
	CopyUshort( current_client->output_buff, current_client->input_buff,
			current_client->input_colums );
	current_client->last_colums = current_client->input_colums;
	current_client->current_colums = current_client->convert_colums;
	current_client->reverse_colums = 0;
	current_client->input_colums = 0;
	current_client->convert_count = 0;
	current_client->input_flag = FINF_MODY;
	return( 0);
	}

int
fj_HenkanCancel()
	{
	Dic_HenkanCancel();

	if( current_client->convert_count != 0 ) {
	    Muhenkan();
	    current_client->current_colums = 0;
	    }
	else {
	    HenkanCancel();
	    }
	DisplayOutputBuff();
	DisplayCursor();
	return( 0);
	}

Static int
HenkanCancel()
	{
	current_client->last_colums = 0;
	current_client->current_colums = 0;
	current_client->input_colums = 0;
	current_client->convert_count = 0;
	current_client->reverse_colums = 0;
	CancelRomaji();
	current_client->input_flag = FINF_MODY;
	return( 0);
	}

int
fj_exec()
	{
	DisplayCursor();
	if( current_client->slcrtok_input_no > 0 ) {
	    Beep();
	    return(-1);
	    }
	Kakutei(1);
	DisplayCursor();
	return( 0);
	}

Static int
Kakutei(disp_sw)
	int	disp_sw;
	{
	int	length;
	if( current_client->convert_count != 0) {
	    current_client->convert_count = 0;
	    if( current_client != root_client ) {
		SendBuffByLength( current_client->window,
				  current_client->output_buff,
				  current_client->reverse_colums );
		sleep(0);
		}
	    length = current_client->last_colums - current_client->reverse_colums;
	    current_client->last_colums = length;
	    CopyUshort( &(current_client->output_buff[0]),
		     &(current_client->output_buff[current_client->reverse_colums]), length );
	    current_client->reverse_colums = 0;
	    current_client->current_colums = 0;
	    current_client->input_flag = FINF_MODY;
	    if( disp_sw )
	        DisplayOutputBuff();
	    }
	return( 0);
	}

int
fj_HenkanReset()
	{
	Fj_InitialMode();
	DisplayCursor();
	CancelRomaji();
	current_client->reverse_colums = current_client->last_colums; 
	current_client->convert_count = 1;	
	if( current_client != root_client
	&&  current_client->prot_type == FPRT_XIM ) {
	    XimpResetSendBuffByLength( current_client->window,
				  current_client->output_buff,
				  current_client->reverse_colums );
	    sleep(0);
	    }
	current_client->last_colums = 0;
	current_client->reverse_colums = 0;
	current_client->current_colums = 0;
	current_client->input_flag = FINF_MODY;
	DisplayOutputBuff();
	DisplayCursor();
	return( 0);
	}

int
fj_CursorMove(flag)
	int	flag;
	{
	DisplayCursor();
	CancelRomaji();
	Kakutei(1);
	switch( flag ) {
	case KS_TOP:
	    current_client->current_colums = current_client->reverse_colums;
	    break;
	case KS_BOTTOM:
	    current_client->current_colums = current_client->last_colums;
	    break;
	case KS_FOWARD:
	    current_client->current_colums++;
	    if( current_client->current_colums > current_client->last_colums ) 
	        current_client->current_colums = current_client->last_colums;
	    break;
	case KS_BACKWARD:
	    current_client->current_colums--;
	    if( current_client->current_colums < current_client->reverse_colums ) 
	        current_client->current_colums = current_client->reverse_colums;
	    break;
	    }
	DisplayCursor();
	return( 0);
	}

int
fj_DeleteChar()
	{
	int length, j;
	Kakutei(0);
	if(current_client->last_colums > 0 ) { 
	    if( current_client->current_colums == current_client->last_colums ) {
	        Beep();
	        return(-1);
	        }
	    for(j=current_client->current_colums; j<current_client->last_colums; j++ ) 
	        current_client->output_buff[j] = current_client->output_buff[j+1];
	    current_client->last_colums--;
	    if(current_client->current_colums > current_client->last_colums)
	        current_client->current_colums--;
	    current_client->input_flag = FINF_MODY;
	    DisplayOutputBuff();
	    DisplayCursor();
	    }
	return( 0);
	}

int
fj_InsertChar()
	{
	int	j;
	Kakutei(0);
	for(j=current_client->last_colums-1; j>=current_client->current_colums; j-- ) 
	    current_client->output_buff[j+1] = current_client->output_buff[j];
	current_client->output_buff[current_client->current_colums]
	  = Hankaku_To_ZenkakuAsciiHiragana(' ');
	if( current_client->last_colums < FXIMP_LINEBUFF_LENGTH - 2 )
	    current_client->last_colums++;
	current_client->input_flag = FINF_MODY;
	DisplayOutputBuff();
	DisplayCursor();
	return( 0);
	}

int
Beep()
	{
	if(!no_beep)
	    XBell(display, 50);
	return( 0);
	}

/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSave.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSave.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
/*
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSave.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
*/
#endif

typedef unsigned char io86reg ;		/* 8 bit hardware registers */
typedef short int IO_Address ;		/* 64K Intel-Style I/O space */

typedef unsigned char DAC_TABLE[768] ;

struct pli_video_hardware_state {
	/* Address locations			READ	--	WRITE */
	/* General Registers */
	io86reg Misc_Output_Reg ;	/*	03CC	--	03C2 */
	io86reg Input_Status_0 ;	/*	03C2	--	XXXX */
	io86reg Input_Status_1 ;	/*	03?A	--	XXXX */
	io86reg Feature_Control ;	/*	03?C	--	03CA */
	io86reg Video_Enable ;		/*	03C3	--	SAME */

	/* Attribute Registers  03C0 & 03C1 */
	io86reg Attr_Addr_Reg ;		/*	03C0	--	SAME */
	/* io86reg ??????? */		/*	03C0	--	SAME */
	io86reg Palette[16] ;		/*	Attr_Addr_Reg == 00 - 0F */
	io86reg Attr_Mode ;		/*	Attr_Addr_Reg == 10 */
	io86reg Overscan_Color ;	/*	Attr_Addr_Reg == 11 */
	io86reg Color_Plane_En ;	/*	Attr_Addr_Reg == 12 */
	io86reg Horiz_PEL_Pan ;		/*	Attr_Addr_Reg == 13 */
	io86reg Color_Select ;		/*	Attr_Addr_Reg == 14 */

	/* Crt Controller Registers  03?4 & 03?5 */
	io86reg Index_Reg ;		/*	03?4	--	SAME */
	/* io86reg ??????? */		/*	03?5	--	SAME */
	io86reg Horiz_Total ;		/*	Index_Reg == 00   */
	io86reg Horiz_End ;		/*	Index_Reg == 01   */
	io86reg H_Blank_Start ;		/*	Index_Reg == 02   */
	io86reg H_Blank_End ;		/*	Index_Reg == 03   */
	io86reg H_Retrace_Start ;	/*	Index_Reg == 04   */
	io86reg H_Retrace_End ;		/*	Index_Reg == 05   */
	io86reg Vert_Total ;		/*	Index_Reg == 06   */
	io86reg Overflow ;		/*	Index_Reg == 07   */
	io86reg Preset_Row_Scan ;	/*	Index_Reg == 08   */
	io86reg Max_Scan_Line ;		/*	Index_Reg == 09   */
	io86reg Cursor_Start ;		/*	Index_Reg == 0A   */
	io86reg Cursor_End ;		/*	Index_Reg == 0B   */
	io86reg Start_Addr_Hi ;		/*	Index_Reg == 0C   */
	io86reg Start_Addr_Lo ;		/*	Index_Reg == 0D   */
	io86reg Cursor_Loc_Hi ;		/*	Index_Reg == 0E   */
	io86reg Cursor_Loc_Lo ;		/*	Index_Reg == 0F   */
	io86reg V_Retrace_Start ;	/*	Index_Reg == 10   */
	io86reg V_Retrace_End ;		/*	Index_Reg == 11   */
	io86reg V_Display_End ;		/*	Index_Reg == 12   */
	io86reg Underline_Loc ;		/*	Index_Reg == 13   */
	io86reg Offset ;		/*	Index_Reg == 14   */
	io86reg V_Blank_Start ;		/*	Index_Reg == 15   */
	io86reg V_Blank_End ;		/*	Index_Reg == 16   */
	io86reg CRTC_Mode ;		/*	Index_Reg == 17   */
	io86reg Line_Compare ;		/*	Index_Reg == 18   */
	io86reg Data_Latches[4] ;	/*	Index_Reg == 22   */
	io86reg Attr_Ctlr_Toggle ;	/*	Index_Reg == 24   */

	/* Sequencer Registers  03C4 & 03C5 */
	io86reg Seq_Addr_Reg ;		/*	03C4	--	SAME */
	/* io86reg ??????? */		/*	03?5	--	SAME */
	io86reg Seq_Reset ;		/*	Seq_Addr_Reg == 00   */
	io86reg Clock_Mode ;		/*	Seq_Addr_Reg == 01   */
	io86reg Mask_Map ;		/*	Seq_Addr_Reg == 02   */
	io86reg Char_Map_Select ;	/*	Seq_Addr_Reg == 03   */
	io86reg Memory_Mode ;		/*	Seq_Addr_Reg == 04   */

	/* Graphics Registers  03CE & 03CF */
	io86reg Graphics_Addr ;		/*	03CE	--	SAME */
	/* io86reg ??????? */		/*	03CF	--	SAME */
	io86reg Set_Reset ;		/*	Graphics_Addr == 00   */
	io86reg Enb_Set_Reset ;		/*	Graphics_Addr == 01   */
	io86reg Color_Compare ;		/*	Graphics_Addr == 02   */
	io86reg Data_Rotate ;		/*	Graphics_Addr == 03   */
	io86reg Read_Map_Select ;	/*	Graphics_Addr == 04   */
	io86reg Graphics_Mode ;		/*	Graphics_Addr == 05   */
	io86reg Miscellaneous ;		/*	Graphics_Addr == 06   */
	io86reg Color_Dont_Care ;	/*	Graphics_Addr == 07   */
	io86reg Bit_Mask ;		/*	Graphics_Addr == 08   */

	/* Video DAC Registers  03CE & 03CF */
	io86reg PEL_WR_Addr ;		/*	03C8	--	SAME */
	io86reg PEL_RD_Addr ;		/*	XXXX	--	03C7 */
	io86reg DAC_State ;		/*	XXXX	--	03C7 */
	io86reg PEL_Data_Reg ;		/*	03C9	--	SAME */
	io86reg PEL_Mask_Reg ;		/*	03C6	--	SAME */

	/* contents of address 0 in video memory */
	io86reg Mem_Loc_0[4] ;	        /*	*/
} ;

/* Global Flag for Virtual Screen Saves */
extern int pliDisabled ;

#define PhysicalPliAccessDisabled	0x1
#define VirtualPliHardwareOpenPending	0x2
#define VirtualPliHardwareClosePending	0x4

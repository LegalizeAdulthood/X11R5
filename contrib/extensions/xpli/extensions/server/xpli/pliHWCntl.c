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
#include "OScompiler.h"
#include "newmacros.h"

/*
#include "ibmIOArch.h"
*/

#include "pliVideo.h"
#include "pliReg.h"
#include "pliSave.h"

/*
 * HARDWARE REGISTER USAGE :
 *******	EVERYTHING
 */
/*
#define IO_DELAY { temp = 1; for ( i = 0 ; i < 200 ; i++ ) temp++/100; }
*/

void
save_pli_state( VS )
register struct pli_video_hardware_state *VS ;
{
register IO_Address Target_Reg ; /* if mono == 0x3B0, if color == 3D0 */
register IO_Address Base_Reg ; /* if mono == 0x3B0, if color == 3D0 */

/* Read General Registers */
VS->Feature_Control = inb( 0x3CA ) ;
VS->Video_Enable = inb( 0x3C3 ) ;
Base_Reg = ( ( VS->Misc_Output_Reg = inb( 0x3CC ) ) & 1 ) ? 0x3D0 : 0x3B0 ;
VS->Input_Status_0 = inb( 0x3C2 ) ;
/* Initialize Flip-Flop */
VS->Input_Status_1 = inb( Base_Reg + 0xA ) ;

/* Save Attribute Registers  03C0 & 03C1 */
/*
INTS_OFF() ;
*/
VS->Attr_Addr_Reg = inb( 0x3C0 ) ; /* Save Existing Index First */
/* Target_Reg Used As Scrap */
for ( Target_Reg = 0 ; Target_Reg <= 0xF ; Target_Reg++ ) {
	outb( 0x3C0, (unsigned char) Target_Reg ) ;
	/* Read It, Save It, Then Write It Back */
	outb( 0x3c0, ( VS->Palette[Target_Reg] = inb( 0x3C1 ) ) ) ;
}
outb( 0x3c0, 0x30 ) ;
outb( 0x3c0, ( VS->Attr_Mode = inb( 0x3C1 ) ) ) ;
outb( 0x3c0, 0x31 ) ;
outb( 0x3c0, ( VS->Overscan_Color = inb( 0x3C1 ) ) ) ;
outb( 0x3c0, 0x32 ) ;
outb( 0x3c0, ( VS->Color_Plane_En = inb( 0x3C1 ) ) ) ;
outb( 0x3c0, 0x33 ) ;
outb( 0x3c0, ( VS->Horiz_PEL_Pan = inb( 0x3C1 ) ) ) ;
outb( 0x3c0, 0x34 ) ;
outb( 0x3c0, ( VS->Color_Select = inb( 0x3C1 ) ) ) ; /* Attr_Addr_Reg == 14 */
/* Re-Enable Video Access To The Color Palette */
/*
INTS_ON() ;
*/

/* Save Crt Controller Registers  03?4 & 03?5 */
VS->Index_Reg = inb( Target_Reg = ( Base_Reg += 0x4 ) ) ;
outb( Base_Reg, 0x00 ) ;
VS->Horiz_Total = inb( ++Target_Reg ) ;
outb( Base_Reg, 0x01 ) ;
VS->Horiz_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x02 ) ;
VS->H_Blank_Start = inb( Target_Reg ) ;
outb( Base_Reg, 0x03 ) ;
VS->H_Blank_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x04 ) ;
VS->H_Retrace_Start = inb( Target_Reg ) ;
outb( Base_Reg, 0x05 ) ;
VS->H_Retrace_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x06 ) ;
VS->Vert_Total = inb( Target_Reg ) ;
outb( Base_Reg, 0x07 ) ;
VS->Overflow = inb( Target_Reg ) ;
outb( Base_Reg, 0x08 ) ;
VS->Preset_Row_Scan = inb( Target_Reg ) ;
outb( Base_Reg, 0x09 ) ;
VS->Max_Scan_Line = inb( Target_Reg ) ;
outb( Base_Reg, 0x0A ) ;
VS->Cursor_Start = inb( Target_Reg ) ;
outb( Base_Reg, 0x0B ) ;
VS->Cursor_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x0C ) ;
VS->Start_Addr_Hi = inb( Target_Reg ) ;
outb( Base_Reg, 0x0D ) ;
VS->Start_Addr_Lo = inb( Target_Reg ) ;
outb( Base_Reg, 0x0E ) ;
VS->Cursor_Loc_Hi = inb( Target_Reg ) ;
outb( Base_Reg, 0x0F ) ;
VS->Cursor_Loc_Lo = inb( Target_Reg ) ;
outb( Base_Reg, 0x10 ) ;
VS->V_Retrace_Start = inb( Target_Reg ) ;
outb( Base_Reg, 0x11 ) ;
VS->V_Retrace_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x12 ) ;
VS->V_Display_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x13 ) ;
VS->Underline_Loc = inb( Target_Reg ) ;
outb( Base_Reg, 0x14 ) ;
VS->Offset = inb( Target_Reg ) ;
outb( Base_Reg, 0x15 ) ;
VS->V_Blank_Start = inb( Target_Reg ) ;
outb( Base_Reg, 0x16 ) ;
VS->V_Blank_End = inb( Target_Reg ) ;
outb( Base_Reg, 0x17 ) ;
VS->CRTC_Mode = inb( Target_Reg ) ;
outb( Base_Reg, 0x18 ) ;
VS->Line_Compare = inb( Target_Reg ) ;
/* Readjust Base Register */
Base_Reg -= 0x4 ;

/* Sequencer Registers  03C4 & 03C5 */
/* VS->Seq_Addr_Reg = inb( 0x3C4 ) ; */ /*	03C4	--	SAME */
outb( 0x3C4, 0x00 ) ;
VS->Seq_Reset = inb( 0x3C5 ) ;
outb( 0x3C4, 0x01 ) ;
VS->Clock_Mode = inb( 0x3C5 ) ;
outb( 0x3C4, 0x02 ) ;
VS->Mask_Map = inb( 0x3C5 ) ;
outb( 0x3C4, 0x03 ) ;
VS->Char_Map_Select = inb( 0x3C5 ) ;
outb( 0x3C4, 0x04 ) ;
VS->Memory_Mode = inb( 0x3C5 ) ;

/* Graphics Registers  03CE & 03CF */
/* VS->Graphics_Addr = inb( 0x3CE ) ; */ /*	03CE	--	SAME */
/* ??????? */		/*	03CF	--	SAME */
outb( 0x3CE, 0x00 ) ;
VS->Set_Reset = inb( 0x3CF ) ;
outb( 0x3CE, 0x01 ) ;
VS->Enb_Set_Reset = inb( 0x3CF ) ;
outb( 0x3CE, 0x02 ) ;
VS->Color_Compare = inb( 0x3CF ) ;
outb( 0x3CE, 0x03 ) ;
VS->Data_Rotate = inb( 0x3CF ) ;
outb( 0x3CE, 0x04 ) ;
VS->Read_Map_Select = inb( 0x3CF ) ;
outb( 0x3CE, 0x05 ) ;
VS->Graphics_Mode = inb( 0x3CF ) ;
outb( 0x3CE, 0x06 ) ;
VS->Miscellaneous = inb( 0x3CF ) ;
outb( 0x3CE, 0x07 ) ;
VS->Color_Dont_Care = inb( 0x3CF ) ;
outb( 0x3CE, 0x08 ) ;
VS->Bit_Mask = inb( 0x3CF ) ;		/*	Graphics_Addr == 08   */

/* Video DAC Registers  03CE & 03CF */
VS->PEL_WR_Addr = inb( 0x3C8 ) ;	/*	03C8	--	SAME */
	/* PEL_RD_Addr is WRITE-ONLY */
VS->DAC_State = inb( 0x3C7 ) ;		/*	XXXX	--	03C7 */
VS->PEL_Data_Reg = inb( 0x3C9 ) ;	/*	03C9	--	SAME */
VS->PEL_Mask_Reg = inb( 0x3C6 ) ;

return ;
} ;

void
restore_pli_state( VS )
register struct pli_video_hardware_state * const VS ;
{
register IO_Address Base_Reg ; /* if mono == 0x3B0, if color == 3D0 */
register IO_Address Target_Reg ;

/* Setup I/O Base Address */
Base_Reg = ( VS->Misc_Output_Reg & 1 ) ? 0x3D0 : 0x3B0 ;

/* Sequencer Registers  03C4 & 03C5 */
/* Do Hardware Syncronous RESET */
SetVideoSequencer( Seq_ResetIndex,		VS->Seq_Reset & 0xFD ) ;
SetVideoSequencer( Clock_ModeIndex,		VS->Clock_Mode ) ;
SetVideoSequencer( Mask_MapIndex,		VS->Mask_Map ) ;
SetVideoSequencer( Char_Map_SelectIndex,	VS->Char_Map_Select ) ;
SetVideoSequencer( Memory_ModeIndex,		VS->Memory_Mode ) ;

/* Write General Registers */
outb( 0x3C2, VS->Misc_Output_Reg ) ;
/* VS->Input_Status_0 & VS->Input_Status_1 are READ-ONLY */
outb( Base_Reg + 0xA, VS->Feature_Control ) ;
outb( 0x3C3, VS->Video_Enable ) ;

/* Attribute Registers */
/* Initialize Flip-Flop */
{ register tmp = inb( Base_Reg + 0xA ) ; }

/* Target_Reg Used As Scrap */
for ( Target_Reg = 0 ; Target_Reg <= 0xF ; Target_Reg++ ) {
	SetVideoAttribute( Target_Reg,		VS->Palette[Target_Reg] ) ;
}
SetVideoAttribute( AttributeModeIndex,		VS->Attr_Mode ) ;
SetVideoAttribute( OverScanColorIndex,		VS->Overscan_Color ) ;
SetVideoAttribute( ColorPlaneEnableIndex,	VS->Color_Plane_En ) ;
SetVideoAttribute( HorizPelPanIndex,		VS->Horiz_PEL_Pan ) ;
SetVideoAttribute( ColorSelectIndex,		VS->Color_Select ) ;

/* Enable CRT Controller Registers 0-7 */
outb( Target_Reg = ( Base_Reg += 0x4 ), 0x11 ) ;
outb( ++Target_Reg, 0x0C ) ;
/* Restore Crt Controller Registers  03?4 & 03?5 */
outb( Base_Reg, 0x00 ) ;
outb( Target_Reg, VS->Horiz_Total ) ;
outb( Base_Reg, 0x01 ) ;
outb( Target_Reg, VS->Horiz_End ) ;
outb( Base_Reg, 0x02 ) ;
outb( Target_Reg, VS->H_Blank_Start ) ;
outb( Base_Reg, 0x03 ) ;
outb( Target_Reg, VS->H_Blank_End ) ;
outb( Base_Reg, 0x04 ) ;
outb( Target_Reg, VS->H_Retrace_Start ) ;
outb( Base_Reg, 0x05 ) ;
outb( Target_Reg, VS->H_Retrace_End ) ;
outb( Base_Reg, 0x06 ) ;
outb( Target_Reg, VS->Vert_Total ) ;
outb( Base_Reg, 0x07 ) ;
outb( Target_Reg, VS->Overflow ) ;
outb( Base_Reg, 0x08 ) ;
outb( Target_Reg, VS->Preset_Row_Scan ) ;
outb( Base_Reg, 0x09 ) ;
outb( Target_Reg, VS->Max_Scan_Line ) ;
outb( Base_Reg, 0x0A ) ;
outb( Target_Reg, VS->Cursor_Start ) ;
outb( Base_Reg, 0x0B ) ;
outb( Target_Reg, VS->Cursor_End ) ;
outb( Base_Reg, 0x0C ) ;
outb( Target_Reg, VS->Start_Addr_Hi ) ;
outb( Base_Reg, 0x0D ) ;
outb( Target_Reg, VS->Start_Addr_Lo ) ;
outb( Base_Reg, 0x0E ) ;
outb( Target_Reg, VS->Cursor_Loc_Hi ) ;
outb( Base_Reg, 0x0F ) ;
outb( Target_Reg, VS->Cursor_Loc_Lo ) ;
outb( Base_Reg, 0x10 ) ;
outb( Target_Reg, VS->V_Retrace_Start ) ;
outb( Base_Reg, 0x11 ) ;
outb( Target_Reg, VS->V_Retrace_End ) ;
outb( Base_Reg, 0x12 ) ;
outb( Target_Reg, VS->V_Display_End ) ;
outb( Base_Reg, 0x13 ) ;
outb( Target_Reg, VS->Underline_Loc ) ;
outb( Base_Reg, 0x14 ) ;
outb( Target_Reg, VS->Offset ) ;
outb( Base_Reg, 0x15 ) ;
outb( Target_Reg, VS->V_Blank_Start ) ;
outb( Base_Reg, 0x16 ) ;
outb( Target_Reg, VS->V_Blank_End ) ;
outb( Base_Reg, 0x17 ) ;
outb( Target_Reg, VS->CRTC_Mode ) ;
outb( Base_Reg, 0x18 ) ;
outb( Target_Reg, VS->Line_Compare ) ;

/* Restore Graphics Registers  03CE & 03CF */
SetVideoGraphics( Set_ResetIndex, VS->Set_Reset ) ;
SetVideoGraphics( Enb_Set_ResetIndex, VS->Enb_Set_Reset ) ;
SetVideoGraphics( Color_CompareIndex, VS->Color_Compare ) ;
SetVideoGraphics( Data_RotateIndex, VS->Data_Rotate ) ;
SetVideoGraphics( Read_Map_SelectIndex, VS->Read_Map_Select ) ;
SetVideoGraphics( Graphics_ModeIndex, VS->Graphics_Mode ) ;
SetVideoGraphics( MiscellaneousIndex, VS->Miscellaneous ) ;
SetVideoGraphics( Color_Dont_CareIndex, VS->Color_Dont_Care ) ;
SetVideoGraphics( Bit_MaskIndex, VS->Bit_Mask ) ; /* Graphics_Addr == 08 */

/* Restore ?? Video DAC Registers  03C7 & 03C8 */
outb( 0x3C9, VS->PEL_Data_Reg ) ;		/*	03C9	--	SAME */
outb( 0x3C6, VS->PEL_Mask_Reg ) ;

/* Re-Enable Hardware i.e. Reset Register */
SetVideoSequencer( Seq_ResetIndex,		VS->Seq_Reset | 0x03 ) ;

return ;
} ;

/* PLI version sets inked pixels to half contrast differential from
   regular pixels e.g. (noninked == 0x0, inked = 0x24), (noninked == 0x3f,
   inked == 0x1b) NOTE may not work for photrons 
   JRR NOTE - hacked for photrons to set ink to black if pixel > 24 and
   white if pixel < 24 */
void
pliSetColor( color, r, g, b )
     register unsigned long int color ;
     register unsigned short r, b, g ;
{
  unsigned short ir, ig, ib;
  outb( 0x3C8, color ) ; /* Point PEL Address Register To Color Entry */
  ir =  r >> 10;
  outb( 0x3C9, ir);
  ig =  g >> 10;
  outb( 0x3C9, ig);
  ib =  b >> 10;
  outb( 0x3C9, ib);
  outb( 0x3C8, color+8 ) ; /* Point PEL Address Register To Ink Color Entry */
  if (ir < 0x24) ir = 0x3f; else ir = 0x00;
  outb( 0x3C9, ir);
  if (ig < 0x24) ig = 0x3f; else ig = 0x00;
  outb( 0x3C9, ig);
  if (ib < 0x24) ib = 0x3f; else ib = 0x00;
  outb( 0x3C9, ib);

return ;
}

void
pli_save_dac( tablePtr )
register DAC_TABLE tablePtr ;
{
register int i ;
register unsigned char *cptr ;

outb( 0x3C7, 0x0 ) ; /* Point PEL Address Register To First Entry */
for ( i = 768, cptr = (unsigned char *) tablePtr ; --i ; )
	*cptr++ = inb( 0x03C9 ) ;
return ;
}

void
pli_restore_dac( tablePtr )
register DAC_TABLE tablePtr ;
{
register int i ;
register unsigned char *cptr ;

outb( 0x3C8, 0x0 ) ; /* Point PEL Address Register To First Entry */
for ( i = 768, cptr = (unsigned char *) tablePtr ; --i ; )
	outb( 0x03C9, *cptr++ ) ;
return ;
}

/*
 *	Initialize the vga to 640 x 480, 16 of 64 colors @ a0000
OR --	Initialize the vga to 720 x 540, 16 of 64 colors @ a0000
 */

void
pli_set_graphics_mode( VS )
register struct pli_video_hardware_state *VS ;
{
	extern void paradise_setup();
	register short temp,i;
/* Setup I/O Base Address */
/*	Color 640 by 480 -- 16 Color
OR -	Color 720 by 540 -- 16 Color */
#define Color_Base_Reg ( 0x3D0 )

/* Do initial setup for the paradise card. */
paradise_setup();

/* Sequencer Registers  03C4 & 03C5 */
SetVideoSequencer( Seq_ResetIndex,		0x01 ) ; /* Syncronous RESET */
SetVideoSequencer( Clock_ModeIndex,		0x01 ) ;
/* Make All Planes Writable */
SetVideoSequencer( Mask_MapIndex,		PLI_ALLPLANES ) ;
SetVideoSequencer( Char_Map_SelectIndex,	0x00 ) ;
SetVideoSequencer( Memory_ModeIndex,		0x06 ) ;

/* Write General Registers */
/* VS.Input_Status_0 & VS.Input_Status_1 are READ-ONLY */
#ifndef PLI720
outb( 0x3C2, /* VS.Misc_Output_Reg */ 0xE3 ) ; /* VS.Misc_Output_Reg */
#else /* PLI720 */
outb( 0x3C2, 0xE7 ) ; /* VS.Misc_Output_Reg */ /* XXX */
#endif
#ifdef DUMP_CORE
IO_DELAY;
outb( Color_Base_Reg + 0xA, VS->Feature_Control ) ;
IO_DELAY;
outb( 0x3C3, /* VS.Video_Enable */ 0x1 ) ;
IO_DELAY;
#endif DUMP_CORE

/* Re-Enable Hardware i.e. Reset Register */
outb( 0x3C4, 0x00 ) ;
IO_DELAY;
outb( 0x3C5, /* VS.Seq_Reset */ 0x03 ) ; /* Enable Hardware Reset Register */

/* Attribute Registers */
/*
INTS_OFF() ;
*/
/* Initialize Flip-Flop */
{ register tmp = inb( Color_Base_Reg + 0xA ) ; }
/* Set Palette Register Value Equal To Palette Register Index Number */
/* i.e. Palette is 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F */
{
	register IO_Address Target_Reg ;
	for ( Target_Reg = 0 ; Target_Reg <= 0xF ; Target_Reg++ ) {
		SetVideoAttribute( Target_Reg, Target_Reg ) ;
	}
}
/* Rest Of The Attribute Registers */
/* Note: 0x20 is added to the index 
 * to Re-Enable Video Access To The Color Palette */
/* VS.Attr_Mode - P4,P5 for color select */
SetVideoAttribute( AttributeModeIndex,		0x81 ) ;
SetVideoAttribute( OverScanColorIndex,		0x00 ) ;
SetVideoAttribute( ColorPlaneEnableIndex,	0x0F ) ;
SetVideoAttribute( HorizPelPanIndex,		0x00 ) ;
SetVideoAttribute( ColorSelectIndex,		0x00 ) ;

/* Enable CRT Controller Registers 0-7 */
outb( 0x3D4, 0x11 ) ;
outb( 0x3D5, 0x0C ) ;
/* Set CRT Controller Registers  03?4 & 03?5 */
outb( 0x3D4, 0x00 ) ;
#ifndef PLI720
outb( 0x3D5, 95 ) ; /* VS.Horiz_Total */
#else /* PLI720 */
outb( 0x3D5, 105 ) ; /* VS.Horiz_Total */ /* XXX */
#endif
outb( 0x3D4, 0x01 ) ;
#ifndef PLI720
outb( 0x3D5, 79 ) ; /* VS.Horiz_End */
#else /* PLI720 */
outb( 0x3D5, 89 ) ; /* VS.Horiz_End */ /* XXX */
#endif
outb( 0x3D4, 0x02 ) ;
#ifndef PLI720
outb( 0x3D5, 0x50 ) ; /* VS.H_Blank_Start */
#else /* PLI720 */
outb( 0x3D5, 0x5A ) ; /* VS.H_Blank_Start */ /* XXX */
#endif
outb( 0x3D4, 0x03 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.H_Blank_End */ 0x82 ) ;
#else /* PLI720 */
outb( 0x3D5, 0x8A ) ; /* VS.H_Blank_End */ /* XXX */
#endif
outb( 0x3D4, 0x04 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.H_Retrace_Start */ 0x54 ) ;
#else /* PLI720 */
outb( 0x3D5, 0x5C ) ; /* VS.H_Retrace_Start */ /* XXX */
#endif
outb( 0x3D4, 0x05 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.H_Retrace_End */ 0x80 ) ;
#else /* PLI720 */
outb( 0x3D5, 0x88 ) ; /* VS.H_Retrace_End */
#endif
outb( 0x3D4, 0x06 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.Vert_Total */ 0x0B ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.Vert_Total */ 0x4F ) ; /* XXX */
#endif
outb( 0x3D4, 0x07 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.Overflow */ 0x3E ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.Overflow */ 0xF0 ) ; /* XXX */
#endif
outb( 0x3D4, 0x08 ) ;
outb( 0x3D5, /* VS.Preset_Row_Scan */ 0 ) ;
outb( 0x3D4, 0x09 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.Max_Scan_Line */ 0x40 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.Max_Scan_Line */ 0x60 ) ; /* XXX */
#endif
outb( 0x3D4, 0x0A ) ;
outb( 0x3D5, /* VS.Cursor_Start */ 0 ) ;
outb( 0x3D4, 0x0B ) ;
outb( 0x3D5, /* VS.Cursor_End */ 0 ) ;
outb( 0x3D4, 0x0C ) ;
outb( 0x3D5, /* VS.Start_Addr_Hi */ 0 ) ;
outb( 0x3D4, 0x0D ) ;
outb( 0x3D5, /* VS.Start_Addr_Lo */ 0 ) ;
outb( 0x3D4, 0x0E ) ;
outb( 0x3D5, /* VS.Cursor_Loc_Hi */ 0 ) ;
outb( 0x3D4, 0x0F ) ;
outb( 0x3D5, /* VS.Cursor_Loc_Lo */ 0 ) ;
outb( 0x3D4, 0x10 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.V_Retrace_Start */ 234 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.V_Retrace_Start */ 32 ) ; /* XXX */
#endif
outb( 0x3D4, 0x11 ) ;
outb( 0x3D5, /* VS.V_Retrace_End */ 0x8C ) ;
outb( 0x3D4, 0x12 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.V_Display_End */ 223 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.V_Display_End */ 27 ) ; /* XXX */
#endif
outb( 0x3D4, 0x13 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.Underline_Loc */ 40 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.Offset */ 45 ) ; /* XXX */
#endif
outb( 0x3D4, 0x14 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.Offset */ 0 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.Underline_Loc */ 0 ) ;
#endif
outb( 0x3D4, 0x15 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.V_Blank_Start */ 231 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.V_Blank_Start */ 35 ) ; /* XXX */
#endif
outb( 0x3D4, 0x16 ) ;
#ifndef PLI720
outb( 0x3D5, /* VS.V_Blank_End */ 4 ) ;
#else /* PLI720 */
outb( 0x3D5, /* VS.V_Blank_End */ 2 ) ;
#endif
outb( 0x3D4, 0x17 ) ;
outb( 0x3D5, /* VS.CRTC_Mode */ 0xE3 ) ;
outb( 0x3D4, 0x18 ) ;
outb( 0x3D5, /* VS.Line_Compare */ 255 ) ;

/* Set Graphics Registers  03CE & 03CF */
SetVideoGraphics( Set_ResetIndex,	0x00 ) ;
SetVideoGraphics( Enb_Set_ResetIndex,	0x00 ) ;
SetVideoGraphics( Color_CompareIndex,	0x00 ) ;
SetVideoGraphics( Data_RotateIndex,	0x00 ) ;
SetVideoGraphics( Read_Map_SelectIndex,	0x00 ) ;
SetVideoGraphics( Graphics_ModeIndex,	PLI_WRITE_MODE_2 ) ;
SetVideoGraphics( MiscellaneousIndex,	0x05 ) ;
SetVideoGraphics( Color_Dont_CareIndex,	PLI_ALLPLANES ) ;
SetVideoGraphics( Bit_MaskIndex,	0xFF ) ; /* All Bits Writable */

/* Video DAC Registers  03C7 & 03C8 */
outb( 0x3C9, VS->PEL_Data_Reg ) ;	/*	03C9	--	SAME */
outb( 0x3C6, VS->PEL_Mask_Reg ) ;

return ;
}

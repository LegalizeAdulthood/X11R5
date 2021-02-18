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
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliReg.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliReg.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
/*
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliReg.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
*/
#endif

#define SET_BYTE_REGISTER( ioport, value )	outb( ioport, value )
#define SET_INDEX_REGISTER( ioport, value ) SET_BYTE_REGISTER( ioport, value )
#define SET_DATA_REGISTER( ioport, value ) SET_BYTE_REGISTER( ioport, value )
#if defined(RTIO) || defined(ATRIO)
#define SET_INDEXED_REGISTER( RegGroup, index, value ) \
	outw( RegGroup, ( ( index ) << 8 ) | ( value ) )
#else
#if defined(PCIO)
#define SET_INDEXED_REGISTER( RegGroup, index, value ) \
	outw( RegGroup, ( ( value ) << 8 ) | ( index ) )
#else
	******** ERROR ********
#endif
#endif

/* There is a jumper on the ega to change this to 0x200 instead !! */
#define REGBASE				0x300

#define AttributeIndexRegister		REGBASE + 0xC0
#define AttributeDataWriteRegister	REGBASE + 0xC0
#define AttributeDataReadRegister	REGBASE + 0xC1
#define AttributeRegister		AttributeIndexRegister
#define AttributeModeIndex		0x30
#define OverScanColorIndex		0x31
#define ColorPlaneEnableIndex		0x32
#define HorizPelPanIndex		0x33
#define ColorSelectIndex		0x34
#define IO_DELAY { register short i, tmp = 1; \
	for ( i = 0; i <200; i++) tmp++/i;}
#define SetVideoAttributeIndex( index ) \
	SET_INDEX_REGISTER( AttributeIndexRegister, index )
#define SetVideoAttribute( index, value ) \
	IO_DELAY ; \
	SetVideoAttributeIndex( index ) ; \
	SET_BYTE_REGISTER( AttributeDataWriteRegister, value ); \
	IO_DELAY ;

	/* Graphics Registers  03CE & 03CF */
#define GraphicsIndexRegister		REGBASE + 0xCE
#define GraphicsDataRegister		REGBASE + 0xCF
#define GraphicsRegister		GraphicsIndexRegister
#define Set_ResetIndex			0x00
#define Enb_Set_ResetIndex		0x01
#define Color_CompareIndex		0x02
#define Data_RotateIndex		0x03
#define Read_Map_SelectIndex		0x04
#define Graphics_ModeIndex		0x05
#define MiscellaneousIndex		0x06
#define Color_Dont_CareIndex		0x07
#define Bit_MaskIndex			0x08
#define SetVideoGraphicsIndex( index ) \
	SET_INDEX_REGISTER( GraphicsIndexRegister, index )
#define SetVideoGraphicsData( value ) \
	SET_INDEX_REGISTER( GraphicsDataRegister, value )
#define SetVideoGraphics( index, value ) \
	SET_INDEXED_REGISTER( GraphicsRegister, index, value )

/* Sequencer Registers  03C4 & 03C5 */
#define SequencerIndexRegister		REGBASE + 0xC4
#define SequencerDataRegister		REGBASE + 0xC5
#define SequencerRegister		SequencerIndexRegister
#define Seq_ResetIndex			00
#define Clock_ModeIndex			01
#define Mask_MapIndex			02
#define Char_Map_SelectIndex		03
#define Memory_ModeIndex		04
#define SetVideoSequencerIndex( index ) \
	SET_INDEX_REGISTER( SequencerIndexRegister, index )
#define SetVideoSequencer( index, value ) \
	SET_INDEXED_REGISTER( SequencerRegister, index, value )

/* BIT CONSTANTS FOR THE VGA/EGA HARDWARE */
/* for the Graphics' Data_Rotate Register */
#define PLI_ROTATE_FUNC_SHIFT 3
#define PLI_COPY_MODE	( 0 << PLI_ROTATE_FUNC_SHIFT ) /* 0x00 */
#define PLI_AND_MODE	( 1 << PLI_ROTATE_FUNC_SHIFT ) /* 0x08 */
#define PLI_OR_MODE	( 2 << PLI_ROTATE_FUNC_SHIFT ) /* 0x10 */
#define PLI_XOR_MODE	( 3 << PLI_ROTATE_FUNC_SHIFT ) /* 0x18 */
/* for the Graphics' Graphics_Mode Register */
#define PLI_READ_MODE_SHIFT 3
#define PLI_WRITE_MODE_0	0
#define PLI_WRITE_MODE_1	1
#define PLI_WRITE_MODE_2	2
#define PLI_WRITE_MODE_3	3
#define PLI_READ_MODE_0		( 0 << PLI_READ_MODE_SHIFT )
#define PLI_READ_MODE_1		( 1 << PLI_READ_MODE_SHIFT )

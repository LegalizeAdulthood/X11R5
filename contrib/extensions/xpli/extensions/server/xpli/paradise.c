#include "newmacros.h"
#include <stdio.h>
#define TRUE 1
#define AX ( ax | al )
#define GRC_INDEX       0X3CE            /* GRAPHICS */

#define IO_DELAY { for ( i = 0 ; i < 200 ; i++ ) temp=0; }

#define EXCHANGE(a,b) { temp = a; a = b; b = temp ; }

#define PARADISE_READ()  { \
	fprintf(stderr, "Invoking outb().\n"); \
	dx= GRC_INDEX ; outb(dx,al); \
	fprintf(stderr, "After outb().\n"); \
	dl++; \
	EXCHANGE(ah,al);\
	al=inb(dx); \
	EXCHANGE(ah,al) ; \
}

/*                */
/* -----> vga mode parameters for mode 3 setup */
typedef char MODE3_PARAM_TYPE;
MODE3_PARAM_TYPE mode3[] = { 
	80,24,16, /* Check if these are to hexa values. */
	0X10, 0x00,

	0X00,0X03,0X00,0X02, /* sequencer parms (Clock mode,map mask, char map sel,mem mode) */

	0X67,				/*  misc. output register */

	0X5f,0X4f,0X50,0X82,0X55,0X81, /*  crt controller regs */
	0Xbf,0X1f,0X00,0X4f,0X0d,0X0e,
	0X00,0X00,0X00,0X00,0X9c,0X8e,
	0X8f,0X28,0X1f,0X96,0Xb9,0Xa3,
	0Xff,				/*  end of crt-c bytes */

	0X00,0X01,0X02,0X03,0X04,0X05,	/* attribute control regs */
	0X14,0X07,0X38,0X39,0X3a,0X3b,
	0X3c,0X3d,0X3e,0X3f,0X0c,0X00,
	0X0f,0X08,			 /* end of attribute ctl regs */

	0X00,0X00,0X00,0X00,0X00,0X10, /* graphics controller regs*/
	0X0e,0X00,0Xff
	};

void paradise_setup()
{
	char ah = 0,al = 0, temp=0 ; 
	int ax,dx;
	register int i,cl;
	int b1=0, bl =0 ;
	register MODE3_PARAM_TYPE * bx;
	char dl;
	
	/* zero out paradise VGA registers. */
	al = 0x0f;
	PARADISE_READ() ;
	ah &= 0x7;
	bl = ah;	/* SAVE RESULT. */
	dx=0x3ce;
	outb(dx, 0X50F);
	al=0X09;
	/* For each paradise resiter, zero the register */
	for ( al = 0X09; al < 0XF ; al++)
	{
		ah=0;
#ifdef USE_GOTO
		if ( al != 0XC) goto check_0b ; /* Video select register? */
		ah=0x2;
		goto pw;
		check_ob:
			if ( al != 0x0b ) goto pw;  /* If pr1, set memory size for 
		                                   paradise board. */
			ah=0x85;
		pw:
			outb(dx, AX );
#else
		if ( al != 0XC) {
			if ( al == 0x0b) { /* pr1 */
			/* set memory size for the WD board. */
			    ah=0x85;
			}
		}
		else {
		/* Video Select register. */
			ah=0x2;
		}
		outb(dx,AX);
		IO_DELAY;
#endif /* ~ USE_GOTO */
		
	} /* for */
	/* ------> Zero the new paradise registers, force VGA to color. */
	al=1;
	dx = 0x3c2 ;
	outb(dx, al); /* force to color mode. */
	IO_DELAY;
	al=0x29;
	ah=0x85;
	ax=ah | al ;
	dx=0x3d4;
	outw(dx,ax); /* Unlock PR11-17 */
	al++;
	outw(dx,ax); /*  Start with register PR11. */
	dx++;	
	al = inb(dx);
	al &= 0x80 ; /* Save bit 7 of PR11 and clear other bits.  */
	outb(dx, al);
	dx--;
	al=0x2b ;	/* start with register 2b */
	/* For each new paradise register, do the task. */
	for ( al = 0x2b ; al <= 0x2F ; al++) {
		ah = 0;
		if ( al == 0x2f) {
			/* Misc. control register 2. */
			ah=0x42;
		}
		else {
	
			if ( al == 0x2b ){
				/* PR12 register. */
				if ( !b1 ) {
					b1++;
					ah= 0x5; /* Force to unlock state. */
				}
				else {
					/* Don't zero if not first time. */
					continue;
				}
			}
			outb(dx,ax);	/* Zero the register. */
			IO_DELAY;
		}
	} /* for */
	al=0x30; 	/* PR17 ( Misc ctrl reg 3 ) */
	ah=0;
	outb(dx,al); /* force 32K ROM */
	/* Unlock Flat panel control registers. */
	dx=0x3d4;
	outw(dx, 0xa634);
	/* select CRT */
	ax=0x2032;
	outw(dx,ax) ; /* CRT selected. */
	IO_DELAY ;
	/* -------> Initialize the video controller. */
	/* Hardware interrupts must be disabled while programming
	 * the sequencer until the sequencer is started. */
	bx = &mode3[5] ; /* bx = sequencer parameter. */
	dx=0x3c4; /* dx = sequencer address register. */
	/* The following will stop the register. */
	ah=1; /* setup to reset the sequencer. */
	cl = 0; /* setup index to register */
	al=cl; /* get index. */
	outb(dx,al); /* set the index register. */
	IO_DELAY;
	dx++; /* point to data port. */
	/* exchange al and ah */
	EXCHANGE(al,ah);
	outb(dx,al);
	IO_DELAY;
	dx--;

	/* Load the rest of sequencer bytes - clock mode/map mask/char 
	 * sel/memory mode */
	ah=*bx; /* get the sequencer from table. Is this correct????? */
	cl++; /* Get the next index register. */
	al=cl;
	outb(dx, al); /* set the index register. */
	IO_DELAY;
	dx++; /* point to data port. */
	/* Exchange al and ah to set al = data. */
	EXCHANGE(al,ah);
	/* Exchange over. */
	outb(dx,al);
	IO_DELAY;
	dx--; /* restore dx. */
	for ( i= 0 ; i < 4 ; i++)
	{
		cl++;
		bx++;
		ah=*bx; /* get the sequencer value from table. */
		al=cl; /* get index */
		outb(dx,al);
		IO_DELAY;
		dx++; /* point to data port. */
		EXCHANGE(al,ah);
		outb( dx, al); /* send data. */
		IO_DELAY;
		dx--; /* restore dx. */
	}	
	/* Program the misc. output register. */
	/* Note you have to stop the sequencer before misc. output register
	 * be programmed. */
#ifdef OLD
	dl = 0Xc2;	 /* set dx = misc. output register. */
	dx = ( (dx & 0x00) | dl );
	bx++; 
#else
	dl = 0Xc2;	 /* set dx = misc. output register. */
	dx = 0x3c2;
	bx = &mode3[9];
#endif OLD
	al=*bx; /* get the misc address regiter value from table */
	outb(dx,al); /* set value to the register. */
	IO_DELAY;
	/* ---------> Start the sequencer. */
	dl = 0xc4; /* dx = sequencer address. */
	ah = 0x03;
	cl = 0x00;
	al=cl;
#ifdef OLD
	dx= ( ( dx & 0x00) | dl );
#else
	dx= 0x3c4;
#endif OLD
	outb(dx,al);
	IO_DELAY;
	dx++; /* point to data port. */
	EXCHANGE(al,ah); /* set al = data */
	outb( dx, al); /* send data. */
	IO_DELAY;
	dx--;
	/* ---------> Program the crt controller registers. */
	dx = 0x3d4;
	cl =0 ;
	bx++;
	for ( cl = 0 ; cl < 25 ; cl++ )
	{
		ah = *bx;
		al=cl;
		outb(dx,al);
		IO_DELAY;
		dx++;
		EXCHANGE(al,ah);
		outb( dx, al); /* send data. */
		IO_DELAY;
		dx--; /* restore dx */
		bx++;
	}
	/* ---------> Program the attribute controller registers. 
	 Note: An input to the input status register ( 3dah or 3bah)
	 initializes the attribute controller flip-flop to select
	 the address register.
	 * ****** Hardware Interrupts must be disabled during the
	 * sequence. */
	dx = 0x3da; /* Set color input status register. */
	al = inb(dx); /* init. attr cntrl f-f to address reg. */
	IO_DELAY;
	dl = 0xc0;
	dx = 0x3c0; /* set dx  = attribute chip register. */
	for ( ah = 0 ; ah < 14 ; ah++ )
	{
		al = *bx;
		EXCHANGE(al,ah);
		outb(dx,al);
		IO_DELAY;
		EXCHANGE(al,ah);
		outb(dx,al) ; /* output data value to index register. */
		IO_DELAY;
		bx++;
	}
	/* ----------> program the graphics controller. */
	dl = 0xce; 
	dx = 0x3ce; /* set dx = graphics cntrl address reg. */
	for ( cl = 0; cl < 9 ; cl++ )
	{
		ah = *bx;
		al =cl ;
		outb(dx, al);
		IO_DELAY;
		dx++;
		EXCHANGE(al,ah);
		outb(dx,al);
		IO_DELAY;
		dx--; /* restore dx */
		bx++;
	}
	/* ---------> Program the feature controller for color monitor. */
	/* Note: Values for this register will be different for pass 2 chips. */
	dx = 0x3da;
	al =4;
	outb(dx,al);
	return ;
} /* paradise_setup() */

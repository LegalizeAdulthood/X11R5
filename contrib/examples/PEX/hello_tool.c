/* $XConsortium: hello_tool.c,v 5.1 91/02/16 09:32:23 rws Exp $ */

/***********************************************************
Copyright (c) 1991 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* a simple program using an x_tool type workstation */

#include "phigs/phigs.h"

main()
{
	Ppoint	text_pt;

	popen_phigs( PDEF_ERR_FILE, PDEF_MEM_SIZE );
	popen_ws( 1, NULL, phigs_ws_type_x_tool );

	popen_struct( 1 );
	    text_pt.x = 0.2;
	    text_pt.y = 0.5;
	    pset_char_ht( 0.05 );
	    ptext( &text_pt, "Hello World", 11); 
	pclose_struct( );

	ppost_struct( 1, 1, 1.0 );

	printf("return...");
	getchar();

	pclose_ws( 1 );
	pclose_phigs( );
}

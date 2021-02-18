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

#ifndef lint
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/forsort/RCS/forsort.c,v 1.3 91/07/25 10:13:51 y-morika Exp $";
#endif
/*
 * forsort.c	�����Ȥ�Ԥʤ�����8�ӥåȥ����ɤ��ݸ��
 *	forsort -7 < [in-file-name] > [out-file-name]
 *		-7	������
 *		-8	�����
 */
#include	<stdio.h>
#include	<ctype.h>

char	*hd	= "0123456789ABCDEF";

/* #define	ESC	'@'*/
#define	ESC 033	
e2j()
{
    register unsigned	c;

    while ( (c = getchar()) != EOF ) {
	if ( isspace(c) ) 
	    putchar(c);
	else {
	    putchar(hd[c>>4]);
	    putchar(hd[c&15]);
	}
    };
}

j2e()
{
    register unsigned	c, d;

    while ( (c = getchar()) != EOF ) {
	if ( isspace(c) )
	    putchar(c);
	else {
	    d = getchar();

	    if ( isdigit(c) ) c -= '0'; else c -= 'A' - 10;
	    if ( isdigit(d) ) d -= '0'; else d -= 'A' - 10;
	    putchar((c<<4)|d);
	};
    };
}

main(n, args)
int	n;
char	*args[];
{
    if( n == 1 ) {		/* ���ޥ��̾�����λ� */
	fprintf(stderr, "usege: forsort -7 < [file],\n       forsort -8 < [file]\n");
	exit( -1 );
    }
    
    if( !strcmp(args[1], "-7"))
	e2j();
    else if( !strcmp(args[1], "-8"))
	j2e();
    else
	fprintf(stderr, "usege: forsort -7 < [file],\n       forsort -8 < [file]\n");
    exit(0);
}



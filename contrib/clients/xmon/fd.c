/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 * Project: XMON - An X protocol monitor
 *
 * File: fd.c
 *
 * Description: Support routines for file descriptors
 *
 * All of this code is to support the handling of file descriptors.
 * The idea is to keep a table of the FDs that are in use and why.  For
 * each FD that is open for input, we keep the name of a procedure to call
 * if input arrives for that FD.  When an FD is created (by an open, pipe,
 * socket, ...) declare that by calling UsingFD.  When it is no longer in
 * use (close ...), call NotUsingFD.
 */

#include <errno.h>	       /* for EINTR, EADDRINUSE, ... */

#ifdef SVR4
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include "common.h"

#include "xmond.h"
#include "select_args.h"

/* function prototypes: */
/* fd.c: */

/* end function prototypes */

extern FDDescriptor		*FDD;
extern long			ReadDescriptors[];
extern long			WriteDescriptors[];
extern short			HighestFD;

Global void
InitializeFD()
{
    register short  i;
    short   MaxFD /* maximum number of FD's possible */ ;
#ifdef SVR4
    struct rlimit rlp;
#endif

    enterprocedure("InitializeFD");
    /* get the number of file descriptors the system will let us use */
#ifdef SVR4
    if (getrlimit(RLIMIT_NOFILE, &rlp) < 0)
    {
	perror("getrlimit(RLIMIT_NOFILE)");
	exit(1);
    }
    MaxFD = rlp.rlim_cur;
#else
    MaxFD = getdtablesize();
#endif

    /* allocate space for a File Descriptor Table */
    FDD = (FDDescriptor *)malloc((long)(MaxFD * sizeof(FDDescriptor)));

    for (i = 0; i < MaxFD; i++)
	FDD[i].InputHandler = (VoidCallback)NULL;

    CLEARBITS(ReadDescriptors);
    CLEARBITS(WriteDescriptors);
    HighestFD = 0;
}

Global FDDescriptor*
UsingFD(fd, Handler, private_data)
    int			    fd;
    VoidCallback	    Handler;
    Pointer		    private_data;
{
    FDD[fd].InputHandler = Handler;
    FDD[fd].private_data = private_data;
    if (Handler == NULL)
	BITCLEAR(ReadDescriptors, fd);
    else
	BITSET(ReadDescriptors, fd);

    if (fd > HighestFD)
	HighestFD = fd;
    return &FDD[fd];
}

Global void
NotUsingFD(fd)
	int fd;
{
    FDD[fd].InputHandler = (VoidCallback)NULL;
    BITCLEAR(ReadDescriptors, fd);

    while (FDD[HighestFD].InputHandler == (VoidCallback)NULL && HighestFD > 0)
	HighestFD -= 1;
}

Global void
EOFonFD(fd)
	int fd;
{
    close(fd);
    NotUsingFD(fd);
}

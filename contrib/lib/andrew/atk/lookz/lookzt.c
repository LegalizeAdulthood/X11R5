/*LIBS: -lbasics -lclass -lerrors -lutil
*/
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookzt.c,v 1.2 1991/09/12 16:24:10 bobg Exp $ */
/* $ACIS:testdo.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookzt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/lookz/RCS/lookzt.c,v 1.2 1991/09/12 16:24:10 bobg Exp $";
#endif /* lint */


/* testdo.c
	test the lookz data object
*/


#include <stdio.h>
#include <class.h>
#include <observe.ih>
#include <proctbl.ih>
#include <dataobj.ih>

#include <lookz.ih>

main( argc, argv )
	register int	  argc;
	register char  **argv;
{
	register struct lookz *st, *st2;
	FILE *f;

	printf("Start\n"); fflush(stdout);
	class_Init(".");		/* use current directory for dataobject path (???) */
	printf("Init done\n"); fflush(stdout);

	observable_StaticEntry;
	proctable_StaticEntry;
	dataobject_StaticEntry;
/*
	lookz_StaticEntry;
*/
	printf("About to New\n"); fflush(stdout);
	st = lookz_New();

	printdata(st);
	lookz_SetVisibility(st, ! lookz_GetVisibility(st));
	printdata(st);

	lookz_Destroy(st);

	printf("\n Phase II\n");  fflush(stdout);
	st = lookz_New();

	printdata(st);

	printf("\nWriting plain data stream\n");  fflush(stdout);
	f = fopen("/tmp/lookzfoo", "w");
	lookz_Write(st, f, 0, 0);
	fclose(f);

	printf("Reading\n");  fflush(stdout);
	f = fopen("/tmp/lookzfoo", "r");
	st2 = lookz_New();
	lookz_Read(st2, f, 0);
	fclose(f);

	printdata(st2);
	lookz_Destroy(st2);

	/* the next file should be empty because the
		WriteID is the same as above */
	printf("\nReWriting with same WriteID\n");  fflush(stdout);
	f = fopen("/tmp/lookzfoo2", "w");
	lookz_Write(st, f, 0, 2);
	fclose(f);

	lookz_SetVisibility(st, FALSE);
	printf("\nWriting with headers\n");  fflush(stdout);
	f = fopen("/tmp/lookzfoo3", "w");
	lookz_Write(st, f, 1, 2);
	fclose(f);


	printf("Reading\n");  fflush(stdout);
	f = fopen("/tmp/lookzfoo3", "r");
	while (TRUE) 
		if((fgetc(f)) == '}') break;
	st2 = lookz_New();
	lookz_Read(st2, f, 3);
	fclose(f);

	printdata(st2);
}

printdata(st)
	register struct lookz *st;
{
	printf("Image is %s\n", (lookz_GetVisibility(st) ? "visible" : "hidden"));
	fflush(stdout);
}

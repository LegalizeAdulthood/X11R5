/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/music/RCS/playtest.c,v 1.6 1991/09/12 16:24:56 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/music/RCS/playtest.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/music/RCS/playtest.c,v 1.6 1991/09/12 16:24:56 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * P_R_P_Q_# (C) COPYRIGHT IBM CORPORATION 1987, 1988
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/*
playtest.c

	test the play package
*/


#include <stdio.h>
#include <class.h>
#include <text.ih>

#include <play.ih>	

#include <filetype.ih>
#include <attribs.h>

#define class_StaticEntriesOnly
#include <observe.ih>
#include <proctbl.ih>
#include <dataobj.ih>
#undef class_StaticEntriesOnly


struct text *text;	/* the source text */


main(argc, argv)
        char **argv;
{
	FILE *inputfile;

	long objectID;
	char *objectType;
	struct attributes *attributes;
	char *tuning = "E";
	boolean playtones = FALSE;

	char buf[500];
	long i, max;

	class_Init(".");		/* use current directory for dataobject path */

	observable_StaticEntry;
	proctable_StaticEntry;
	dataobject_StaticEntry;

	while (argc > 1  &&  *(argv[1]) == '-') {
		argv++;
		argc --;
		switch (*(argv[0]+1)) {
		case 't':	playtones = TRUE;   break;
		case 'E':  *tuning = 'E';  break;
		case 'J':  *tuning = 'J';  break;
		case 'M':  *tuning = 'M';  break;
		case 'P':  *tuning = 'P';  break;
		}
	}

	text = text_New();

	inputfile = 0;

	if (argc < 2 || (inputfile=fopen(argv[1], "r" )) == NULL) {
		printf("File %s not found\n", argv[1]);
	}

	attributes = NULL;
	if (inputfile && (objectType = filetype_Lookup(inputfile, argv[1], &objectID, &attributes))
				!= NULL) 
		/* NULL means use default object. Text in this case. */
		/* If not default, make sure that the object type of the file is compatible
		 * with text.  */
		if (!class_IsTypeByName(objectType, "text")) {
			fprintf(stderr, "File is not a text object, its a %s\n", objectType);
			exit(1);
		}
	if (attributes != NULL)
		/* Gets things like readonly and others. Omit if you don't need the
		 * attributes. (You can pass NULL into filetype_Lookup instead of the
		 * attributes parameter.  */
		text_SetAttributes(text, attributes);

	play_Volume(3);
	play_Tuning(tuning);
	play_Retard(0);

	printf("inputfile: %d\n", inputfile);

	if (inputfile && playtones) {
		/* each line of input file specifies a tone with
			frequency duration(msec) volume(0..9) */
		double freq;
		long dura, vol;
		while (fscanf(inputfile, "%lf %d %d", &freq, &dura, &vol) == 3) {
			play_Tone(freq, dura, vol);
		}
	}
	else if (inputfile) {
		text_Read(text, inputfile, objectID);
		max = text_GetLength(text);
		if (max > sizeof(buf) - 2) max = sizeof(buf) -2;
		for (i = 0; i < max; i ++)
			buf[i] = text_GetChar(text, i);
		buf[i] = '\0';
		play_Notes(buf);
	}
	else
		/* play three blind mice */
		play_Notes("T180 L2 E D C  P4     E D C P4  \
			G L4 F F L2 E P4    G L4 F F L2 E P4  \
			L4 G >C   C < B A B  >C<  G G P4  \
			G >C   C < B A B  >C<   G G P4  \
			G L1 E D C");
}

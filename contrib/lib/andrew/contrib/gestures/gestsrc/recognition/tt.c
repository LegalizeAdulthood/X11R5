/***********************************************************************

tt.c - Creates and evaluates classifiers made from mouse data

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

***********************************************************************/



/*
  gesture trainer and tester with no graphical I/O

  Uses:

	Read in the file of example gestures gesture.train and write the
	classifier into the file gesture.cl:

		tt -t gesture.train -c gesture.cl

	Read in gesture.train and evaluate the resulting classifier on the
	gestures in gesture.test:
		
		tt -t gesture.train -e gesture.test

	Read in gesture.cl and try the classifier on gesture.test

		tt -c gesture.cl -e gesture.test

*/

#include <math.h>
#include <stdio.h>
#include "util.h"
#include "bitvector.h"
#include "matrix.h"
#include "sc.h"
#include "gf.h"
#include "fv.h"
#include "args.h"
#include "zdebug.h"

char _zdebug_flag[128];
double kludge[128];
int nclosest = 0;

GestureFile traingf, evalgf;
char *trainfilename, *evalfilename;

sClassifier sC;
char *classifierfilename;

main(argc, argv)
char **argv;
{
	FILE *f;

	ArgLoop(argc, argv);

	if(trainfilename == NULL && classifierfilename == NULL)
		error("tt: must specify at least one of '-t file.train' or '-c file.cl'");

	if(trainfilename) {
		printf("Reading training file %s...", trainfilename);
		fflush(stdout);
		traingf = ReadGestureFile(trainfilename);
		Train();
		Z('c') sDumpClassifier(sC);
		printf("%d gestures read\n", traingf->ngestures);

		if(classifierfilename) {
			if((f = fopen(classifierfilename, "w")) == NULL)
				error("Can't open %s\n", classifierfilename);
			printf("Writing classifier file %s ",
				classifierfilename), fflush(stdout);
			sWrite(f, sC);
			fclose(f);
			printf("\n");
		}
	}
	else if(classifierfilename) {
		if((f = fopen(classifierfilename, "r")) == NULL)
			error("Can't open %s\n", classifierfilename);
		printf("Reading classifier file %s ", classifierfilename);
		fflush(stdout);
		sC = sRead(f);
		printf("\n");
	} 

	if(evalfilename) {
		printf("Reading evaluation file %s...", evalfilename);
		fflush(stdout);
		evalgf = ReadGestureFile(evalfilename);
		printf("%d gestures read\n", evalgf->ngestures);
	}
	else {
		evalfilename = trainfilename;
		evalgf = traingf;
	}

	if(evalgf) {
		printf("Evaluating gestures in file %s\n", evalfilename);
		Eval();
	}

	
	Z('L') Loop();
}

Train()
{
	register Gesture g;
	register Gpoint p;
	FV fv;

	sC = sNewClassifier();
	fv = FvAlloc();
	for(g = traingf->gesture;
	    g < &traingf->gesture[traingf->ngestures]; g++) {
		FvInit(fv);
		for(p = g->point; p < &g->point[g->npoints]; p++)
			FvAddPoint(fv, p->x, p->y, p->t);
		g->y = FvCalc(fv);
		Z('f') PrintVector(g->y, "%s: ", g->examplename);
		sAddExample(sC, ClassName(g), g->y, g->examplename);

	}
	sDoneAdding(sC);

	if(nclosest > 0) sDistances(sC, nclosest);
}

Eval()
{
	register Gesture g;
	register Gpoint p;
	register sClassDope scd, expectedscd;
	FV fv;
	int ntries = 0, nwrong = 0;

	fv = FvAlloc();
	for(g = evalgf->gesture;
	    g < &evalgf->gesture[evalgf->ngestures]; g++) {

		expectedscd = sClassNameLookup(sC, ClassName(g));
		if(expectedscd == NULL) {
			printf("Eval: Ignoring example %s\n", g->examplename);
			continue;
		}

		FvInit(fv);
		for(p = g->point; p < &g->point[g->npoints]; p++)
			FvAddPoint(fv, p->x, p->y, p->t);
		g->y = FvCalc(fv);

		Z('f') PrintVector(g->y, "%s: ", g->examplename);

		ntries++;
		scd = sClassify(sC, g->y);

		if(scd == NULL)
			error("Eval: rejected gesture (shouldn't happen)");

		if(scd != expectedscd) {
			Z('e') printf("%s(%s) misclassified as %s\n",
				g->examplename, ClassName(g), scd->name);
			nwrong++;
		}
	}
	if(ntries != 0)
	    printf("%d tried, %d wrong (%.1f%%)  RATE: %.1f%%\n", 
		ntries,
		nwrong, (100.0*nwrong)/ntries,
		(100.0 * (ntries-nwrong) ) / ntries);
	else
		printf("No examples to evaluate!\n");
}

ArgLoop(argc, argv)
char **argv;
{
	ARGLOOP
		STRINGARG('c')	classifierfilename = p;	ENDSTRINGARG
		STRINGARG('e')	evalfilename = p;	ENDSTRINGARG
		STRINGARG('t')	trainfilename = p;	ENDSTRINGARG

		STRINGARG('Z')	while(*p) _zdebug_flag[*p++]++; ENDSTRINGARG
		STRINGARG('z')	while(*p) _zdebug_flag[*p++]--; ENDSTRINGARG
		STRINGARG('K')	kludge[*p] = atof(p+1); ENDSTRINGARG
		STRINGARG('C')	nclosest = atoi(p);	ENDSTRINGARG
			
		BADARG
			fprintf(stderr, "tt: unknown option %c\n", *p);
			exit(2);
		ENDBADARG
	ENDARGLOOP

	{ extern double se_th_rolloff, dist_sq_threshold;
	  register int i, j, z;

	for(i = z = 0; i < 128; i++) {
		if(_zdebug_flag[i] && z==0)
			z++, printf("-Z");
		for(j = 0; j < _zdebug_flag[i]; j++)
			printf("%c", i);
	} if(z) printf("\n");

	if(kludge['R'] != 0.0)
		printf("Setting se_th_rolloff = %g\n", se_th_rolloff = kludge['R']);
	if(kludge['D'] != 0.0)
		printf("Setting dist_sq_threshold = %g\n", dist_sq_threshold = kludge['D']);
	}
}

Loop()
{
	char line[100];
	char *args[10], argspace[10][50];
	int argc;
	int i;

	for(;;) {

		printf("New args: ");
		gets(line);
		for(i = 0; i < 10; i++) args[i] = argspace[i];
		strcpy(args[0], "tt");
		argc = sscanf(line, "%s %s %s %s %s %s %s %s %s %s",
			args[1], args[2], args[3], args[4], args[6],
			args[7], args[8], args[9]) + 1;


		if(argc <= 1) break;

		ArgLoop(argc, args);

		printf("Training from %s\n", trainfilename);
		Train();
		printf("Evaluating from %s\n", evalfilename);
		Eval();
	}
}

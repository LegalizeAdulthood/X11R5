/***********************************************************************
/***********************************************************************

sc.h - creates classifiers from feature vectors of examples, as well as
   classifying example feature vectors.

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

***********************************************************************/


/*
 single path classifier
*/

#define	MAXSCLASSES	100

typedef struct sclassifier *sClassifier; /* single feature-vector classifier */
typedef int		    sClassIndex; /* single feature-vector class index */
typedef struct sclassdope  *sClassDope;	 /* single feature-vector class dope */

struct sclassdope {
	char 		*name;
	sClassIndex	number;
	Vector		average;
	Matrix		sumcov;
	int		nexamples;
};

struct sclassifier {
	int		nfeatures;
	int		nclasses;
	sClassDope	*classdope;

	Vector		cnst;	/* constant term of discrimination function */
	Vector		*w;	/* array of coefficient weights */
	Matrix		invavgcov; /* inverse covariance matrix */
};

sClassifier	sNewClassifier();
sClassifier	sRead();		/* FILE *f */
void		sWrite();		/* FILE *f; sClassifier sc; */
void	 	sFreeClassifier();	/* sc */
void		sAddExample();		/* sc, char *classname; Vector y */
void		sRemoveExample();	/* sc, classname, y */
void		sDoneAdding();		/* sc */
sClassDope	sClassify();		/* sc, y */
sClassDope	sClassifyAD();		/* sc, y, double *ap; double *dp */
sClassDope	sClassNameLookup();	/* sc, classname */
double		MahalanobisDistance();	/* Vector v, u; Matrix sigma */

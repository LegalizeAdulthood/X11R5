/* user code begins here for HeaderInfo */
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/controllers/RCS/pcontrol.ch,v 2.4 1989/02/17 15:18:56 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/controllers/RCS/pcontrol.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_pcontrol_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/controllers/RCS/pcontrol.ch,v 2.4 1989/02/17 15:18:56 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
/* user code ends here for HeaderInfo */

class pcontrol {
    classprocedures:
        InitializeClass() returns boolean;
/* user code begins here for classprocedures */
/* user code ends here for classprocedures */
data:
	struct value *kb_2;
	struct pianoV *kb_2View;
	struct value *kb_3;
	struct pianoV *kb_3View;
	struct value *replay;
	struct buttonV *replayView;
	struct value *volume;
	struct fourwayV *volumeView;
	struct value *nt_10;
	struct buttonV *nt_10View;
	struct value *clear;
	struct buttonV *clearView;
	struct value *nt_11;
	struct buttonV *nt_11View;
	struct value *nt_0;
	struct buttonV *nt_0View;
	struct value *nt_1;
	struct buttonV *nt_1View;
	struct value *mode;
	struct onoffV *modeView;
	struct value *nt_2;
	struct buttonV *nt_2View;
	struct value *nt_3;
	struct buttonV *nt_3View;
	struct value *nt_4;
	struct buttonV *nt_4View;
	struct value *nt_5;
	struct buttonV *nt_5View;
	struct value *nt_6;
	struct buttonV *nt_6View;
	struct value *nt_7;
	struct buttonV *nt_7View;
	struct value *nt_8;
	struct buttonV *nt_8View;
	struct value *nt_9;
	struct buttonV *nt_9View;
	struct value *speed;
	struct sliderV *speedView;
	struct value *Read;
	struct buttonV *ReadView;
	struct value *noteout;
	struct stringV *noteoutView;
	struct value *undo;
	struct buttonV *undoView;
	struct text *score;
	struct textview *scoreView;
	struct arbiter *ab1;
	struct arbiterview *ab1View;
	struct lset *ab2;
	struct lsetview *ab2View;
	struct value *rest;
	struct buttonV *restView;
	struct value *kb_0;
	struct pianoV *kb_0View;
	struct value *Save;
	struct buttonV *SaveView;
	struct value *kb_1;
	struct pianoV *kb_1View;
/* user code begins here for classdata */
	struct value *duration;
	long lastnoteval,lastlen;
/* user code ends here for classdata */
	struct view *v;
	struct pcontrol *next;
};

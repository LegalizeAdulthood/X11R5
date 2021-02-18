/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftext.h,v 2.7 1991/09/18 23:22:26 gk5g Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftext.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftext.h,v 2.7 1991/09/18 23:22:26 gk5g Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifndef AIX
/* type checking */
extern int SetIndent(/*struct rofftext *self,int u*/);
extern int SetTempIndent(/*struct rofftext *self,int u*/);
extern int Is_BOL(/*struct rofftext *self*/);
extern int SetBOL(/*struct rofftext *self*/);
extern int DestroyContext(/*IC c*/);
extern Trickle topen(/*struct rofftext *self, FILE *f, char *s*/);
extern int tclose(/*struct rofftext *self, Trickle t*/);
extern int g(/*struct rofftext *self, Trickle t*/);
extern int ung(/*struct rofftext *self,char c,Trickle t*/);
extern int tpush(/*struct rofftext *self, Trickle t, FILE *f, char *s, boolean push, int argc, char *argv[]*/);
extern int munch(/*struct rofftext *self, Trickle t*/);
extern int special(/*struct rofftext *self,Trickle t*/);
extern int setfont(/*struct rofftext *self, Trickle t*/);
extern int getwidth(/*struct rofftext *self, Trickle t*/);
extern int munchmove(/*struct rofftext *self, Trickle t*/);
extern int getname(/*struct rofftext *self, Trickle t, char *name*/);
extern char *getregister(/*struct rofftext *self, Trickle t*/);
extern int putregister(/*struct rofftext *self, char *name, double value, enum RegFmt fmt, double inc, boolean relative*/);
extern char *getstring(/*struct rofftext *self, char *name*/);
extern int putstring(/*struct rofftext *self, char *name, char *value*/);
extern int getarg(/*struct rofftext *self, Trickle t, char *buf, int n, boolean copymode*/);
extern int put(/*struct rofftext *self, char c*/);
extern int DoBreak(/*struct rofftext *self*/);
extern int get(/*struct rofftext *self, Trickle t*/);
extern int DoCommand(/*struct rofftext *self, Trickle t, char *name, boolean br*/);
extern int Scan(/*struct rofftext *self, Trickle t, char *cmd*/);
#endif /* AIX */

/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/nbutterv.ch,v 1.1 1991/09/27 20:36:16 rr2b Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/nbutterv.ch,v $ */

class nbutterv : sbuttonv {
overrides:
    Touch(int ind, enum view_MouseAction action) returns boolean;
};


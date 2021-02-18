/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/environ.ch,v 2.6 1991/09/12 19:21:40 bobg Exp $ */
/* $ACIS:environ.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/environ.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidenviron_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/environ.ch,v 2.6 1991/09/12 19:21:40 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* environ.H
 * Class declarations for environment and preference maodule.
 *
 */

package environ {
    classprocedures:
        Put(char *variable, char *value);
        Delete(char *variable);
        Get(char *variable) returns char *;
        GetProfile(char *preference) returns char *;
        GetProfileSwitch(char *preference, boolean defaultValue) returns boolean;
	GetProfileInt(char *preference, long defaultValue) returns long;
	ProfileEntryExists(char *preference, boolean useDefault) returns boolean;
	GetConfiguration(char *key) returns char *;
	AndrewDir(char *str) returns char *;
	LocalDir(char *str) returns char *;
	ReadConfigureFile(char *filename) returns struct configurelist *;
	GetConfig(struct configurelist *cList, char *key, boolean usedefault) returns char *;
	FreeConfigureList(struct configurelist *cList);
	GetHome(char *user) returns char *;  /* pass in NULL for current user */
	SetProgramName(char *string);	    /* for use by the GetProfile stuff */
};

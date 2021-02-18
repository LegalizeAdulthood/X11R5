/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#ifdef sun
#define profile_STATICBASE	0x8000
#endif /* sun */
#ifdef vax
#define profile_STATICBASE	2
#endif /* vax */
#ifdef ibm032
#define profile_STATICBASE	0
#endif /* ibm032 */

#ifndef profile_STATICBASE
#define	profile_STATICBASE	2
#endif /* profile_STATICBASE */

package profile {
    classprocedures:
	Start(char *textbase, long textlength, char *filename) returns int;
	StartClass(char *classname, char *filename) returns int;
	Stop() returns int;
	Active() returns int;
    macros:
        GetStaticBase() (profile_STATICBASE)
};

#define profile_DEFAULTFILE "gmon.out"

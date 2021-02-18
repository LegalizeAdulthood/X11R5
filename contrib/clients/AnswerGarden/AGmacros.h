
/************************************************

  AGMACROS.H

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  Copyright 1991 Massachusetts Institute of Technology.  Answer Garden is
  a trademark of the Massachusetts Institute of Technology.  For additional
  information on copyright and trademark, see AG.c.

  These are for the Suns of the world.

************************************************/
#ifndef _AGmacros_h
#define _AGmacros_h

  /* Xos will go out and pick up either string.h or strings.h.  If SYSV,
     then index and rindex will be set correctly */
#include <X11/Xos.h> 

#define AGindex(ptr,chr)  index(ptr,chr)

#define StringEqual(s1,s2)  (!AGstrcmp(s1,s2))
#define StringNotEqual(s1,s2) (AGstrcmp(s1,s2))
		    

/* Saber requires the "!= NULL" check - should remove later. */

#define AGstrlen(string)  ((string != NULL) ? strlen(string) : 0)

#define AGstrcmp(s1,s2)  ((s2 != NULL) ? \
			  (  (s1 != NULL) ?  strcmp(s1,s2) : -1 ) : \
			  (  (s1 != NULL) ?  1 : 0 ) \
			  )

#define AGstrcpy(s1,s2)  ((s2 != NULL) ? strcpy(s1,s2) : strcpy(s1,"") )
#define AGstrncpy(s1,s2,n)  ((s2 != NULL) ? strncpy(s1,s2,n) : strncpy(s1,"",1) )
#define AGstrcat(s1,s2)  ((s2 != NULL) ? strcat(s1,s2) : strcat(s1,""))

#define AGMakeString(string,first_char,end_char) \
    { \
	    AGstrncpy(string,first_char,end_char+1-first_char); \
	    string[(int)(end_char+1-first_char)] = EOS; \
	    }






#ifdef NEXT
#define AGstrlen(string)  strlen(string)

#define AGstrcmp(s1,s2)  strcmp(s1,s2)

#define AGstrcpy(s1,s2)  strcpy(s1,s2)

#endif


#endif /*_AGmacros_h*/



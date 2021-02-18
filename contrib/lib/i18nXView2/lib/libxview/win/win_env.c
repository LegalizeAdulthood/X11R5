#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)win_env.c 20.12 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Implement the win_environ.h & win_sig.h interfaces. (see win_ttyenv.c for
 * other functions)
 */

#include <xview/rect.h>
#include <xview/win_env.h>
#include <xview/win_struct.h>

/*
 * Public routines
 */
we_setparentwindow(windevname)
    char           *windevname;
{
    (void) setenv(WE_PARENT, windevname);
}

int
we_getparentwindow(windevname)
    char           *windevname;
{
    return (_we_setstrfromenvironment(WE_PARENT, windevname));
}
 
we_setgfxwindow(windevname)
    char           *windevname;
{
    (void) setenv(WE_GFX, windevname);
}

int
we_getgfxwindow(windevname)
    char           *windevname;
{
    return (_we_setstrfromenvironment(WE_GFX, windevname));
}

we_setinitdata(initialrect, initialsavedrect, iconic) 
    struct rect    *initialrect, *initialsavedrect; 
    int             iconic; 
{ 
    char            rectstr[60]; 
  
    rectstr[0] = '\0'; 
    (void) sprintf(rectstr, "%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04ld", 
                   initialrect->r_left, initialrect->r_top, 
                   initialrect->r_width, initialrect->r_height, 
                   initialsavedrect->r_left, initialsavedrect->r_top, 
                   initialsavedrect->r_width, initialsavedrect->r_height, 
                   iconic); 
    (void) setenv(WE_INITIALDATA, rectstr); 
}       
  
int 
we_getinitdata(initialrect, initialsavedrect, iconic) 
    struct rect    *initialrect, *initialsavedrect; 
    int            *iconic; 
{ 
    char            rectstr[60]; 
  
    if (_we_setstrfromenvironment(WE_INITIALDATA, rectstr)) 
        return (-1); 
    else { 
        if (sscanf(rectstr, "%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd", 
                   &initialrect->r_left, &initialrect->r_top, 
                   &initialrect->r_width, &initialrect->r_height, 
                   &initialsavedrect->r_left, &initialsavedrect->r_top, 
                   &initialsavedrect->r_width, &initialsavedrect->r_height, 
                   iconic) != 9) 
            return (-1); 
        return (0); 
    }    
}
 
int
_we_setstrfromenvironment(tag, target)
    char           *tag, *target;
{
    char           *en_str, *getenv();
 
    *target = 0;
    if (en_str = getenv(tag)) {
        (void) strcat(target, en_str);
        return (0);
    } else {
        return (-1);
    }
}

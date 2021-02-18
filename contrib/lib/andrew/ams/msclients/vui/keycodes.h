/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/keycodes.h,v 2.5 1991/09/12 19:17:21 bobg Exp $ */

#ifdef IBMPC

#define KEYCODE_LEFT        (256*75)
#define KEYCODE_RIGHT       (256*77)
#define KEYCODE_UP          (256*72)
#define KEYCODE_DOWN        (256*80)
#define KEYCODE_TAB         (256*15)+9
#define KEYCODE_CTRL_TAB    (256*116)
#define KEYCODE_SHIFT_TAB   (256*15)
#define KEYCODE_CTRL_LEFT   (256*115)
#define KEYCODE_RETURN      (256*28)+13
#define KEYCODE_FAKERETURN  (256*28)+13
#define KEYCODE_INSERT      (256*82)
#define KEYCODE_DELETE      (256*83)
#define KEYCODE_HOME        (256*71)
#define KEYCODE_END         (256*79)
#define KEYCODE_BACKSPACE   (256*14)+8
#define KEYCODE_DEL         (256*14)+8	/* this matches BS and will be ignored */
#define KEYCODE_CTRL_END    (256*117)
#define KEYCODE_ESCAPE      (256+27)
#define KEYCODE_CTRL_HOME   (256*119)
#define KEYCODE_PAGE_DOWN   (256*81)
#define KEYCODE_PAGE_UP     (256*73)
#define KEYCODE_F1          (256*59)
#define KEYCODE_F2          (256*60)
#define	KEYCODE_F3	    (256*61)
#define KEYCODE_ALT_F1      (256*104)
#define KEYCODE_REDRAW	    (256*1)+1	/* mas V1.3 : this key value is fake! */

#else /* IBMPC */

#define KEYCODE_LEFT	    (2)
#define KEYCODE_RIGHT	    (6)
#define KEYCODE_UP	    (16)
#define KEYCODE_DOWN	    (14)
#define KEYCODE_TAB	    (9)
#define KEYCODE_CTRL_TAB    (256 + 9)
#define KEYCODE_SHIFT_TAB   (256 + 2)
#define KEYCODE_CTRL_LEFT   (256 + 2)
#define KEYCODE_RETURN	    (13)
#define KEYCODE_FAKERETURN  (10)
#define KEYCODE_INSERT	    (15)
#define KEYCODE_DELETE	    (4)
#define KEYCODE_HOME	    (1)
#define KEYCODE_END	    (5)
#define KEYCODE_BACKSPACE   (8)
#define KEYCODE_DEL	    (127)
#define KEYCODE_CTRL_END    (11)
#define KEYCODE_ESCAPE	    (7)
#define KEYCODE_CTRL_HOME   (256 + '<')
#define KEYCODE_PAGE_DOWN   (22)
#define KEYCODE_PAGE_UP	    (256 + 'v')
#define KEYCODE_F1	    (256 + '1')
#define KEYCODE_F2	    (256 + '2')
#define	KEYCODE_F3	    (256 + '3')
#define KEYCODE_ALT_F1	    (256 + '!')
#define KEYCODE_REDRAW	    (12)

#endif /* IBMPC */

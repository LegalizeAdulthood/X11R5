/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/**************************************************************************\
* 									   *
* 			   @@@@@@  @       @ @       @			   *
* 			  @      @ @       @ @@     @@			   *
* 			 @         @   @   @ @ @   @ @			   *
* 			 @    @@@@ @  @ @  @ @  @ @  @			   *
* 			 @       @ @ @   @ @ @   @   @			   *
* 			  @     @@ @@     @@ @       @			   *
* 			   @@@@@ @ @       @ @       @			   *
* 									   *
*  _    ,								   *
* ' )  /      _/_                   /)					   *
*  /--/ o _   /  __,_  , ,     ____//     __  _ , __o  _   o ______   _	   *
* /  (_(_/_)_(__(_) (_(_/_    (_) /__    / (_(<_\/ (__/_)_(_(_) /) )_/_)_  *
*                      /         />					   *
*                     '         </					   *
* 									   *
\**************************************************************************/

static char           *RCS_Version = "$GwmVersion: 1.7h $";

/* Here is the log of all revisions 
 * NOTE: Revision numbers are non-significant, use Version numbers in all mail.
 */
/* $Log: gwm.shar,v $
 * Revision 1.57  1991/10/04  13:37:58  colas
 * *** Version 1.7h ***
 * ***** R5 TAPE VERSION *****
 * uses xpm v3.0
 * icons with no center plug resize when bars resize Mike Liang <mike@lsil.com>
 *
 * Revision 1.56  1991/10/02  17:04:16  colas
 * *** Version 1.7g ***
 * bug in error message in undefined key (Philippe Kaplan)
 * xpm/Imakefile corrections by Pekka.Nikander@ngs.fi
 * bug with some false window-group hints
 * data/amc-lisp.el copyright changed to X one, with author agreement
 * Doug Bogia patches for resize into upper-left of screen
 * confined resize problems fixed
 * blk@zoot.cca.cr.rockwell.com (Barry L. Kaplan) GWM-Lisp major mode for Emacs
 *   included in distrib
 * Hugues.Leroy@irisa.fr (Hugues Leroy) additions of rxcmdtool to rxterm.script
 * PROBLEMS file for listing hints & tips
 *
 * Revision 1.55  1991/09/13  08:21:32  colas
 * *** Version 1.7f ***
 * (send-key-to-window "aB@#" alone) works as expected
 *
 * Revision 1.54  1991/09/13  08:12:58  colas
 * *** Version 1.7f ***
 * Upgrade of xpm (3.0b) to read xpm v2 pixmaps
 * Do trap bad transient-for windows
 * blk@zoot.cca.cr.rockwell.com (Barry L. Kaplan) bug in submenu execution in mwm
 *
 * Revision 1.53  1991/09/10  12:43:52  colas
 * small bug in doc
 *
 * Revision 1.51  1991/09/10  12:21:38  colas
 * XPM v3.0a library included
 *
 * Revision 1.50  1991/09/10  09:17:51  colas
 * malloc.c from wool.
 * no more resource manager bugs. were due to ...
 * no more XIOError trapping.
 *
 * Revision 1.49  1991/09/06  15:28:39  colas
 * xpm/FILES included
 * color-make-rgb function added by dab@berserkly.cray.com (David Borman)
 * do not use bzero anymore on sparc
 *
 * Revision 1.48  1991/08/29  11:24:56  colas
 * *** Version 1.7d ***
 * Transient-for windows buggy
 *
 * Revision 1.47  1991/08/28  15:10:52  colas
 * *** Version 1.7c ***
 * Compiles with R5
 * John Carr <jfc@ATHENA.MIT.EDU> patches for IBM RT and RS/6000, X11R5, ansiC
 * more careful during window decoration (for xrn crashes)
 * Sun4 bugs corrected
 * window-to-client and client-to-window functions
 *
 * Revision 1.46  1991/08/22  07:28:48  colas
 * machine names truncated to first dot
 *
 * Revision 1.45  1991/08/20  08:14:32  colas
 * bug in flushing grab events queue
 *
 * Revision 1.44  1991/08/15  16:44:15  colas
 * patches to README by MAP@lcs.mit.edu (Michael A. Patton) + revised myself
 * window-icon? documented and coded in C
 *
 * Revision 1.43  1991/08/15  14:40:10  colas
 * *** Version 1.7a ***
 * window-is-valid & wob-is-valid functions
 * data/Imakefile repaired
 * NOXEHTYPE flag for pre-R4 includes
 * make wool makes a command-line interpreter version
 * dvrooms additions by Dwight Shih <dwight@s1.gov>
 *
 * Revision 1.42  1991/08/14  09:37:31  colas
 * *** 1.7 ***
 * distrib details
 *
 * Revision 1.41  1991/08/13  17:11:45  colas
 * *** 1.7 ***
 * small bugs
 *
 * Revision 1.40  1991/08/07  14:54:20  colas
 * (draw-text pixmap x y font text)
 * bug in redecorating windows in groups fixed
 * profile-colas.gwm included
 *
 * Revision 1.39  1991/07/29  14:57:30  colas
 * *** 1.7_epsilon ***
 *
 * Revision 1.38  1991/07/29  13:35:11  colas
 * place-button with 3D-look
 * null string for the window-client-name badly matched (patch by Doug Bogia)
 * no-set-focus not always set properly (patch by Doug Bogia)
 * amc-lisp.el emacs style included
 * data/Imakefile automatically generated
 * fixed crash when client quits immediately after a colormap change request
 *     (problem showed by Frank Mangin <mangin@sa.inria.fr>)
 * no more use of makedepend in makefile
 * draw-rectangle
 * color-free
 * no need to put pressed button as modifier for release events anymore
 * double-buttonpress was bugged: acted as a buttonpress
 *
 * Revision 1.37  1991/06/24  11:50:08  colas
 * madler@apollo.com (Michael Adler) dims_outer_to_inner() in client.c:
 *     failed to account for the border width of the decoration
 * Eyvind Ness <eyvind@hrp.no> near-mouse placement policy included in file
 *     near-mouse.gwm
 * mods to isolate wool from gwm
 * client moving only one coord would move to 0
 * xid-to-wob function
 * delete-read-properties pointer to delete x properties after reading them
 * GWM_EXECUTE gets deleted after being read
 * client-name context var was ignored in place-menu
 * client-info menu wasn't movable
 * new place-button in utils.gwm
 *
 * Revision 1.36  1991/04/29  16:17:03  colas
 * window-icon checks for window existence
 *
 * Revision 1.35  1991/04/11  15:02:25  colas
 * (= "foo" 'foo) didn't work!
 * good resource for host names with dots in them DGREEN@IBM.COM (Dan R.Greening)
 * proper use of * in resource specs
 * uses xpm library v3.0a
 * tar archive includes gwm dir (tar c gwm instead of tar c .)
 *
 * Revision 1.34  1991/04/05  14:25:17  colas
 * William.Lott@ARDATH.SLISP.CS.CMU.EDU: (wob-borderpixel <pixmap>) broken
 * restart code fixed also in mwm.gwm and twm.gwm
 * keys works in vscreen window now
 * no-decoration.gwm wasn't included
 * John Carr patches for IBM PC-RTs under BSD (AOS), not AIX
 * Mike Newton patches for building on DG AViiON
 * Dan R. Greening patches to Imakefile for IBM RS-6000
 * sven@tde.lth.se (Sven Mattisson) patches for Sequent Symmetry DYNIX 3.0.17
 * meters stay in place
 *
 * Revision 1.33  91/01/11  17:03:13  colas
 * John Carr <jfc@ATHENA.MIT.EDU>: fixes for correct argument types in functions
 * 
 * Revision 1.32  91/01/09  14:25:35  colas
 * Jay Berkenbilt <qjb@ATHENA.MIT.EDU>: closing code executed at the end/restart
 *     of gwm was just value of closing, causing random crashes
 * icon-groups-old.gwm  simple-icon-old.gwm  simple-win-old.gwm files were
 *     missing from distrib (for mwm and twm profiles)
 * 
 * Revision 1.31  91/01/08  18:30:58  colas
 * av@cs.uta.fi (Arto V. Viitanen) ids too long for appolo C compiler
 * Mike Newton: new version of mon-keys.gwm
 * iconification of clients not matched by describe-window failed
 * Mike Newton: pop menu on root with any modifier on menu button
 * Mike Newton: patches to gwm_utils & compile flags for Data General AViiON
 * tom@srl.mew.mei.co.jp (Tom Borgstrom): OBJS1 in Imakefile
 * Malloc Bug in xpm librairy for monochrome pixmaps
 * 
 * Revision 1.30  91/01/02  16:08:15  colas
 * *** Version 1.7_gamma ***
 * basebars on shaped windows was buggy
 * John Mackin <john@cs.su.oz.au> corrections to the man page
 * send-key-to-window adds automatically shift as modifier if letter is uppercase
 *     Frank Mangin <Frank.Mangin@mirsa.inria.fr>
 * CHANGES file updated (no doc yet)
 * 
 * Revision 1.29  90/12/27  18:47:43  colas
 * new events enter-window-not-from-grab & leave-window-not-from-grab
 * default action for menus now works
 * random bus error bug in redecorate window fixed
 * menu labels can now be pixmaps or lists (evaluated)
 * "\xXX" was bugged
 * new color-components function
 * 
 * Revision 1.28  90/12/26  20:40:04  colas
 * 1.7_beta version
 * 
 * Revision 1.27  90/12/26  20:17:57  colas
 * icon-groups-old.gwm included in distrib
 * quick & dirty patches to twm & mwm modes to run with 1.7 standard profile
 * reparse-standard-behaviors simplified
 * xpm v2.8 library included
 * colors in pixmap-load can be either string or pixel value (number)
 * qjb vscreen package
 * now you must include standard-behavior in your fsms, is not included anymore
 *     in window/icon-behavior
 * 
 * Revision 1.26  90/12/13  18:48:51  colas
 * rxterm.script recognize full internet adresses
 * rotate-cut-buffer was bugged
 * 
 * Revision 1.25  90/12/11  17:58:07  colas
 * Wool bug: (boundp '()) was returning ()!
 * new "customize" function to generically redefine decos by context
 * set-icon now works again
 * "client info" menu item pops up a window
 * try to trap bus errors in current-mouse-pos (in move-opaque)
 * clients with window-icon started iconic before gwm starts work now OK
 *   (bug report: John Mackin <john@cs.su.oz.au>, David Hogan <dhog@cs.su.oz.au>)
 * 
 * Revision 1.24  90/11/29  17:10:45  colas
 * setting window-size bugged on windows which diddn't have resize incr hints
 * warning: float package makes redecorate on an icon crash
 * (pixmap-load filename [symbol1 color1 ... symbolN colorN])
 * icon-groups add iconify others in group
 * icon boxes of phk
 * all resources uses the class.name.name.machine resource spec
 * trace package included
 * raise-on-move, -resize and -iconify
 * make-string-usable-for-resource-key traps now blanks as well as . *
 * decos are evaluated ONLY at decoration time, not at profile read time
 * decos are recursively evaluated till obtaining a window description
 * do not call opening code on unmapped window group leaders
 * 
 * Revision 1.23  90/11/13  16:44:21  colas
 * the propagate argument to send-user-event stayed set after function invocation
 * reads xpm v2 pixmap files
 * 
 * Revision 1.22  90/11/08  19:03:23  colas
 * menu-make skips nil entries
 * float up or down
 * was mis-interpreting some WM hints
 * didn't handle resizes or move too early after initial mapping (this one was
 *        a tought one!)
 * multi-item entries in menus
 * menu code reorganized
 * simple-icon and term-icon customizable
 * traps Program sizes <= 1 for this brain-dead motif 1.1!
 * could crash if window was destroyed too fast
 * default menu action triggered if clicked too fast for menu to pop
 * 
 * Revision 1.21  90/10/22  11:52:47  colas
 * removed Scott Blachowicz's modifs to placement.gwm (bus errors?)
 * duanev@mcc.com (Duane Voth) buttonrelease inst. of press on icon of s-ed-ed-win
 * simple-win title stayed in gray
 * Rod Whitby: floating windows (was called auto-raise) (float)
 * 	    unconfined-move (unconf-move)
 * 	    F5 and F7 (suntools-keys)
 * 
 * Revision 1.20  90/08/02  14:39:35  colas
 * Scott Blachowicz  new patches to placements.gwm
 * 
 * Revision 1.19  90/07/26  18:16:02  colas
 * Carl Witty <cwitty@portia.stanford.edu> patches included:
 *     confined windows could get 1 pixel off the right & down of screen
 *     mwm profile:
 *         f.maximize corrected:
 *             If executed from a menu, it didn't correctly change the
 * 	    state of the window's zoom button
 * 	    It did set the zoom button's pixmap incorrectly, although
 * 	    this was corrected as soon as a leave-window event is generated.
 * 	keyboard menu traversal more sensible
 * 	new functions:
 * 	    (f.eval (? "Hello")) will print hello when it is executed.
 * 	    (f.identify) pops up a window which giving information about the
 * 	    current window.
 * 	    (f.delete) uses the WM_DELETE_WINDOW protocol to delete a window.
 * 	    (f.load "mwm") will reload the mwm profile.
 * 	    (f.raise_lower) is my version, which doesn't raise a window unless
 * 	    another on-screen window actually obscures it.
 * 	    (f.raise_move) is obvious.
 * 	    (f.refresh_win) didn't do anything before; now it does.
 * 	    (f.warpto "emacs") warps the mouse to the first emacs window found
 * 	    and brings it to the front.  I implemented it for a friend who 
 * 	    likes twm.
 * 	I didn't like the look of disabled menu items in the mwm profile, with
 * 	    that line drawn through them, so I added some modifications to 
 * 	    allow disabled menu items to be displayed in a different color
 * 	    instead.
 *     Here are some patches to make the icon manager in the twm profile act
 *         more like the one in the X11R4 twm.  That is, when the cursor is in
 * 	the icon manager, the focus is set to the window whose bar the cursor
 * 	is over.  The patches also change it so that the icon manager doesn't
 * 	resize when a window is iconified or deiconified.
 * 
 * Revision 1.18  90/07/26  17:10:08  colas
 * John Carr <jfc@ATHENA.MIT.EDU>: redecorating windows have no background
 *     (was not added last time)
 * dvroom.auto-add
 * XGetProperties were not tested enough for failure: pb of iconification
 * (exit insts...) were evaluating inst after restoring old context!
 * exit out of a for didn't restore correcetly the old values
 * screen-tile & icon-pixmap are defaulted-to intead of set
 * select for events sooner on windows, should not loose early resize events
 * casey@gauss.llnl.gov (Casey Leedom): namespace doc entries out of order
 * Scott Blachowicz <scott%grlab%hpubvwa@beaver.cs.washington.edu>
 *     patch to placements.gwm for decreasing lists
 * 
 * Revision 1.17  90/05/28  16:04:36  colas
 * *** Version 1.6b ***
 * Jay Berkenbilt <qjb@ATHENA.MIT.EDU> east gravities bugged
 * MW_STATE now overrides initial_state, even in "normal" case
 * John Carr <jfc@ATHENA.MIT.EDU>: redecorating windows have no background
 * 
 * Revision 1.16  90/05/23  18:06:18  colas
 * duanev@mcc.com (Duane Voth): dvrooms maintain a GWM_ROOM window property 
 *     on window to put them back in room managers 
 * mwm profile: closing a xmh sub window was generating X errors
 * button 2 in icon of simple-ed-win works now 
 *     (Andrew Simms <ams@acm.Princeton.EDU>)
 * tmiller@suna0.cs.uiuc.edu withdrawing a client having an "user-icon" set
 *     didn't work (such as a dvroom)
 * 
 * Revision 1.15  90/05/21  10:57:55  colas
 * Rod Whitby <rwhitby@austek.oz.au> added the variable "confine-windows" to keep
 *     interactive moves and resizes confined to screen boundaries.
 * added confine-windows support to move-opaque
 * More Muller patches to mwm emulation code
 * Rod Whitby <rwhitby@austek.oz.au> xterm-list more defaultable
 * cwitty@portia.stanford.edu reallocating when adding strings could fail if
 *     new_size was > 2*old_size
 * when match fails, returns () when caleed with 2args, "" otherwise.
 * Doug Bogia <bogia@suna0.cs.uiuc.edu> The no-set-focus did not allow the 
 *     autocolormap to work.
 * new global values class-name & client-name for class of placed menus
 * 
 * Revision 1.14  90/05/09  15:18:47  colas
 * *** Version 1.6a ***
 * icon-groups: new members always started iconic if leader had -iconic flag
 * deplibs added in Imakefile (hleroy@irisa.fr (Hugues Leroy))
 * compil without USER_DEBUG dacseg@uts.amdahl.com (Scott E. Garfinkle)
 * icon description evalution was done too early
 * bus errors trapped Ray Nickson <Ray.Nickson@comp.vuw.ac.nz>
 * mult-screen confusion fixed <eirik@elf.TN.Cornell.EDU>
 * map on an alerady mapped leader bug <carroll@suna0.cs.uiuc.edu>
 * WM_CLIENT_MACHINE was atom inst.of string Carl Witty <cwitty@cs.stanford.edu>
 * Dana Chee <dana@bellcore.com> bitmaps stipple were offset
 * Dana Chee blank pixmaps in mwm icons
 * Jay Berkenbilt <qjb@ATHENA.MIT.EDU> replacing menu inside screen was
 *     forgetting borders
 * could drop resize events Simon Kaplan <kaplan@cs.uiuc.edu>
 * bug in destroying placed menus fixed Carl Witty <cwitty@portia.stanford.edu>
 * (pop-menu 'here) was buggy not work (--> no cascading mwm menus)
 * kill gwm was doing a fatal error, quits now.
 * 
 * Revision 1.13  90/03/30  15:45:55  colas
 * *** Version 1.6 ***
 * could unmap group leader on startup
 * DecorateWindow was resetting TargetWob
 * bar with nothing didn't took size of pixmap (Gilles.Muller@irisa.fr)
 * icon group works on clienst iconified on stratup
 * iconify on startup calls (iconify-window)
 * icon-groups.excluded
 * 
 * Revision 1.12  90/03/28  18:42:11  colas
 * Gilles.Muller@irisa.fr: mwm error when closing xmh acc. window
 * mwm bugs when closing xmh sub-windows
 * can put a 'expr. in set-icon (Steve Anderson <anderson@sapir.cog.jhu.edu>)
 * do not map withdrawn windows on ending
 * doc was saying 'fixed instead of 'here. doc corrected.
 * deltabutton
 * 
 * Revision 1.11  90/03/23  18:54:18  colas
 * bug repaired in ref count: Vincent Bouthors <vincent@mirsa.inria.fr>
 *    (: a (:b 20)) (with (a 0) (: b 0)) would free the number 20
 * OpenWindow could modify the current window
 * withdrawing a window could destroy it
 * no user-positioning for transient windows cwitty@CSLI.STANFORD.EDU(Carl Witty)
 * traps bogus inc hints
 * 
 * Revision 1.10  90/03/20  16:10:56  colas
 * bar-min-width was ignored (Carl Witty <cwitty@portia.stanford.edu>)
 * shaped windows were offset by window borderwidth
 *     (Carl Witty <cwitty@portia.stanford.edu>)
 * simple-icon wasn't working in multi screen mode (found by
 *     Doug Bogia (bogia@cs.uiuc.edu))
 * changing the name after editing window name left in black/black
 *     Doug Bogia (bogia@cs.uiuc.edu)
 * window-icon-pixmap-id
 * patch to the README file by casey@gauss.llnl.gov (Casey Leedom)
 * or returned an un-evaluted argument!
 * new sort-icons and (rows.limits 'sort func)
 * reparse-standard-behaviors
 * action-button, select-button, menu-button
 * current-event-... return 0 (instead of ()) if event wasn't relevant
 * 
 * Revision 1.9  90/03/12  17:34:08  colas
 * Doug Bogia <bogia@cs.uiuc.edu>: patches to the no-set-focus code
 * 
 * Revision 1.8  90/03/07  18:14:51  colas
 * *** Version 1.5e ***
 * zoom-win.gwm renamed as mwm-zoom-win.gwm
 * GetWMNormalHints used
 * no more use of obsolete flags
 * 
 * Revision 1.7  90/02/28  21:52:02  colas
 * *** Version 1.5d ***
 * arup's patches to twm
 * doc of simple-icon.legend (false is now default)
 * if placed a menu before wool_host_name was referenced, crash
 * move-opaque part of distrib
 * bugs in mwm
 * better mwm menus
 * mwm de-iconify raises
 * 
 * Revision 1.6  90/02/21  15:18:06  colas
 * *** Version 1.5c ***
 * window-icon checked for existence
 * machine name set on menus
 * new hostname active value
 * elapsed-time returns real time in milliseconds
 * mwm was asking for placement of user-positioned windows
 * window-icon? was bugged
 * mwm didn't set colormaps
 * 
 * Revision 1.5.1.9  90/02/15  19:54:36  colas
 * *** Version 1.5b ***
 * Rod Whitby <rwhitby@adl.austek.oz.au> : visibility events
 * new naming with $Version: (in file gwm.shar,n)
 * GWM_EXECUTE built in
 * make-string-usable-for-resource-key (no more in window-name)
 * GWM_RUNNING property and GwmWindow hidden window
 * mwm profile gets resources by class and show window icon names on icons
 * 
 * Revision 1.5.1.8  90/02/14  17:59:40  colas
 * icon-groups could make windows disappear
 * restart with icon windows works now.
 * -r for retrying, waiting for you to kill other wm
 * menu popping could break (parent kept being set to old window)
 * do not take into account icon windows in window groups
 *     Michael Urban <urban%rcc@rand.org>
 * only check for WM_STATE on startup rjt@sedist.cray.com (Randy Thomas)
 * nice man page by casey@gauss.llnl.gov (Casey Leedom) (gwm.man)
 * delete-window no longer have the side-effect of changing current wob
 * standard profile: icons update names on window-icon-name change
 * window-icon-name on root was crashing gwm
 * all register decls removed
 * 
 * Revision 1.5.1.7  90/02/01  14:52:27  colas
 * re-install colormap if client changes its WM_COLORMAP_WINDOWS list
 * casey@gauss.llnl.gov (Casey Leedom) patches to the Imakefile
 * dummy man page
 * 
 * 
 * Revision 1.5.1.6  90/02/01  11:16:07  colas
 * patch for interactive386 by drich%dialogic@uunet.UU.NET (Dan Rich)
 * patch from John Carr <jfc@ATHENA.MIT.EDU> for keeping same screen
 *     during opening & close 
 * term-icon as idraw drawings
 * juha@tds.kth.se (Juha Sarlin): bugs in RCSRevNum in doc
 * 
 * Revision 1.5.1.5  89/12/20  19:05:27  colas
 * december release of arup's twm package
 * menu min/max width was ignored when resized
 * process-events nary
 * flag reenter_on_opening
 * 
 * Revision 1.5.1.3  89/12/20  15:25:08  colas
 * 2 patches from dab@berserkly.cray.com (David Borman) for DEC BSD4.4 (-DSTUPID)
 * x, y, gravity, borderwidth & borderpixel in "meter"
 * meter returns list of previous values that can be re-given to meter
 * Redecorating sometimes unmapped the window
 * meter nows shrinks and grows (was only growing before)
 * window-creep fixed
 * Dlists package cleaned up
 * 
 * Revision 1.5.1.2  89/12/07  17:41:09  colas
 * map-notify event
 * 
 * Revision 1.5.1.1  89/12/05  13:36:20  colas
 * new rcs system
 * was sending 2 icccm events on resize+move
 * on withdraw request, undecorate the window to start afresh on new map.
 * menu-max-width & menu-min-width
 * 
 * Revision 1.5  89/11/27 18:10:00 colas
 * X11R4 RELEASE	
 * - is now n-ary (- n1 (+ n2 n3 n4))
 */

#include "wool.h"
DECLARE_strchr;

GWM_print_banner()
{
    static char     rev_number[20];

    strncpy(rev_number, strchr(RCS_Version, ' ') + 1,
	    strchr(RCS_Version + 1, '$') - strchr(RCS_Version, ' ') - 2);
    printf("GWM %s %s\n", rev_number,
#ifdef DEBUG
	   " (debug)"			/* debug-mode banner */
#else					/* DEBUG */
	   " (gwm@mirsa.inria.fr)"
#endif					/* DEBUG */
	   );
}

/* WARNING: This is an internal rcs number for my eyes only. The official
 * one is found in RCS_Version!
 */

static char           *RCS_Header = 
"$Id: gwm.shar,v 1.57 1991/10/04 13:37:58 colas Exp $";

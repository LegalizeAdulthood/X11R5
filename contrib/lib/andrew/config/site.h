/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* CMU-local overrides to the distributed system.h file.  The distributed version
   of this file is empty, and patches are never sent for it. */

/* We have AFS version 3.0 at CMU. */
#define	AFS_ENV	1
#define	AFS30_ENV	1

#ifdef _IBMR2
#define AFS31_ENV 1
#endif /* _IBMR2 */


#ifndef AMS_DELIVERY_ENV
#define AMS_DELIVERY_ENV 1
#endif /* AMS_DELIVERY_ENV */

#ifndef WHITEPAGES_ENV
#define WHITEPAGES_ENV 1
#endif /* WHITEPAGES_ENV */

#ifndef SNAP_ENV
#define SNAP_ENV 1
#endif /* SNAP_ENV */

#ifndef _IBMR2 
#ifndef ANDREW_MALLOC_ENV
#define ANDREW_MALLOC_ENV 1
#endif /* ANDREW_MALLOC_ENV */
#endif  _IBMR2  

#ifndef _IBMR2  
#ifndef DEBUG_MALLOC_ENV
#define DEBUG_MALLOC_ENV 1
#endif /* DEBUG_MALLOC_ENV */
#endif  _IBMR2 

#ifndef MK_BASIC_UTILS
#define MK_BASIC_UTILS 1
#endif /* MK_BASIC_UTILS */

#ifndef MK_AUTHORING
#define MK_AUTHORING 1
#endif /* MK_AUTHORING */

#ifndef MK_AUX_UTILS
#define MK_AUX_UTILS 1
#endif /* MK_AUX_UTILS */

#ifndef MK_AUX_INSETS
#define MK_AUX_INSETS 1
#endif /* MK_AUX_INSETS */

#ifndef MK_EXAMPLES
#define MK_EXAMPLES 1
#endif /* MK_EXAMPLES */

/* Run AMDS here at CMU. */
#define	RUN_AMDS_ENV	1

/* Other CMU-specific instructions. */
#ifndef CMU_ENV
#define	CMU_ENV	1
#endif /* CMU_ENV */

#ifdef ANDREW_PRINTING_ENV
#undef ANDREW_PRINTING_ENV
#endif /* ANDREW_PRINTING_ENV */
#define ANDREW_PRINTING_ENV	1

/* Install real copies, not links. */
#ifdef LINKINSTALL_ENV
#undef LINKINSTALL_ENV
#endif /* LINKINSTALL_ENV */

/* Build contributions here at CMU. */
#define CONTRIB_ENV 1

#ifdef _IBMR2
#define PRE_X11R4_ENV 1
#endif /* _IBMR2 */

#ifdef DEFAULT_ANDREWDIR_ENV
#undef DEFAULT_ANDREWDIR_ENV
#define DEFAULT_ANDREWDIR_ENV /usr/itc/released
#endif /* DEFAULT_ANDREWDIR_ENV */

#ifdef DEFAULT_LOCALDIR_ENV
#undef DEFAULT_LOCALDIR_ENV
#define DEFAULT_LOCALDIR_ENV /usr/itc/projects
#endif /* DEFAULT_LOCALDIR_ENV */

#define BUILDANDREWINSTALL_ENV 1

#ifdef WM_ENV
#undef WM_ENV 
#endif /* WM_ENV */

#define	ISO80_FONTS_ENV	1
#ifdef _IBMR2
#define print_PRINTCOMMAND "psdit|lpr"
#define print_PSCPRINTCOMMAND "lpr"
#endif

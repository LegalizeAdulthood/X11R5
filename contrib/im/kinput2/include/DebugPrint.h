/* $Id: DebugPrint.h,v 1.4 1991/09/23 04:09:49 ishisone Rel $ */

#ifdef DEBUG
extern int	debug_all;

#ifdef DEBUG_VAR
int DEBUG_VAR = 0;
#else
#define DEBUG_VAR	0
#endif

#define DEBUG_CONDITION	(debug_all || DEBUG_VAR)
#define DPRINT(args)	{ if (DEBUG_CONDITION) printf args; }

#else /* !DEBUG */

#define DEBUG_CONDITION	0
#define DPRINT(args)

#endif

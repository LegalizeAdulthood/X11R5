/*
 *  UPDATE_INTERVAL define the number of microseconds between
 *  each position update when the game is running.
 */
 
#define UPDATE_INTERVAL		125000

/*
 *  REDRAW_EVERY defines the number of update intervals between each
 *  redraw of player screens.
 */

#define REDRAW_EVERY		3

/*
 *  We'll check every now and again to see if any aircraft are waiting to
 *  be resupplied.  RESUPPLY_EVERY defines the number of update intervals
 *  between each check (30 seconds, here).
 */

#define RESUPPLY_EVERY		(30 * 8)

/*
 *  ACM_PORT define the internet port number to be used in the game 
 *  startup handshaking.
 */

#define ACM_PORT		3232

/*
 *  ACM becomes the name used to lookup X resources on a particular
 *  X server.
 */

#define ACM			"acm"

/*
 *  Animation on multi-plane displays can be performed with pixmaps or by
 *  manipulating planes and colors.  If USE_PIXMAP_ANIMATION is defined, we'll
 *  use that method.
 *
 *  *** Warning:  this does not work, yet ***
 */

/* #define USE_PIXMAP_ANIMATION */

/*
 *  Linear response to control stick inputs can make the plane harder
 *  to fly.
 */

/* #define LINEAR_CONTROL_RESPONSE */

/*
 *  We have code in-place to perform flight debugging.  Define this to
 *  compile in that code.
 */

/* #define FLIGHTDEBUG */

/*
 *  When on the ground, we won't allow planes to stray too far from their
 *  team's airport.  MAX_GROUND_DISTANCE defines that distance.
 */

#define MAX_GROUND_DISTANCE	(3.0 * NM)

/*
 *  Alot of the world uses eight plane color displays.  We use six of those
 *  to perform eight color double-buffered animation.  Don't undefine this
 *  in any pre-3.0 acm revision.
 */

#define EIGHT_COLORS

/*
 *  There are functions in acm that would like to return an IEEE NaN value
 *  in certain situations.  If your system *really* supports IEEE NaN's,
 *  then define HAS_IEEE_NAN.
 */

/* #define	HAS_IEEE_NAN */

/*
 *  Define a library directory to contain the acm files
 */

#define ACM_LIBRARY		"/usr/games/lib/acm/"

/*
 *  a signal handler function type
 */

#ifdef SIGVOID
#define acm_sig_t	void	/* System V returns void */
#else
#define acm_sig_t	int	/* BSD does int's */
#endif

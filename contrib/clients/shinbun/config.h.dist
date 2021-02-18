#ifdef sccs
static  char sccsid[] = "@(#)config.h	1.2 91/03/18";
#endif

/* If version is up, po file should be modefied */
#define SHINBUN_VERSION_NO "Shinbun tool Ver.1.0"

/* for the use of Kanji character */
#define MLE

/* for the use of EUC Kanji character on JLE */
#define JLE

#ifdef JLE
/* Use JLE commands and functions to convert the code */
#define EUCNEWS
#endif

/* Name of your mail domain. */
#define MAILADDRESS "japan.sun.com"

/* Organization */
#define ORGANIZATION "Nihon Sun Microsystems K.K."

/* Get user information from the gcos field */
#define GCOS "ypmatch %s passwd | awk \'BEGIN { FS = \":\" } { print $5 }\'"

/* Indent include msg. First %s is for Message ID.  Second one is for
 * senders address.
 */
#define INDENT_INCLUDE_MSG "\n\nIn article%s,\n\t%s says:\n\n"

/* Start and End include messages */
#define BEGIN_INCLUDE "----- Begin Included Message -----"
#define END_INCLUDE "----- End Included Message -----"

/* Maximum string length */
#define MAXSTRLEN  512

/* Max line of nbiff panel list item */
#define MAX_OBSERVED_GROUPS 50

#ifndef TRUE
#define TRUE (1)
#endif
#ifndef FALSE
#define FALSE (0)
#endif

/* distribution Field */
#ifdef SUN_JAPAN_ENGNEERING
#define DEFAULT_DIST_NAME "engj"
#define DEFAULT_DIST_MENU_NAME "engj", "sunj", "fj", "world"
#else
#define DEFAULT_DIST_NAME NULL
#define DEFAULT_DIST_MENU_NAME "comp", "alt", "world"
#endif

/* Distribution field for beginners in our dept. */
#ifdef FOR_INTERNAL_USE
#define INTERNAL_NEWS_GROUP	"engj", "sunj", "sun"
#endif

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#ifdef POSIX
#include <unistd.h>
#endif

#if defined(USG) || defined(STDC_HEADERS)
#include <string.h>
#define index strchr
#define rindex strrchr
#define bcopy(from, to, len) memcpy ((to), (from), (len))
#define bzero(s, n) memset ((s), 0, (n))
#else /* USG or STDC_HEADERS */
#include <strings.h>
extern char* malloc();
extern char* realloc();
extern double atof();
#endif /* USG or STDC_HEADERS */

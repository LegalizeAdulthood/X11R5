#include <math.h>

/*
 *  Some data structures to perform fast 2-D interpolation
 *
 *  Alot of aircraft information is in tabular form.  We perform linear
 *  interpolation to determine values.  In order to speed up that process,
 *  we'll create preprocessed tables.  Each entry in the table contains the
 *  upper bound of the domain value (x) and the equation of the line that
 *  defines the data for that interval (y = m * x + b).
 *
 *  The program ibuild preprocesses the tabular information into a
 *  corresponding C structure that the interpolate() function can use.
 */

typedef float float_t;

typedef struct {
	float_t	x;		/* upper x value for this interval */
	float_t	m;		/* slope of line in this interval */
	float_t	b;		/* y-intercept of line in this interval */
	} IEntry;

typedef struct {
	int	count;		/* entry count in the interpolation table */
	float_t	minX;		/* minimum x value that can be interpolated */
	IEntry	*entry;		/* vector of intterpolation entries */
	} ITable;

/*
 *  I'm doing this to catch bad interpolation requests.  If an invalid
 *  domain value is submitted, (and we're on an IEEE FP system) we'll
 *  get back the IEEE NaN value as the interpolated result.  Non-IEEE systems
 *  will get varying results.
 */

#ifdef HAS_IEEE_NAN
#define I_NaN	(sqrt(-1.0))
#else
#define I_NaN	(0.0)
#endif

extern float_t interpolate();

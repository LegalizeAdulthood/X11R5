typedef struct {
	int	xorg;		/* x loc of bottom of scale */
	int	yorg;		/* y loc of bottom of scale */
	int	length;		/* length of scale (pixels) */
	int	orientation;	/* orientation flags */
	double	scale;		/* units per pixel */
	int	minorInterval;	/* units per minor tick */
	int	minorSize;	/* width of minor ticks (pixels) */
	int	majorInterval;	/* units per major tick */
	int	majorSize;	/* width of major ticks (pixels) */
	int	indexSize;	/* width of index (pixels) */
	double	divisor;	/* divisor on digit scale */
	char	*format;	/* output format */
	}	scaleControl;

#define orientRight		1
#define orientHorizontal	2

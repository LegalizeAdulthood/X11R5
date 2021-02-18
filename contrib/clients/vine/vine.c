/*
 *  vine.c
 *  Written by Dan Heller one fine winter week in the spirit of the
 *  coming holidays, 1985.  Works on Sun versions 1.4 and up.
 *
 *  compile: cc -O -s vine.c -lsunwindow -lpixrect -lm -o vine
 *
 *  Updating to work on X11 March 1990.
 *  compile: cc -O -s vine.c -DX11 -lX11 -lm -o vine
 *
 * The way vines works is a "stem" grows and leaves grow from that stem.
 * The stem and the leaf are arrays of "points" (struct pr_pos).  You can
 * add your own images (like flowers or something) with the -S and -L
 * flags.  The usages for these flags are:
 * 
 * -S file
 * -L file
 * 
 * These files are opened and pairs of points are read in place of the
 * Stems and Leaves respectively.  The end of an image is represented
 * by the coordinate pair 0,0.
 * 
 * For each coordinate pair read, pr_vectors are drawn from 0,0 to the
 * first point, then to the second and so on till the final 0,0 and then
 * a pr_polygon2 routine is called to fill in the image.
 *
 * Because there is an extra line in the middle of leaves, some of the code
 * around pr_polygon2() is added to continue drawing the leaves.  Unless your
 * own images are simple, you're probably going to have to do the same thing
 * in that same area (search for pr_polygon2 for where I'm talking about).
 *
 * If you do anything interesting to this code, I would GREATLY appreciate
 * knowing what you did.  Please send any hacks or comments/flames to:
 *
 *    argv@sun.com
 */

char *err_msg[] = {
    "usages for %s:\n",
    "-c:  vines grow from the center of the screen",
    "-e:  vines grow along the edges of the screen (default)",
    "-D:  degradee the color down the colormap",
    "-f:  fall season (brown leaves: color only)",
    "-h:  vines halt upon RETURN",
    "-r:  reverse video; leafs are inverted",
    "-C:  clears the screen and prepares for color display (-w not allowed)",
    "-b N: probability of branching (max 40)",
    "-i N: interleaf black/white leaves 1 for each N (-N white/black)",
    "-l N: as N increases, more leaves grow from each stem (max 35)",
    "-s N: max size of leaves (default: 2)",
    "-v N: grow N vines from the top of the screen. (default 4)",
    "-R N: have N different leaf rotations (max 360)",
#ifdef X11
    "-d display: use display:server[.screen]",
#else /* X11 */
    "-d device: use alternate frame buffer; default is /dev/fb",
    "-w:  vines grow around each window on the screen (not icons; no color)",
#endif /* X11 */
    "-L file: use leaf defined in file as x,y coordinate pairs",
    "-S file: use stem defined in file as x,y coordinate pairs",
    0
};

#define MAX_FORKS 6

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <ctype.h>
#include <signal.h>

#define when break;case

#ifdef X11
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
typedef XPoint point_t;
struct rect {
    int	r_left, r_top, r_width, r_height;
};
#else /* X11 */
typedef struct pr_pos point_t;
#include <suntool/tool_hs.h>
#include <pixrect/pixrect_hs.h>
#include <suntool/wmgr.h>
#include <sunwindow/cms_mono.h>
#define DefaultDepthOfScreen(s) s->pr_depth
#define WidthOfScreen(s) s->pr_size.x
#define HeightOfScreen(s) s->pr_size.y
#define WUF_WMGR1 0x10
#define XFlush(dpy) /* not used in Suntools */
#endif /* X11 */

#include <sys/wait.h>
#include <sys/resource.h>
#include <math.h>

#define UP 			001  /* turn object up-side down */
#define MIRROR 			002  /* mirror the object */
#define FLIP			004  /* rotate object 90 deg. */
#define FLOP			010  /* rotate object -90 deg. */
#define DEG_TO_RADS 		57.295779513
#ifdef X11
#define SetBlack() intensity = (is_color || !reverse)? \
    BlackPixelOfScreen(screen) : WhitePixelOfScreen(screen); \
    XSetForeground(dpy, gc, intensity)
#define SetColor(color) intensity = is_color? color : \
    reverse? WhitePixelOfScreen(screen) : BlackPixelOfScreen(screen); \
    XSetForeground(dpy, gc, intensity)
#define draw(x1,y1,x2,y2) XDrawLine(dpy,win,gc,(int)x1,(int)y1,(int)x2,(int)y2)
#else /* X11 */
#define SetBlack() (intensity = (is_color || !reverse)? \
    BLACK : is_color? 255 : WHITE)
#define SetColor(color)         if (is_color) intensity = color; else SetBlack()
#define RAS_OP			PIX_COLOR(intensity)|PIX_SRC	
#define draw(x1,y1,x2,y2) 	pr_vector(screen,x1,y1,x2,y2,RAS_OP,1)
#endif /* X11 */

/*
 * each {x,y} is a relative point to 0,0. End is {0,0} pair
 */
point_t leaf[] = {
    {-4,4}, {-9,6}, {-13,9}, {-15,11}, {-16,14}, {-17,16}, {-17,20},
    {-16,24}, {-13,28}, {-11,30}, {-9,32}, {-4,36}, {-1,38}, {4,39}, {6,37},
    {4,37}, {3,34}, {3,31}, {4,29}, {6,25}, {9,20}, {10,16}, {10,12},
    {8,8}, {7,7}, {4,6}, {2,4}, {1,0}, {0,2}, {-1,4}, {-3,8},
    {-5,12}, {-6,16}, {-6,20}, {-5,24}, {-3,27}, {0,0}
};
int leafsize = sizeof (leaf) / sizeof (point_t);

point_t stem[50] = {
    {0,7}, {1,8}, {2,7}, {1,12}, {0,11}, {-1,12}, {-3,15}, {-3,19}, {-2,22},
    {-1,24}, {1,26}, {3,28}, {4,30}, {5,32}, {5,36}, {7,40}, {8,42}, {9,43},
    {8,49}, {5,51}, {2,54}, {0,60}, {0,0}
};
int stemsize = sizeof (stem) / sizeof (point_t);
point_t leaves[360][sizeof(leaf) / sizeof(point_t)];
struct rect win_rects[32];

int reverse;          /*  -r:  reverse video */
int rot_increment=45; /*  -R:  rotation increment... upto 360 degrees */
int interleaf;        /*  -i:  interleaf black and white leaves */
int lsize = 2;        /*  -s:  max magnification variable for leaves */
int leaf_freq = 30;   /*  -l:  frequency of leafs per stem */
int do_halt;	      /*  -h:  program halts on RETURN */
int branch_freq = 5;  /*  -b:  how often to branch off a new vine */
int n_vines = 4;      /*  -v:  4 is default */
int from_center;      /*  -c:  start at the center of screen */
int is_color;	      /*  -C:  user is running from a color monitor */
int dbug;	      /*  -g:  debug mode, inspired by cc's -g flag for dbx */
int do_edges;	      /*  -e:  just traverse the edges of the screen */
int do_windows;       /*  -w:  grow around each open window in suntools */
int season;	      /*  -f:  fall season */
int fade;	      /*  -D:  fade the color down the colormap */

unsigned long colors[255];
int max_color;        /* maximum number of colors available */
int win_no, my_win;   /* number of windows running / which win we are */
int total_forks;      /* the total number of forks we've done so far */

char *prog_name;

#ifdef X11
unsigned long intensity;        /* shade of color to paint */
Display *dpy; /* display */
Screen *screen;
Window win; /* root window */
GC gc; /* points to default GC of screen */
char *device; /* default screen == default to NULL (see XOpenDisplay below) */
#else /* X11 */
int intensity;        /* shade of color to paint */
struct pixrect *screen;   /*  our screen's frame buffer */
char *device = "/dev/fb"; /* default frame buffer */
#endif /* X11 */
char *leaf_file;      /*  files for alternate leaf and stem images */
char *stem_file;
FILE *f;

#ifndef X11
#include <pixrect/pr_planegroups.h>
#endif /* X11 */

main(argc, argv)
char **argv;
{
    register int count, x, y = 1;
    int c;
    int dead_kids();

    prog_name = argv[0];

    if (argc > 1)
	parse_args(argc, argv);
    else
	do_edges = 1, interleaf = 3;

    if (leaf_file) {
        if (!(f = fopen(leaf_file,"r")))
	    perror(leaf_file), exit(1);
	for (c = 0; c < leafsize; ++c)
	    if (fscanf(f,"%d,%d", &leaf[c].x, &leaf[c].y) < 2)
	        break;
	(void) fclose(f);
	if (leaf[c - 1].y || leaf[c - 1].x) {
	    if (c == leafsize)
	        fprintf(stderr, "%s: too many points (limit is %d)\n",
			leaf_file, c);
	    else
	        fprintf(stderr, "%s: last coordinate pair must be 0,0\n",
		      leaf_file);
	    exit(1);
	}
	leafsize = c - 1;
    }

    if (stem_file) {
        if (!(f = fopen(stem_file,"r")))
	    perror(stem_file), exit(1);
	for (c = 0; c < stemsize; ++c)
	    if (fscanf(f,"%d,%d", &stem[c].x, &stem[c].y) < 2)
	        break;
	(void) fclose(f);
	if (stem[c - 1].y || stem[c - 1].x) {
	    if (c == stemsize)
	        fprintf(stderr, "%s: too many points (limit is %d)\n",
			stem_file, c);
	    else
	        fprintf(stderr, "%s: last coordinate pair must be 0,0\n",
			stem_file);
	    exit(1);
	}
    }

    if (do_halt || from_center)
	/* don't count this fork ... */
	if (fork())
	    getchar(), kill(0, 9);
    if (interleaf < 0)
	reverse = !reverse, interleaf *= -1;
    setbuf(stdout, NULL);
    printf("growing leaves ");
    for (x = 0, y = 0; x < 360; x += rot_increment) {
	rotate_obj(leaf, leaves[y++], leafsize, x/DEG_TO_RADS);
	if (!(y % 10))
	    putchar('.');
    }
    puts(" done.");
    init_display();
#ifndef X11
    if (!is_color && do_windows) {
	if (!get_windows())
	    puts("No windows!");

	for (my_win = 0; my_win < win_no-1 && fork(); my_win++)
	    ;

	if (fork())
	    edge_vines(win_rects[my_win].r_left + win_rects[my_win].r_width / 2,
			    win_rects[my_win].r_top, FLIP);
	else
	    edge_vines(win_rects[my_win].r_left + win_rects[my_win].r_width / 2,
			    win_rects[my_win].r_top, FLOP);
	exit(0);
    }
#endif /* X11 */
    y = 1;
    if (do_edges) {
	if (!fork()) {
	    init_display();
	    edge_vines(WidthOfScreen(screen) / 2, 10, FLIP);
	} else
	    edge_vines(WidthOfScreen(screen) / 2, 10, FLOP);
	exit(0);
    }
#ifdef SIGCHLD
    (void) signal(SIGCHLD, dead_kids);
#endif /* SIGCHLD */
    for (count = 0, x = WidthOfScreen(screen) / (n_vines+1); count < n_vines;
			     x += WidthOfScreen(screen) / (n_vines+1), count++)
	if (from_center)
	    random_vines(WidthOfScreen(screen)/2, HeightOfScreen(screen)/2);
	else
	    random_vines(x, y);
}

#ifdef SIGCHLD
/* catch exited forked processes and decrement number of total_forks avail */
dead_kids()
{
#ifdef SVR4
    siginfo_t si;
#else
    union wait status;
#endif

#ifdef SVR4
    while (waitid(P_ALL, 0, &si, WNOHANG|WEXITED))
#else
    while (wait3(&status, WNOHANG, (struct rusage *) 0))
#endif
        total_forks--;
}
#endif /* SIGCHLD */

extern long time();
extern long random();

edge_vines(x, y, which_way)
{
    register int stop_l, stop_r, y_stop;
    if (do_windows) {
	stop_l = win_rects[my_win].r_left;
	stop_r = stop_l + win_rects[my_win].r_width;
	y_stop = y + win_rects[my_win].r_height;
    } else {
	stop_r = WidthOfScreen(screen) - 60;
	stop_l = 60;
	y_stop = HeightOfScreen(screen) - 60;
    }
    /* srandom must be here for recursive calls from new forked procs */
    srandom((int) (getpid() * time((long *)0)));

    leaf_freq = 34;  /* edge vines have constant leaf frequency */

    while(x > stop_l && x < stop_r)
	draw_stem(&x, &y, which_way | ((random() % 2) * UP));
    while(y < y_stop)
	draw_stem(&x, &y, (random() % 2) * MIRROR);
    /* draw along the bottom
    while(x < stop_r)
	draw_stem(&x, &y,
	    (random() % 2) * UP | ((which_way == FLIP) ? FLOP : FLIP));
    */
    XFlush(dpy);
}

random_vines(xstart, ystart)
{
    /* parent returns */
    if(total_forks < MAX_FORKS && fork()) {
	total_forks++;
	return;
    }
    init_display();
    /* make the child think there are too many forks */
    total_forks = MAX_FORKS;
    srandom((int) (getpid() * time((long *) 0)));
    while(ystart && ystart < HeightOfScreen(screen) &&
	  xstart && xstart < WidthOfScreen(screen)) {
	do_stem(&xstart, &ystart);
	if (!(random() % (1 + (40 - branch_freq))))
	    random_vines(xstart, ystart);
    }
    XFlush(dpy);
    exit(0);
}

do_stem(x,y)
int *x,*y;
{
    long dir = 0;
    if (!(random() % (from_center ? 2 : 20)))
	dir |= UP;
    if (random() % 2)
	dir |= MIRROR;
    if (random() % 2)
	dir |= (random() % 2) ? FLIP : FLOP;
    draw_stem(x, y, dir);
}

draw_stem(x,y,dir)
register int *x, *y;
long dir;
{
    /* ox,oy are old x and y; nx,ny are new x and new y coords */

    register int tmp, ox = *x, oy = *y, nx, ny;
    for(tmp = 0; stem[tmp].y || stem[tmp].x; tmp++) {
	if (dir & FLIP) {
	    ny = *y + (stem[tmp].x * ((dir & UP) ? -1 : 1));
	    nx = *x + (stem[tmp].y * ((dir & MIRROR) ? -1 : 1));
	} else if (dir & FLOP) {
	    ny = *y + (stem[tmp].x * ((dir & UP) ? -1 : 1));
	    nx = *x - (stem[tmp].y * ((dir & MIRROR) ? -1 : 1));
	} else {
	    ny = *y + (stem[tmp].y * ((dir & UP) ? -1 : 1));
	    nx = *x + (stem[tmp].x * ((dir & MIRROR) ? -1 : 1));
	}
	if (!do_windows || check_rects(nx, ny)) {
	    SetBlack();
	    draw(ox, oy, nx, ny);
	    if(!(random() % (1 + (35 - leaf_freq))))
		do_leaf(nx, ny);
	}
	ox = nx, oy = ny;
    }
    *x = ox, *y = oy;
}

do_leaf(x,y)
{
    int reverse_it = 0;
    if (interleaf && (reverse_it = !(random() % interleaf)))
	reverse = !reverse;
    /* start at x,y and rotate */
    draw_leaf(x, y, random() % (360/rot_increment),
		(double)(.6 + (random() % (10 * lsize)) / 20.0),  /* magnify */
		(long) random() % 2 ? -1 : 1); /* mirror image */
    if (interleaf && reverse_it)
	reverse = !reverse;
}

draw_leaf(x,y,rotation,magnification,mirror)
long rotation;
double magnification;
long mirror;
{
    static col;
    int tmp, ox = x, oy = y, nx, ny;
#ifndef X11
    int npts[1];
#endif
    point_t leaf2[sizeof(leaf) / sizeof(point_t)];

    /* magnify, move leaf into position, and mirror, if necessary */
    for(tmp = 0; tmp < leafsize; tmp++) {
	ny = y + magnification * leaves[rotation][tmp].y;
	nx = x + magnification * leaves[rotation][tmp].x * mirror;
	leaf2[tmp].y = oy, leaf2[tmp].x = ox;
	ox = nx, oy = ny;
    }
    /* paint leaf before drawing outline */
    reverse = !reverse;
    if (is_color)
	if (fade) {
	    col += fade;
	    if (!col || col == max_color-1)
		fade = -fade;
	} else
	    col = random() % max_color;
	    /* col = 80 + random() % 165; */
    SetColor(colors[col]); /* for black and white: intensity has no meaning */
    reverse = !reverse;
#ifdef X11
    XFillPolygon(dpy, win, gc, leaf2, 28, Nonconvex, CoordModeOrigin);
#else /* X11 */
    npts[0] = 28;   /* kludge for middle line of leaf! */
    pr_polygon_2(screen, 0, 0, 1, npts, leaf2, RAS_OP, (Pixrect *) 0,0,0);
#endif /* X11 */

    SetBlack();
    /* now draw the outline of the leaf */
    for (tmp -= 2; tmp; tmp--)
	draw(leaf2[tmp].x, leaf2[tmp].y, leaf2[tmp-1].x, leaf2[tmp-1].y);
}

/* check to see if we're insdie the rect of another window.  The win_rects
 * array has the oldest children (windows sitting "under" other windows)
 * first in the array.  So, we only check windows higher than us in the array
 * since we know we overlay the other windows.
 */
check_rects(x,y)
register int x,y;
{
    register int win;
    static lastwin = -1;
    for (win = my_win+1; win < win_no; win++)
	if (x > win_rects[win].r_left &&
	    x < win_rects[win].r_left + win_rects[win].r_width &&
	    y > win_rects[win].r_top &&
	    y < win_rects[win].r_top + win_rects[win].r_height) {
		if (dbug && win != lastwin)
		    printf("win %d ran into win %d at %d,%d\n",
			my_win, (lastwin = win), x,y);
		return 0;
	    }
    return 1;
}

/* foreach pt in object, rotate_pt() */
rotate_obj(object, newobject, npts, rotation)
point_t object[], newobject[];
double rotation;
{
    int i;
    for(i = 0; i < npts; i++)
	rotate_pt(&object[i], &newobject[i], rotation);
}

/* matrix multiplication */
rotate_pt(oldpt,newpt,rot)
point_t *oldpt, *newpt;
double rot;
{
    /* temp storage -- might want to copy to itself */
    register double tx = (double)oldpt->x, ty = (double)oldpt->y, X,Y;
    X = tx * cos(rot) - ty * sin(rot);
    Y = tx * sin(rot) + ty * cos(rot);
    newpt->x = (int)X + ((X * 10.0 - (int)X * 10) >= 5); /* round up */
    newpt->y = (int)Y + ((Y * 10.0 - (int)Y * 10) >= 5); /* round up */
    if (dbug == 2)
	printf("%g,%g --> %g,%g --> %d,%d\n", tx, ty, X, Y, newpt->x, newpt->y);
}

parse_args(argc, argv)
char **argv;
{
    register int c;
    extern int optind;
    extern char *optarg;

    int errors = 0;

    while ((c = getopt(argc, argv, "DefFhrgwcCb:d:i:l:s:v:R:L:S:?")) != EOF)
	switch (c) {
	    when 'c':
		from_center = 1, branch_freq = 20;
	    when 'e':
		do_edges = 1;
	    when 'D':
		fade = 1;
	    when 'f':
		season = 1;
	    when 'h':
		do_halt = 1;
	    when 'r':
		reverse = 1;
	    when 'g':
		dbug = 1;
#ifndef X11
	    when 'w':
		if (is_color)
		    fputs("ignoring -w after -C\n", stderr);
		else
		    do_windows = 1;
#endif /* X11 */
	    when 'C':
		if (do_windows)
		    fputs("ignoring -C after -w\n", stderr);
		else
		    is_color = 1;
	    when 'b':
		branch_freq = atoi(optarg);
		if (branch_freq > 40)
		    fprintf(stderr, "maximum branch frequency is 40\n"),
		    errors = 1;
	    when 'd':
		device = optarg;
	    when 'i':
		interleaf = atoi(optarg);
	    when 'l':
		if (do_edges)
		    fputs("warning: edge vines have constant leaf frequency\n", stderr);
		else {
		    leaf_freq = atoi(optarg);
		    if (leaf_freq > 40)
			fprintf(stderr, "maximum leaf frequency is 40\n"), errors = 1;
		}
	    when 's':
		lsize = atoi(optarg);	
		if (lsize <= 0)
		    fprintf(stderr, "bad leaf size: %d\n", lsize), errors = 1;
	    when 'v':
		n_vines = atoi(optarg);
	    when 'R':
		rot_increment = atoi(optarg);
		if (rot_increment <= 0 || rot_increment > 360)
		    fprintf(stderr, "bad leaf rotation: %d\n", rot_increment),
		    errors = 1;
	    when 'L':
		leaf_file = optarg;
	    when 'S':
		stem_file = optarg;
	    when '?':
		errors = 1;
	}

    if (errors) {
	fprintf(stderr, err_msg[0], prog_name);
        for (c = 1; err_msg[c]; c++)
	    fprintf(stderr, "%s\n", err_msg[c]);
	exit(1);
    }
}

#ifdef X11
get_windows(){}

/*
 * Open the display (connection to the X server).  If it's already
 * open, close it and reopen it since this is a child process and
 * each child needs its own connection.
 */
init_display()
{
    int x_error(), first_time = !dpy;
    char *getenv(), *p = getenv("DISPLAY");

    if (dpy)
	XCloseDisplay(dpy);
    if (!(dpy = XOpenDisplay(device? device : p? p : ""))) {
	fprintf(stderr, "%s: unable to open display '%s'\n",
	    prog_name, XDisplayName(p));
	exit(1);
    }
    XSetErrorHandler(x_error);
    screen = ScreenOfDisplay(dpy, DefaultScreen(dpy));
    win = RootWindowOfScreen(screen);
    gc = XCreateGC(dpy, win, 0L, NULL);
    if (first_time)
	initialize_color();
    XFlush(dpy);
}

XColor color;
initialize_color()
{
    Colormap cm = DefaultColormapOfScreen(screen);

    if (!is_color)
	return;
    color.flags = DoRed | DoGreen | DoBlue;
    for (max_color = 0; max_color < 256; max_color++) {
	if (season) {
	    color.green = (255 - max_color)<<8;
	    color.red = (255 - max_color/3)<<8;
	} else {
	    color.red = (255 - max_color)<<8;
	    color.green = (255 - max_color/3)<<8;
	}
	color.blue = (max_color/5)<<8;
	if (dbug)
	    printf("%d=", max_color);
	if (!XAllocColor(dpy, cm, &color)) {
	    printf("ran out of colors at index %d\n", max_color--);
	    break;
	}
	colors[max_color] = color.pixel;
	if (dbug)
	    printf("%d, ", color.pixel);
    }
}

int
x_error(display, error)
Display	*display;
XErrorEvent	*error;
{
    char	buffer[BUFSIZ];
    char	num[10];
    static int	count;		/* cumulative error total */

    /* get and print major error problem */
    XGetErrorText(display, error->error_code, buffer, BUFSIZ);

    fprintf(stderr, "x_error(%d): <%s>", ++count, buffer);
    /* print the request name that caused the error -- from X Database */
    /* internally this expects a numeric string (I peeked!) */
    sprintf(num, "%d", error->request_code );
    XGetErrorDatabaseText(display, "XRequest", num, num, buffer, BUFSIZ);
    fprintf(stderr, " %s\n", buffer);

    printf("color.red = %d, green = %d, blue = %d, pixel = %d\n",
	color.red>>8, color.green>>8, color.blue>>8, color.pixel);
    /* abort to core dump or global debug flag set.. */
    /* abort(0); */
    exit(0);
}

#else /* X11 */

init_display()
{
    char groups[PIXPG_OVERLAY+1];
    if (screen)
	return;
    if (!(screen = pr_open(device)))
	exit(1); /* pr_open prints its own error messages (bad idea) */

    pr_available_plane_groups(screen, sizeof(groups), groups);

    if (groups[PIXPG_OVERLAY] && groups[PIXPG_OVERLAY_ENABLE] && !is_color) {
	pr_set_plane_group(screen, PIXPG_OVERLAY_ENABLE);
	pr_rop(screen, 0,0, WidthOfScreen(screen), HeightOfScreen(screen),
	    PIX_SET, (Pixrect *) 0,0,0);
	pr_set_plane_group(screen, PIXPG_OVERLAY);
    }
}

get_windows()
{
    char name[WIN_NAMESIZE];
    int toolfd, wfd, link;
    struct screen screen;

    /*
     * Determine parent and get root fd
     */
    if (we_getparentwindow(name)) {
	perror(name);
	puts("You need to be running suntools to do the -w option");
	exit(1);
    }
    if ((wfd = open(name, O_RDONLY, 0)) < 0)
	perror(name);
    win_screenget(wfd, &screen);
    close(wfd);
    if ((wfd = open(screen.scr_rootname, O_RDONLY, 0)) < 0)
	perror(screen.scr_rootname);

    link = win_getlink(wfd, WL_OLDESTCHILD);
    while (link != WIN_NULLLINK) {
	win_numbertoname(link, name);
	if ((toolfd = open(name, O_RDONLY, 0)) < 0)
	    perror(name), exit(1);
	if (!(win_getuserflags(toolfd) & WMGR_ICONIC))
	    win_getrect(toolfd, &win_rects[win_no++]);
	link = win_getlink(toolfd, WL_YOUNGERSIB); /* Next */
	close(toolfd);
    }
    close(wfd);
    if (dbug) {
	int i;
	puts("windows at:");
	for (i = 0; i < win_no; i++) {
	    printf("%d,%d size: %d,%d\n",
		win_rects[i].r_left, win_rects[i].r_top,
		win_rects[i].r_width, win_rects[i].r_height);
	}
    }
    return win_no;
}

initialize_color()
{
    u_char red[256], green[256], blue[256];

    if (!is_color) {
        cms_monochromeload(red, green, blue);
        pr_putcolormap(screen, 0, 1, red, green, blue);
        max_color = 1;
    } else {
	for (max_color = 0; max_color < 256; max_color++) {
	    red[max_color] = 255 - max_color;
	    green[max_color] = 255 - max_color/3;
	    blue[max_color] = max_color/5;
	    colors[max_color] = max_color;
	}
	if (!season)
	    pr_putcolormap(screen, 0, 255, red, green, blue);
	else /* swapping red and green maps makes shades of brown */
	    pr_putcolormap(screen, 0, 255, green, red, blue);
    }
}
#endif /* X11 */

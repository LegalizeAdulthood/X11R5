/*************************************************************************
 * ttp.c - Totally Trivial PEXIM test case.
 */
#include <X11/Xlib.h>
#include <phigs/pexim.h>

main(int argc, char *argv[])
{
  int i;
  Display *display;
  Window w;
  char *displayString = (char *)0;
  int x = 100, y = 100, width = 500, height = 400;
  int err;

  long wkid = 1;
  Pgcolr gcolr;
  Pint error;
  Ppoint3 points[10], *p;
  Ppoint_list3 plist;
/*************************************************************************
 *
 */

  for ( i = 1; i<argc; i++ ) {
    if ((strncmp(argv[i],"-display",strlen(argv[i]))) == 0) {
      if (++i > argc) { printf("not enough args"); exit(1); }
      displayString = argv[i];
    }
  }

  display = XOpenDisplay(displayString);
  if (!display) { printf(" cannot open display %s\n",displayString); exit(1);}

  w = XCreateSimpleWindow( display, DefaultRootWindow(display), x, y,
                           width, height, 5, 
                           BlackPixel(display,DefaultScreen(display)),
                           WhitePixel(display,DefaultScreen(display)));

  XMapWindow(display,w);
  XFlush(display);

  error = pinit();

  error = popen_renderer(wkid, display, w);
  if (error != POK) {printf("popen_renderer, error = %d\n",error); exit(1);}
  XSync(display, 1);

  error = pbegin_rendering(wkid);
  XSync(display, 1);

  gcolr.type = PMODEL_RGB;
  gcolr.val.general.x = 1; gcolr.val.general.y = 1; gcolr.val.general.z = 0;
  pset_line_colr(&gcolr);

  plist.points = points;
  p = points;
  p->x = 0.0; p->y = 0.0; p->z = 0.0; p++;
  p->x = 0.0; p->y = 1.0; p->z = 0.0; p++;
  p->x = 0.9; p->y = 0.1; p->z = 0.0; p++;
  p->x = -0.5; p->y = -0.5; p->z = 0.0; p++;
  p->x = 0.1; p->y = 0.9; p->z = 0.0; p++;
  p->x = 0.8; p->y = 0.6; p->z = 0.0; p++;
  p->x = 0.0; p->y = 0.0; p->z = 1.0;
  plist.num_points = 7;
  ppolyline3( &plist );
  XSync(display, 1);

  error = pend_rendering(wkid, 1);
  XSync(display, 1);
  getchar(); /* just leave it up there for a second */
}

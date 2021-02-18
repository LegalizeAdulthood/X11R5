#include <X11/Xlib.h>


main(argc,argv)
     int argc;
     char *argv[];
{

   Display *d;
   Window w;
   int width;
   int height;
   XSetWindowAttributes attr;

   ++argv;
   width = atoi(*argv);
   ++argv;
   height = atoi(*argv);
   attr.background_pixmap = None;

   d = XOpenDisplay("");
   w = XCreateWindow(d,RootWindow(d,0),0,0,width,height,1,DefaultDepth(d,0),
		     CopyFromParent,CopyFromParent,CWBackPixmap,&attr);
   XMapWindow(d,w);

   while (1)
      XFlush(d);
}

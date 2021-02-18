/*
  Define declarations.
*/
#ifdef SYSV
#include <poll.h>
poll((struct poll *) 0, (size_t) 0, usec / 1000);
#define Delay(milliseconds)  \
  poll((struct poll *) 0,(size_t) 0,milliseconds/1000);
#else
#ifdef vms
#define Delay(milliseconds)
#else
#define Delay(milliseconds)  \
{  \
  struct timeval  \
    timeout;  \
  \
  timeout.tv_usec=(milliseconds % 1000)*1000;  \
  timeout.tv_sec=milliseconds/1000;  \
  select(0,(void *) 0,(void *) 0,(void *) 0,&timeout);  \
}
#endif
#endif

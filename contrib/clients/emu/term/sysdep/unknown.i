/* Generate a compile-time error indication somehow */

#ifdef __GNUC__
#error "I don't know how to deal with this kind of system."
#else
/* GACK! */
#include "I don't know how to deal with this kind of system."
#endif

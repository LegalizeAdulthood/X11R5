/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/****************************************************\
* 						     *
*  package to help maintaining one .h file per type  *
*  INTERN means "export" (and allocate memory) for   *
*  all following includes			     *
* 						     *
\****************************************************/

#ifdef EXT
#undef EXT
#endif
#define EXT

#ifdef INIT
#undef INIT
#endif
#define INIT(x) = x

#define DO_INIT

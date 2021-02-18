/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/****************************************************\
* 						     *
*  package to help maintaining one .h file per type  *
*  EXTERN means "import" for all following includes  *
* 						     *
\****************************************************/

#ifdef EXT
#undef EXT
#endif
#define EXT extern

#ifdef INIT
#undef INIT
#endif
#define INIT(x)

#ifdef DO_INIT
#undef DO_INIT
#endif

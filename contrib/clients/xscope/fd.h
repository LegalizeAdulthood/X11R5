/* **********************************************
 *						*
 * header file file descriptor (FD) code        *
 *						*
 *	James Peterson, 1987			*
 *	(c) Copyright MCC, 1987 		*
 *						*
 ********************************************** */


/* 
   the following structure remembers for each file descriptor its
   state.  In particular, we need to know if it is busy or free
   and if it is in use, by whom.
*/

typedef int FD;

struct FDDescriptor
{
    Boolean Busy;
    int     (*InputHandler)();
};

struct FDDescriptor *FDD /* array of FD descriptors */ ;
short   MaxFD /* maximum number of FD's possible */ ;

short   nFDsInUse /* number of FD's actually in use */ ;

long    ReadDescriptors /* bit map of FD's in use -- for select  */ ;
short   HighestFD /* highest FD in use -- for select */ ;

/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/examples/ex10/RCS/hello.ch,v 2.7 1991/09/12 19:34:12 bobg Exp $ */
/* Mouse drag, key bindings, menus, scrollbar, data object */

#define POSUNDEF -1

class helloworld[hello]: dataobject[dataobj] {
data:
    long x,y;
    boolean blackOnWhite;
};

/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*****************************************\

  Chart Programming Guide --  Example #1

\*****************************************/

class chartx1app [chartx1a]: application [app]
  {
  overrides:
   Start()				    returns boolean;

  data:
    struct chart			   *chart_data_object;
    struct chartv			   *chart_view_object;
    struct im				   *im;
    struct frame			   *frame;
  };

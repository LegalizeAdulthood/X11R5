/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 * BE1 to BE2 conversion package
 */

package be1be2 {

classprocedures:

    /*
     * CheckBE1 uses heuristics to decide if a document
     * is BE1.  It is fairly accurate although it's impossible
     * to do this task with the preferred efficiently.
     */

    CheckBE1(struct text *text) returns boolean;

    /*
     * Convert usually adds many entries to text's stylesht.
     * Returns a boolean indication of success.  However, it
     * is too picky and usually does better than it reports.
     */

    Convert(struct text *text) returns boolean;

};

/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "mode.h"
#include "resources.h"

print_to_printer(printer, center, mag)
    char            printer[];
    int		    center;
    float           mag;
{
    char            prcmd[200], translator[60], syspr[60];
    char            tmpfile[32];

    sprintf(tmpfile, "%s%06d", "/tmp/xfig-print", getpid());
    if (write_file(tmpfile))
	return;

    sprintf(translator, "fig2dev -Lps %s -P -m %f %s",
	    center ? "-c" : "",
	    mag,
	    print_landscape ? "-l xxx" : " ");

    if (emptyname(printer)) {	/* send to default printer */
#ifdef SYSV
	sprintf(syspr, "lp -oPS");
#else
	sprintf(syspr, "lpr -J %s", cur_filename);
#endif
	put_msg("Printing figure on default printer in %s mode ...",
		print_landscape ? "LANDSCAPE" : "PORTRAIT");
    } else {
#ifdef SYSV
	sprintf(syspr, "lp -d%s -oPS", printer);
#else
	sprintf(syspr, "lpr -J %s -P%s", cur_filename, printer);
#endif
	put_msg("Printing figure on printer %s in %s mode ...",
		printer, print_landscape ? "LANDSCAPE" : "PORTRAIT");
    }

    /* make up the whole translate/print command */
    sprintf(prcmd, "%s %s | %s", translator, tmpfile, syspr);
    if (system(prcmd) == 127)
	put_msg("Error in printing");
    unlink(tmpfile);
}

print_to_file(file, lang, mag)
    char           *file, *lang;
    float           mag;
{
    char            prcmd[200];
    char            tmpfile[32];

    if (!ok_to_write(file, "EXPORT"))
	return (1);

    sprintf(tmpfile, "%s%06d", "/tmp/xfig-export", getpid());
    if (write_file(tmpfile))
	return (1);

    put_msg("Exporting figure to file \"%s\" in %s mode ...",
	    file, print_landscape ? "LANDSCAPE" : "PORTRAIT");

    sprintf(prcmd, "fig2dev -L%s -m %f %s %s %s", lang, mag,
	    print_landscape ? "-l xxx" : " ", tmpfile, file);
    if (system(prcmd) == 127)
	put_msg("Error during EXPORT");
    unlink(tmpfile);
    return (0);
}

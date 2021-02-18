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
#include "object.h"
#include "mode.h"
#include "resources.h"

int
emptyname(name)
    char            name[];

{
    if (*name == '\0') {
	return (1);
    } else {
	return (0);
    }
}

int
emptyname_msg(name, msg)
    char            name[], msg[];

{
    int             returnval;

    if (returnval = emptyname(name))
	put_msg("No file name specified, %s command ignored", msg);
    return (returnval);
}

int
emptyfigure()
{
    if (objects.texts != NULL)
	return (0);
    if (objects.lines != NULL)
	return (0);
    if (objects.ellipses != NULL)
	return (0);
    if (objects.splines != NULL)
	return (0);
    if (objects.arcs != NULL)
	return (0);
    if (objects.compounds != NULL)
	return (0);
    return (1);
}

int
emptyfigure_msg(msg)
    char            msg[];

{
    int             returnval;

    if (returnval = emptyfigure())
	put_msg("Empty figure, %s command ignored", msg);
    return (returnval);
}

int
change_directory(path)
    char           *path;
{
    if (path == NULL) {
	*cur_dir == '\0';
	return (0);
    }
    if (chdir(path) == -1) {
	put_msg("Can't go to directory %s, : %s", path, sys_errlist[errno]);
	return (1);
    }
    if (get_directory(cur_dir) != NULL)	/* get cwd */
	return (0);
    else
	return (1);
}

get_directory(direct)
    char           *direct;
{
#ifdef SYSV
    extern char    *getcwd();

#else
    extern char    *getwd();

#endif

#ifdef SYSV
    if (getcwd(direct, 1024) == NULL) {	/* get curent working dir */
	put_msg("%s", "Can't get current directory");
#else
    if (getwd(direct) == NULL) {/* get curent working dir */
	put_msg("%s", direct);	/* err msg is in directory */
#endif
	*direct = '\0';
	return NULL;
    }
    return 1;
}

#ifndef S_IWUSR
#define	S_IWUSR	0000200
#endif
#ifndef S_IWGRP
#define	S_IWGRP	0000020
#endif
#ifndef S_IWOTH
#define	S_IWOTH	0000002
#endif

int
ok_to_write(file_name, op_name)
    char	   *file_name, *op_name;
{
    struct stat     file_status;
    char            string[180];

    if (stat(file_name, &file_status) == 0) {   /* file exists */
        if (file_status.st_mode & S_IFDIR) {
            put_msg("\"%s\" is a directory", file_name);
            return (0);
        }
        if (file_status.st_mode & (S_IWUSR | S_IWGRP | S_IWOTH)) {
            /* writing is permitted by SOMEONE */
            if (!((file_status.st_mode & S_IWUSR) &&    /* user writable */
                  (file_status.st_uid == geteuid()))
                &&
                !((file_status.st_mode & S_IWGRP) &&    /* group writable */
                  (file_status.st_gid == getegid()))
                &&
                !(file_status.st_mode & S_IWOTH)) {     /* world writable */
                put_msg("Write permission for \"%s\" is denied", file_name);
                return (0);
            } else {
                sprintf(string, "\"%s\" already exists.\nDo you want to overwrite it?", file_name);
                if (!win_confirm(canvas_win, string)) {
                    put_msg("%s cancelled", op_name);
                    return (0);
                }
            }
        } else {
            put_msg("\"%s\" is read only", file_name);
            return (0);
        }
    } else if (errno != ENOENT)
        return (0);            /* file does exist but stat fails */

    return (1);
}


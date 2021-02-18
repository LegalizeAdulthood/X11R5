/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *gl_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/gl.c,v 2.8 1991/09/12 15:51:23 bobg Exp $";


/* Methods for the GroupList data type for the Netnews Snarfer
*/

#include <big.h>

static void     GLDeleteRecentEntries();
static void     GLSort();
static void     GLSortAndComputeRefs();

void	    GLInit(gl)
GList_t	*gl;
{
    gl->num = gl->size = 0;
    gl->entries = (GListEntry_t *) NULL;
}

/* This function returns the number of entries added to the list,
** or a negative error code
*/

int	     GLBuildList(gl, dir)
GList_t	*gl;
char	   *dir;
{
    DIR	    *dp, *opendir();
    struct direct  *dirent, *readdir();
    int	     startNum = gl->num;

    Verbiage(3, "In GLBuildList");
    if (!(dp = opendir(dir)))
	return (CANT_OPEN_DIR);
    while ((dirent = readdir(dp)) && GLAddEntries(gl, dir, dirent));
    closedir(dp);		/* BOGUS -- does this have a return value? */
    Verbiage(2, "Added all possible entries; now sorting...");
    GLSortAndComputeRefs(gl);
    return (gl->num - startNum);
}

static int      GLAddEntries(gl, dir, dirent)
GList_t	*gl;
char	   *dir;
struct direct  *dirent;
{
    int	     fd, dirNameLen = strlen(dir), added = 0;
    char	    groups[GROUPSLINESIZE], *groupStart, *groupEnd, *index(), fullName[MAXPATHLEN + 1];
    long	    time;

    Verbiage(3, "In GLAddEntries");
    strcpy(fullName, dir);
    if (dirent->d_name[0] != '.') {	/* Ignore dot files */
	strcpy(fullName + dirNameLen, dirent->d_name);
	Verbiage(1, "Working on file:");
	Verbiage(1, fullName);
	if ((fd = open(fullName, O_RDONLY)) < 0)
	    return (FALSE);	/* Nonfatal error */
	if (FindNewsgroupsAndMaybeTime(fd, groups, &time)) {
	    groupStart = groups;
	    while (groupStart) {
		if (groupEnd = index(groupStart, ','))
		    *groupEnd = '\0';
		if (NeedNewsgroup(groupStart)) {
		    if (GLAddEntry(gl, dirent->d_name, groupStart, time))
			++added;
		    else {
			Verbiage(1, "GLAddEntry failed; deleting recent entries");
			GLDeleteRecentEntries(gl, added);
			close(fd);	/* BOGUS -- is there a return value? */
			return (FALSE);
		    }
		}
		groupStart = groupEnd ? groupEnd + 1 : NULL;
	    }
	}
	else {
	    Verbiage(1, "No newsgroup found; sending to \"lost\"");
	    strcpy(groups, LOSTDIR);
	    if (!GLAddEntry(gl, dirent->d_name, groups, time)) {
		Verbiage(1, "GLAddEntry failed");
		close(fd);	/* BOGUS -- return value? */
		return (FALSE);
	    }
	}
	close(fd);		/* BOGUS -- return value? */
    }
    return (TRUE);
}

static int      NeedNewsgroup(newsgroup)
char	    *newsgroup;
{
	static struct Gp {char *Name; int Len;} *Grps = NULL;
	static int numGrps = 0;
	int Ix;

	if (Globals.Options.ActiveGroups == NULL) return (TRUE);
	if (Grps == NULL) {
		char *P, *Q;

		numGrps = 0;
		P = Globals.Options.ActiveGroups;
		while (P != NULL) {
			Q = index(P, ',');
			++numGrps;
			P = Q ? Q+1 : NULL;
		}
		Grps = (struct Gp *) malloc(numGrps * sizeof(struct Gp));
		if (Grps != NULL) {
			Verbiage(1, "Parsed active newsgroup list to:");
			Ix = 0;
			P = Globals.Options.ActiveGroups;
			while (P != NULL) {
				Q = index(P, ',');
				if (Q != NULL) *Q++ = '\0';
				Grps[Ix].Name = P;
				Grps[Ix].Len = strlen(P);
				Verbiage(1, Grps[Ix].Name);
				++Ix;
				P = Q;
			}
		}
	}
	for (Ix = 0; Ix < numGrps; ++Ix) {
		if (strncmp(newsgroup, Grps[Ix].Name, Grps[Ix].Len) == 0) {
			if (newsgroup[Grps[Ix].Len] == '.') return (TRUE);
		}
	}
	Verbiage(1, "Dropping garbage newsgroup:");
	Verbiage(1, newsgroup);
	return (FALSE);
}

static int      GLAddEntry(gl, filename, newsgroup, time)
GList_t	*gl;
char	   *filename, *newsgroup;
long	    time;
{
    char	   *filenameBuf, *newsgroupBuf;

    Verbiage(3, "In GLAddEntry");
    if (!GLGrowIfNecessary(gl)) {
	Verbiage(1, "GLGrowIfNecessary failed");
	return (FALSE);
    }
    if (!(filenameBuf = STFindOrMake(&(Globals.STable), filename))) {
	Verbiage(1, "STFindOrMake of filenameBuf failed on string:");
	Verbiage(1, filenameBuf);
	return (FALSE);
    }
    if (!(newsgroupBuf = STFindOrMake(&(Globals.STable), newsgroup))) {
	Verbiage(1, "STFindOrMake of newsgroupBuf failed on string:");
	Verbiage(1, newsgroupBuf);
	return (FALSE);
    }
    GLESet(&(gl->entries[gl->num]), filenameBuf, newsgroupBuf, 0, 0, 0, time);
    ++(gl->num);
    return (TRUE);
}

static int      GLGrowIfNecessary(gl)
GList_t	*gl;
{
    GListEntry_t   *tmp;

    Verbiage(3, "In GLGrowIfNecessary");
    if (gl->num == gl->size) {
	if (gl->size >= GL_TOOBIG)
	    return (FALSE);
	Verbiage(2, "Attempting to grow the Grouplist");
	if (gl->size) {
	    if (!(tmp = (GListEntry_t *) realloc(gl->entries, (gl->size + GL_GROWSIZE) * sizeof(GListEntry_t))))
		return (FALSE);
	    gl->entries = tmp;
	}
	else {
	    if (!(tmp = (GListEntry_t *) malloc(GL_GROWSIZE * sizeof(GListEntry_t))))
		return (FALSE);
	    gl->entries = tmp;
	}
	gl->size += GL_GROWSIZE;
    }
    return (TRUE);
}

static void     GLDeleteRecentEntries(gl, num)
GList_t	*gl;
int	     num;
{
    gl->num -= num;
}

static void     GLSortAndComputeRefs(gl)
GList_t	*gl;
{
    char	   *currentFile = NULL;
    int	     i, j, numRefs = 0;
    GListEntry_t   *base = 0;

    for (i = 0; i < gl->num; ++i) {
	if (currentFile) {
	    if (currentFile != GLEGetFilename(&(gl->entries[i]))) {
		/* We can do this since all strings are stored only once */
		GLSort(base, numRefs);
		for (j = 0; j < numRefs; ++j) {
		    GLESetAhead(&(base[j]), numRefs - j - 1);
		    GLESetBefore(&(base[j]), j);
		}
		currentFile = GLEGetFilename(base = &(gl->entries[i]));
		numRefs = 1;
	    }
	    else
		++numRefs;
	}
	else {
	    currentFile = GLEGetFilename(base = &(gl->entries[i]));
	    numRefs = 1;
	}
    }
    if (currentFile) {
	GLSort(base, numRefs);
	for (j = 0; j < numRefs; ++j) {
	    GLESetAhead(&(base[j]), numRefs - j - 1);
	    GLESetBefore(&(base[j]), j);
	}
    }
    GLSort(gl->entries, gl->num);
}

static void     GLSort(base, num)
GListEntry_t   *base;
int	     num;
{
    qsort(base, num, sizeof(GListEntry_t), GLECompare);
}

void	    GLPurge(gl)
GList_t	*gl;
{
    if (gl->size)
	free(gl->entries);
    gl->size = gl->num = 0;
    gl->entries = NULL;
}

/* This one looks in the file for the Newsgroups line and the Date line.
** If Newsgroups is not found, then FALSE is returned, otherwise TRUE.
** "groups" is filled in with the contents of the newsgroups header,
** and time is filled in with the Unix-style time which is parsed from the
** date header (if found), or zero (if not found).
*/

static int      FindNewsgroupsAndMaybeTime(fd, groups, time)
int	     fd;
char	   *groups;
long	   *time;
{
    int	     foundNewsgroups = FALSE, foundTime = FALSE, looking = TRUE, numToRead = FILECHUNKSIZE, foo;
    char	    buffer[FILECHUNKSIZE + 1], *startPtr, *endPtr, *bufPtr = buffer;

    *time = 0L;			/* In case we never find it */
    while (looking) {
	if (read(fd, bufPtr, numToRead) < 1)
	    looking = FALSE;
	else {
	    startPtr = buffer;
	    while (startPtr) {
		if (*startPtr) {
		    if (endPtr = index(startPtr, '\n')) {
			if (startPtr == endPtr)	/* End of headers! */
			    looking = FALSE;
			else {
			    *endPtr = '\0';
			    if (!foundNewsgroups && !strncmp("Newsgroups: ", startPtr, 12)) {
				strcpy(groups, startPtr + 12);
				foundNewsgroups = TRUE;
			    }
			    if (!foundTime && !strncmp("Date: ", startPtr, 6)) {
				MS_ParseDate(startPtr + 6, &foo, &foo, &foo, &foo, &foo, &foo, &foo, time);

				/*
				 * We ignore the return value; errors don't
				 * matter 
				 */
				foundTime = TRUE;
			    }
			    looking = !(foundNewsgroups && foundTime);
			}
			startPtr = endPtr + 1;
		    }
		    else {	/* We have a partial line.  Move it to the
				 * beginning of the buffer, then read in more
				 * after it */
			strcpy(buffer, startPtr);
			bufPtr = buffer + strlen(startPtr);
			numToRead = FILECHUNKSIZE - strlen(startPtr);
			startPtr = NULL;
		    }
		}
		else {		/* The end of the line is also the end of the
				 * buffer */
		    bufPtr = buffer;
		    numToRead = FILECHUNKSIZE;
		    startPtr = NULL;
		}
	    }
	}
    }
    return (foundNewsgroups);
}

int	     GLGetNum(gl)
GList_t	*gl;
{
    return (gl->num);
}

GListEntry_t   *GLGetEntry(gl, i)
GList_t	*gl;
int	     i;
{
    return (&(gl->entries[i]));
}

/* This function sets the "ignore" flag for all entries in gl
** whose filenames are filename.
*/

void	    GLIgnore(gl, filename)
GList_t	*gl;
char	   *filename;
{
    int	     i;

    for (i = 0; i < gl->num; ++i) {
	if (!strcmp(filename, GLEGetFilename(&(gl->entries[i]))))
	    GLESetIgnore(&(gl->entries[i]), TRUE);
    }
}

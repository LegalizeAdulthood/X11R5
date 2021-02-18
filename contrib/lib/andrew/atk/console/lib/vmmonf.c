/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmonf.c,v 2.9 1991/09/20 18:20:13 gk5g Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmonf.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/vmmonf.c,v 2.9 1991/09/20 18:20:13 gk5g Exp $";
#endif /* lint */

#include <andrewos.h>
#include <class.h>
#include <im.ih>
#include <conclass.ih>
#include <console.h>
#include <sitevars.h>
#include <menulist.ih>

#include <errno.h>

extern char *index();

#define	GETSTAT_MASK   64   /* Getstats -is- dead, Restart Getstats -is- showing */

RestartStats(self)
struct consoleClass *self;
{
    mydbg(("entering: RestartStats\n"));
    self->menuMask &= ~GETSTAT_MASK;
    menulist_SetMask(self->stdMenulist, self->menuMask);
    /* remove menu item */
    InitStats(self);
    consoleClass_PostMenus(self, self->stdMenulist);
}

EndStats(self, vmstats, error, type, fatal)
struct consoleClass *self;
FILE *vmstats;
char *error;
char *type;
int fatal;
{
    char buffer[256];
    
    mydbg(("entering: EndStats\n"));
    sprintf(buffer, "%sconsole:<getstats/%s> %s - %s not monitor Disk and GVMStats", (fatal) ? "ERROR: " : "", type, error, (fatal) ? "will" : "might");
    ReportInternalError(self, buffer);
    if (fatal){
	im_RemoveFileHandler(vmstats);
	fclose(vmstats);
	self->menuMask |= GETSTAT_MASK;
	menulist_SetMask(self->stdMenulist, self->menuMask);
	consoleClass_PostMenus(self, self->stdMenulist);
	ReportInternalError(self, "console: ->> Try 'Restart Stats' Menu <<-");
    }
}
    
ComputeStatistics(vmstats, self)
FILE *vmstats;
struct consoleClass *self;
{
    int id = 0, val = 0, num = 0;
    char dname[50], buf[200];

    mydbg(("entering: ComputeStatistics\n"));
    errno = 0;

    if (fgets(buf, sizeof(buf), vmstats) == NULL) {
	if (errno != EWOULDBLOCK) {
	    EndStats(self, vmstats, "EOF sent", "pipe", TRUE);
	}
	return;
    }
    num = sscanf(buf, "%d:%d:%48s", &id, &val, dname);
    if (num < 2 || num > 3){
	EndStats(self, vmstats, "Received too few elements from pipe", "pipe", FALSE);
    }
    else {
	if (num == 3){
	    int len = strlen(dname);
	    if (Numbers[id].RawText == Nullity){ /* I'd like a better test */
		Numbers[id].RawText = (char *)malloc(len + 1);
		strcpy(Numbers[id].RawText, dname); /* should this be strNcpy ? */
	    }
	    return;
	    /* if a string was found, assume no value worth looking at */
	}
	else {
	    switch (id){
		case LOADCPU:
		case LOADIO:
		    if (DoLOAD)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case LOADUSER:
		case LOADSYS:
		case LOADIDLE:
		    if (DoCPU)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case VM:
		    if (DoVM)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case PAGEIN:
		case PAGEOUT:
		case PAGEREPLACABLE:
		case PAGEDEFICIT:
		    if (DoPAGING)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case MEMACTIVE:
		case MEMFREE:
		    if (DoMEM)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case QUEUERUN:
		case QUEUEBLOCK:
		case QUEUEMEM:
		    if (DoQUEUE)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case PROCSUSER:
		case PROCSTOTAL:
		case PROCSOTHER:
		    if (DoPROCESSES)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
		case INTSIO:
		case INTSSYS:
		case INTSSWAP:
		    if (DoINTS)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
#ifdef sun
		case NDSTATIN:
		case NDSTATOUT:
		case NDSTATERR:
		    if (DoNDSTAT)
			NewValue(self, &Numbers[id], val, NULL, FALSE);
		    break;
#endif /* sun */
		case PARSE_ERR_1:
		    EndStats(self, vmstats, "Wrong number of arguments", "parser", TRUE);
		    break;
		case PARSE_ERR_2:
		    EndStats(self, vmstats, "Invalid polling frequency (< 1)", "parser", TRUE);
		    break;
		case GVM_ERR_1:
		    EndStats(self, vmstats, "Cannot read proc table", "gvm", TRUE);
		    break;
		case GVM_ERR_2:
		    EndStats(self, vmstats, "nlist (/vmunix) failed", "gvm", TRUE);
		    break;
		case GVM_ERR_3:
		    EndStats(self, vmstats, "No namelist (/vmunix)", "gvm", TRUE);
		    break;
		case GVM_ERR_4:
		    EndStats(self, vmstats, "Cannot open /dev/kmem", "gvm", TRUE);
		    break;
		case DISK_ERR_1:
		    EndStats(self, vmstats, "setmntent failed", "disk", TRUE);
		    break;
		case DISK_ERR_2:
		    EndStats(self, vmstats, "setfsent failed", "disk", TRUE);
		    break;
		case DISK_ERR_3:
		    EndStats(self, vmstats, "Screwy /etc/fstab entry", "disk", FALSE);
		    break;
		case DISK_ERR_4:
		    EndStats(self, vmstats, "Disk mounted on unknown device", "disk", FALSE);
		    break;
		case DISK_ERR_5:
		    EndStats(self, vmstats, "Dismounted disk? (EIO)", "disk", FALSE);
		    break;
		default:
		    if (id >= DISK1 && id <= MAXGETSTATSCOUNTERS){
			if (Numbers[id].IsDisplaying)
			    NewValue(self, &Numbers[id], val, NULL, FALSE);
			break;
		    }
		    else{
		    /* else ignore value - not worth complaining about and worrying user */
			mydbg(("Unknown ID value received from Getstats: %d\n", id));
		    }
	    }
	}
    }
}

CheckWindows(self) 
struct consoleClass *self;
{
    int count, max;

    mydbg(("entering: CheckWindows\n"));
    /*    wm_NumWindows(&count, &max); */
    /* What should we replace this with - for now I am going to make
     * count = 1 and max = 10 *sigh*
     */
    count = 1;
    max = 10;
    NewValue(self, &Numbers[WINDOWUSE], max ? count * 100 /max : -1, NULL, FALSE);
}

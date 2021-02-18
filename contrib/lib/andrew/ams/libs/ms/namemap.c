/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *namemap_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/namemap.c,v 2.8 1991/09/12 15:46:15 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/namemap.c,v 2.8 1991/09/12 15:46:15 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/namemap.c,v $
*/
#include <stdio.h>
#include <ms.h>
#include <andrewos.h> /* sys/file.h */
#include <sys/stat.h>

extern FILE *fopen();

MS_NameSubscriptionMapFile(Root, MapFile)
char *Root;  /* Parameter passed in from CUI */
char *MapFile; /* Buffer to be filled and sent back by this routine */
{
    char    PublicMap[MAXPATHLEN + 1], File1[MAXPATHLEN+1], File2[MAXPATHLEN+1];
    int     code;
    FILE *mfp, *mmfp;

    debug(1,("MS_NameSubscriptionMapFile %s\n", Root ? Root : "<no root>"));
    if (!Root || !*Root) {
	/* Want a subscription map for everything we subscribe to ONLY */
	GenTempName(MapFile);
	return(WriteSimpleSubsMap(MapFile));
    }
    (void) DeSymLink(Root, File1, 0);
    if (abspath(File1, File2) != 0) strcpy(File2, File1);
    sprintf(PublicMap, "%s/%s", File2, AMS_SUBSCRIPTIONMAPFILE);
    debug(4,("Looking for map file %s\n", PublicMap));
    if (access(PublicMap, R_OK) != 0) {
	debug(4, ("Rebuilding public map file\n"));
	if ((mfp = fopen(PublicMap, "w")) == NULL) {
	    GenTempName(PublicMap);
	    debug(4,("Have to rebuild in a temporary spot: %s (errno %d)\n", PublicMap, errno));
	    if ((mfp = fopen(PublicMap, "w")) == NULL) {
		AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_NAMESUBSCRIPTIONMAP);
	    }
	}
	if ((code = SubsTreeWalk(mfp, File2)) != 0) {
	    fclose(mfp);
	    return(code);
	}
	if (ferror(mfp) || feof(mfp)) {
	    fclose(mfp);
	    AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_NAMESUBSCRIPTIONMAP);
	}
	if (vfclose(mfp) != 0) {
	    AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_NAMESUBSCRIPTIONMAP);
	}
    }
    debug(4, ("Now it is time for my private map file\n"));
    GenTempName(MapFile);
    if ((mfp = fopen(PublicMap, "r")) == NULL) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_NAMESUBSCRIPTIONMAP);
    }
    if ((mmfp = fopen(MapFile, "w")) == NULL) {
	fclose(mfp);
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_NAMESUBSCRIPTIONMAP);
    }
    if ((code = BuildPrivateSubscriptionMap(mfp, mmfp, File2)) != 0) {
	fclose(mfp);
	fclose(mmfp);
	return(code);
    }
    if (ferror(mmfp) || feof(mmfp)) {
	fclose(mmfp);
	fclose(mfp);
	AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_NAMESUBSCRIPTIONMAP);
    }
    if (vfclose(mmfp) != 0) {
	fclose(mfp);
	AMS_RETURN_ERRCODE(errno, EIN_VCLOSE, EVIA_NAMESUBSCRIPTIONMAP);
    }
    if (ferror(mfp)) {
	fclose(mfp);
	AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_NAMESUBSCRIPTIONMAP);
    }
    if (vfclose(mfp) != 0) {
	AMS_RETURN_ERRCODE(errno, EIN_VCLOSE, EVIA_NAMESUBSCRIPTIONMAP);
    }
    return(0);
}

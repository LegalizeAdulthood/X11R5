/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *apndfile_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/apndfile.c,v 2.9 1991/09/12 15:41:21 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/apndfile.c,v 2.9 1991/09/12 15:41:21 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/apndfile.c,v $
*/
#include <andrewos.h>
#include <ms.h>

extern char    *rindex();

MS_AppendFileToFolder(FileName, FolderName)
char           *FileName, *FolderName;          /* BOTH IN */
{
    debug(1, ("MS_AppendFileToFolder %s %s\n", FileName, FolderName));
    return (AppendFileToFolder(FileName, FolderName, TRUE));
}

AppendFileToFolder(FileName, FolderName, DoDelete)
char           *FileName, *FolderName;
int             DoDelete;
{
    int             errsave = 0;
    struct MS_Directory *Dir = NULL;

    CloseDirsThatNeedIt();
    if (ReadOrFindMSDir(FolderName, &Dir, MD_APPEND)) {
        errsave = mserrcode;
        if(Dir) CloseMSDir(Dir, MD_APPEND);
        return (errsave);
    }
    errsave = AppendFileToMSDir(FileName, Dir, DoDelete);
    mserrcode = CloseMSDir(Dir, MD_APPEND);
    return (errsave ? errsave : mserrcode);
}


AppendFileToMSDir(FileName, Dir, DoDelete)
char           *FileName;
struct MS_Directory *Dir;
int             DoDelete;
{
    return (AppendFileToMSDirInternal(FileName, Dir, DoDelete, FALSE));
}

AppendFileToMSDirPreservingFileName(FileName, Dir, DoDelete)
char           *FileName;
struct MS_Directory *Dir;
int             DoDelete;
{
    return (AppendFileToMSDirInternal(FileName, Dir, DoDelete, TRUE));
}

AppendFileToMSDirInternal(FileName, Dir, DoDelete, TreatAsAlien)
char           *FileName;
struct MS_Directory *Dir;
int             DoDelete;
int             TreatAsAlien;
{
    struct MS_Message *Msg;
    struct MS_CaptionTemplate CapTemplate;
    int             saveerr;
    char            NewFileName[1 + MAXPATHLEN];

    Msg = (struct MS_Message *) malloc(sizeof(struct MS_Message));
    if (Msg == NULL) {
        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_APPENDMESSAGETOMSDIR);
    }
    bzero(Msg, sizeof(struct MS_Message));
    Msg->OpenFD = -1;
    bzero(&CapTemplate, sizeof(struct MS_CaptionTemplate));
    CapTemplate.basictype = BASICTEMPLATE_NORMAL;
    CapTemplate.datetype = DATETYPE_FROMHEADER;
    if (ReadRawFile(FileName, Msg, DoDelete)
        || ParseMessageFromRawBody(Msg)
        || CheckAuthUid(Msg)
        || BuildDateField(Msg, DATETYPE_FROMHEADER)
        || BuildReplyField(Msg)
        || BuildAttributesField(Msg)
        || InventID(Msg)
        || BuildCaption(Msg, &CapTemplate, TRUE)) {
        saveerr = mserrcode;
        FreeMessage(Msg, TRUE);
        return (saveerr);
    }
    if (TreatAsAlien) {
        char           *s;

        s = rindex(FileName, '/');
        if (s)
            *s++ = NULL;
        if (!s || strcmp(Dir->UNIXDir, FileName)
            || (strlen(s) >= AMS_IDSIZE)) {
            FreeMessage(Msg, TRUE);
            AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_APPENDMESSAGETOMSDIR);
        }
        strcpy(AMS_ID(Msg->Snapshot), s);
        DoDelete = FALSE;              /* Cannot allow it! */
    }
    if (IsMessageAlreadyThere(Msg, Dir)) {
        if (DoDelete)
            unlink(FileName);
        return (0);
    }
    if (!TreatAsAlien) {
        sprintf(NewFileName, "%s/+%s", Dir->UNIXDir, AMS_ID(Msg->Snapshot));
        if (WritePureFile(Msg, NewFileName, FALSE, 0644)) {
            saveerr = mserrcode;
            FreeMessage(Msg, TRUE);
            return (saveerr);
        }
    }
    if (AppendMessageToMSDir(Msg, Dir)) {
        FreeMessage(Msg, TRUE);
        unlink(NewFileName);           /* The old copy of the file is still in
                                        * place */
        return (mserrcode);
    }
    FreeMessage(Msg, TRUE);
    if (DoDelete)
        unlink(FileName);              /* Errors here are funny; better an
                                        * orphan file than a bogus error
                                        * message, though */
    return (0);
}

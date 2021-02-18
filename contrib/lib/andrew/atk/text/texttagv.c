/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/texttagv.c,v 1.3 1991/09/12 16:35:59 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/texttagv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/texttagv.c,v 1.3 1991/09/12 16:35:59 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <texttag.ih>
#include <texttagv.eh>
#include <environ.ih>
#include <fontdesc.ih>
#define DataObject(A) (A->header.view.dataobject)
#define Data(A) ((struct texttag *)(DataObject(A)))


void texttagv__Print(self, f, process, final, toplevel)
struct texttagv *self;
FILE *f;
char *process;
char *final;
int toplevel;
{
    struct texttag *tag;
    char buf[256];
    if(environ_Get("IndexOnly") != NULL) return;
    tag = Data(self);
    texttag_GetTag(tag,255,buf);
    fprintf(f,".iy \"TEXTTAG %s\"\n",buf);
}
struct view *texttagv__Hit(self,action,mousex,mousey,numberOfClicks) 
struct texttagv *self;
enum view_MouseAction action;
long mousex, mousey, numberOfClicks;
{
    return super_Hit(self,action,mousex,mousey,numberOfClicks) ;
}
#define FONTNAME "andy"
#define FONTSIZE 12
#define OFNAME "andy"
#define OFSIZE 8
boolean texttagv__InitializeObject(classID,self)
struct classheader *classID;
struct texttagv *self;
{
    struct fnotev *fv = (struct fnotev *) self;
    texttagv_SetDisplayStr(self,"@");
    fv->fd = fontdesc_Create(FONTNAME,0,FONTSIZE);
    fv->ofd = fontdesc_Create(OFNAME,0,OFSIZE);
    return TRUE;
}
boolean texttagv__InitializeClass(classID)
struct classheader *classID;
{
    return TRUE;
}

/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/dict.c,v 2.5 1991/09/12 16:31:23 bobg Exp $ */
/* $ACIS:dict.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/dict.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/dict.c,v 2.5 1991/09/12 16:31:23 bobg Exp $";
#endif /* lint */


#include <class.h>
#include <dict.eh>
#include <view.ih>

#define INITIALSIZE 128
#define dictionary_DELETED NULL
#define EntryDeleted(A) (A->view == (struct view *) dictionary_DELETED &&  A->id == (char *)dictionary_DELETED)
#define DeleteEntry(A)  A->view = (struct view *)dictionary_DELETED ; A->id = (char *) dictionary_DELETED ;
struct dirtable {
    struct view *view;
    char *id;
    char *object;
};
static struct dirtable *table, *last, *end;
boolean dictionary__InitializeClass(classID)
    struct classheader *classID;
{
    table = (struct dirtable *) malloc(INITIALSIZE * sizeof(struct dirtable));
    end = &(table[INITIALSIZE]);
    last = table;

    return TRUE;
}
void dictionary__Insert(classID, view,id,object)
struct classheader *classID;
register struct view *view;
char *id; 
char *object;
{
    register struct dirtable *dd,*freeref;
    freeref = NULL;
    for(dd = table; dd < last; dd++){
        if(dd->view == view && dd->id == id) break;
        if(freeref == NULL && EntryDeleted(dd)) freeref = dd;
    }
    if(dd == last) {
        if(freeref != NULL){
            dd = freeref;
        }
        else {
            if(last == end){
                int newsize = INITIALSIZE + (end - table) ;
		long diff = last - table; 
                table = (struct dirtable *) realloc(table,newsize * sizeof(struct dirtable));
		if(table == NULL){
			fprintf(stderr,"Out Of Memory in dict.c\n");
			return;
			} 
                end  = &(table[newsize]);
		last = table + diff;
		dd = last;
            }
            last++;
        }
    }
    dd->view = view;
    dd->id = id;
    dd->object = object;
}
char *dictionary__LookUp(classID, view,id)
struct classheader *classID;
register struct view *view;
char *id;
{
    register struct dirtable *dd;
    for(dd = table; dd < last; dd++)
        if(dd->view == view && dd->id == id) return(dd->object);
    return(NULL);
}
long dictionary__CountViews(classID, id)
struct classheader *classID;
char *id;
{
    register struct dirtable *dd;
    long i = 0;
    for(dd = table; dd < last; dd++){
        if(dd->id == id && dd->view != NULL){
            i++;
        }
    }
    return i;
}
long dictionary__ListViews(classID, id,viewarray,len)
struct classheader *classID;
char *id;
struct view **viewarray;
long len;
{
    register struct dirtable *dd;
    long i = 0;
    for(dd = table; dd < last; dd++){
        if(dd->id == id && dd->view != NULL ){
            viewarray[i++] = dd->view;
            if(i == len) break;
        }
    }
    return i;
}
long dictionary__CountRefs(classID, view)
struct classheader *classID;
register struct view *view;
{
    register struct dirtable *dd;
    long i = 0;
    for(dd = table; dd < last; dd++){
        if(dd->view == view ){
            i++;
        }
    }
    return i;
}
long dictionary__ListRefs(classID, view,refarray,len)
struct classheader *classID;
register struct view *view;
char **refarray;
long len;
{
    register struct dirtable *dd;
    long i = 0;
    for(dd = table; dd < last; dd++){
        if(dd->view == view){
            refarray[i++] = dd->id;
            if(i == len) break;
        }
    }
    return i;
}

void dictionary__Delete(classID, view,id)
struct classheader *classID;
register struct view *view;
char *id;
{
    register struct dirtable *dd;
    for(dd = table; dd < last; dd++)
        if(dd->view == view && dd->id == id){
            DeleteEntry(dd);
            return;
        }
}

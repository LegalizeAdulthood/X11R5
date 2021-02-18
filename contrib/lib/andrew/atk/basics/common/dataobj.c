/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/dataobj.c,v 2.14 1991/09/12 15:58:04 bobg Exp $ */
/* $ACIS:dataobj.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/dataobj.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/dataobj.c,v 2.14 1991/09/12 15:58:04 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <dataobj.eh>
#include <attribs.h>
#include <view.ih>
/* #include "dict.ih" */

boolean dataobject__InitializeObject(classID, self)
    struct classheader *classID;
    register struct dataobject *self;
{
    self->id = dataobject_UniqueID(self);
    self->writeID = dataobject_UNDEFINEDID;
    self->modified = 0;
    self->properties = NULL;
    return TRUE;
}

long dataobject__Read(self, file, id)
    struct dataobject *self;
    FILE *file;
    long id;
{

/*  The following may be used as a template for creating real read routines 
    for objects that may contain other objects */

    long endcount = 1;
    boolean begindata;
    char *s;
    long c;
    long status;
    char objectname[200];
    long objectid;
    struct dataobject *newobject;

    dataobject_SetID(self,dataobject_UniqueID(self));/* change id to unique number */
    while (endcount != 0)  {
        while ((c = getc(file)) != EOF && c != '\\')  {
	    if(endcount == 1){
		/* Place actual read code here */
	    }
        }
        if (c == EOF) return dataobject_NOREADERROR;
        if ((c = getc(file)) == EOF)
            return dataobject_PREMATUREEOF;
        if (c == 'b')  {
            begindata = TRUE;
            s = "egindata";
        }
        else if (c == 'e')  {
            begindata = FALSE;
            s = "nddata";
        }
        else  {
	    if(endcount == 1){
		/* Place handling of \x characters here */
	    }
            continue;
        }
        while ((c = getc(file)) != EOF && c == *s) s++;
        if (c == '{' && *s == '\0')  {
            if (begindata) {
                s = objectname;
                while ((c = getc(file)) != EOF && c != ',')
                    *s++ = c;
                if (c == EOF) return dataobject_PREMATUREEOF;
                *s = '\0';
                objectid = 0;
                while ((c = getc(file)) != EOF && c != '}')
                    if(c >= '0' && c <= '9')objectid = objectid * 10 + c - '0';
                if (c == EOF) return dataobject_PREMATUREEOF;
		if((c = getc(file))!= '\n') ungetc(c,file);
                /* Call the New routine for the object */
                if ((newobject = (struct dataobject *) class_NewObject(objectname)))  {
                    /* Register the object with the dictionary */
		    /* This call should be made, It is only commented out here
		     to avoid making the basics directory dependent on the support directory */
/*		    dictionary_Insert(NULL,(char *)objectid, (char *)newobject); */
                    /* Call the read routine for the object */
                    status = dataobject_Read(newobject, file, objectid);
		    if (status != dataobject_NOREADERROR) return status;
		}
                else {
                    endcount += 1;
		    /* return dataobject_OBJECTCREATIONFAILED; */
		}

	    }
            else  {
                endcount -= 1;
                while ((c = getc(file)) != EOF && c != '}');
		if((c = getc(file))!= '\n') ungetc(c,file);
            }
        }
        else if(endcount == 1){
	    
        /* 	    Place Handling of characters following \  
           */	}
    }
    return dataobject_NOREADERROR;
}


long dataobject__Write(self, file, writeID, level)
    struct dataobject *self;
    FILE *file;
    long writeID;
    int level;
{
    if (dataobject_GetWriteID(self) != writeID)  {
        dataobject_SetWriteID(self,writeID);
        fprintf(file, "\\begindata{%s,%ld}\n", class_GetTypeName(self),dataobject_GetID(self));
/*	place file writing code here */
        fprintf(file, "\\enddata{%s,%ld}\n", class_GetTypeName(self),dataobject_GetID(self));
    }

    return dataobject_GetID(self);
}


/* 
 * GetModified/SetModified
 * 
 * With every call to SetModified, a dataobject is assigned a 
 * modification timestamp.  The timestamp is not related to the time 
 * of day.  It is simply a long integer.  Timestamps are issued in 
 * increasing order, starting with 1.  Therefore, timestamps can be 
 * compared with the normal integer comparison functions (<, >, etc.)
 * 
 */

static long mod_time = 1;
#define NextTime()  (mod_time++)

long dataobject__GetModified(self)
    struct dataobject *self;
{
    return self->modified;
}

void dataobject__SetModified(self)
    struct dataobject *self;
{
    self->modified = NextTime();
}
void dataobject__RestoreModified(self, oldmodified)
    struct dataobject *self;
    long oldmodified;
{   /* allow the reseting of the modified flag to an old value. 
      This is NOT a supported function */
    self->modified = oldmodified;
}

static char viewname[100];

char *dataobject__ViewName(self)
    struct dataobject *self;
{
    sprintf(viewname, "%sview", class_GetTypeName(self));
    if (class_Load(viewname) != NULL
            && class_IsTypeByName(viewname,"view"))
        return viewname;

    viewname[strlen(viewname) - strlen("iew")] = '\0';
    if (class_Load(viewname) != NULL
            && class_IsTypeByName(viewname,"view"))
        return viewname;

    strcpy(viewname, "view");
    return viewname;
}

void dataobject__SetAttributes(self, attributes)
    struct dataobject *self;
    struct attributes *attributes;
{
}




void dataobject__Put( self, property, type, value )
     struct dataobject *self;
     struct atom * property;
     struct atom * type;
     long value;
{
  struct property * newprop =
    (struct property *)malloc(sizeof(struct property));

  newprop->type = type;
  newprop->data = value;
  if (self->properties == NULL)
      self->properties = namespace_New();
  namespace_SetValue( self->properties, property, (long) newprop );
}


short dataobject__Get( self, property, type, value )
     struct dataobject *self;
     struct atom * property;
     struct atom **type;
     long * value;
{
  struct property * prop;

  /* we found the property if: the name is bound, and the types match or 
     the specified type is NULL.  If type is not NULL, but *type is, then 
     we fill in the actual type. */

  if (self->properties != NULL && namespace_Boundp(self->properties, property, (long *) &prop)
		       && (type == NULL || *type == NULL || *type == prop->type))
    {
      if (value != NULL)
	*value = prop->data;
      if (type != NULL && *type == NULL)
	*type = prop->type;
      return TRUE;
    }
  else
    return FALSE;
}

dataobject__ListCurrentViews(self,array,size)
struct dataobject *self;
struct view **array;
int size;
{ 
    /* fills in the array (of size 'size') with a list of views
          observing this dataobj.
      Returns the number of views found, which may be greater than size.
      Note that to just get a count of views,
         this routine may be called with array = NULL and size = 0 */
    int i,count;
    struct observable *ob,**observers;
    ob = (struct observable *)self;
    count = 0; 
    for (i = 0, observers = ob->observers; i < ob->nObservers ; i++, observers++)
	if(class_IsTypeByName(class_GetTypeName(*observers),"view") &&
	   ((struct view *)(*observers))->dataobject == self){
	    if(count < size) *array++ = (struct view *)*observers;
	    count++;
	}
    if(count < size)  *array = NULL;
    return count;
}


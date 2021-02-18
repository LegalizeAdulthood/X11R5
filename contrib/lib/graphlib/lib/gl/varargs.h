/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Eric Anderson 
 * Buttons and menus mostly by Jeffrey Hutzelman
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */
#ifndef _GL_VARARGS_H
#define _GL_VARARGS_H

#include "X11/gl/sysdef.h"
#include "X11/gl/stddef.h"
typedef void (*VPTF)();
typedef GLPointer (*PointerPTF)();

#define GL_Done 0

typedef struct _GL_DataList {
  GLPointer data;
  VPTF key;/* Use the init_function as the key, 
	      as we can guarentee no overlap. Or at least some function.*/
  struct _GL_DataList *next;
} *GL_DataList;

GLPointer xmalloc(/* size*/);
#define XMALLOC(type) (type *)xmalloc(sizeof(type))
void CheckAlloc(/* Pointer to test, does not return if pointer is null. */);
GLPointer GL_Create();
void GL_Modify();
GLPointer GL_Search_DataList();
int GL_Key_In_DataList(/*List,Key*/);/* returns 1 if in */

GL_DataList GL_Init_DataList();
void GL_Add_To_DataList(/* List,Key,Data */);

/*
  Brand NEW WAY COOL FORMAT
   Oh wow gee neat huh?

Prob something like :
   GLPointer GL_Create(VPTF init_function,VPTF Handler, VPTF Execute, 
                    VPTF Destroy,PointerPTF Returner
                    ...All the standard varargs foo.
		    );
  VPTF init_function(GL_DataList *list);
  VPTF Handler(GL_DataList *list,long modifier,va_list *pvar);
  VPTF Execute(GL_DataList *list);
  VPTF Destroy(GL_DataList *list)
  PointerPTF Returner(GL_DataList *list);
*/

/*Very useful. */
#ifndef MAX(a,b)
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef MIN(a,b)
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#endif 

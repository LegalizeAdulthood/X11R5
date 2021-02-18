/*
 * $Id: rk_alloc.c,v 1.2 1991/09/16 21:34:38 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/***********************************************************************
			rk_alloc.c
						88. 6.13  �� ��

		�⡼�����ɽ���б�ɽ�γ����ǿ��ˤĤ��ơ� �³��ͤ�
		�ۤ��Ƥ�����פʤ褦�ˡ��ơ��֥�Υ����������Ĺ��
		��������ο������ؿ���
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#include "rk_extvars.h"
#ifdef	MULTI
#include "rk_multi.h"
#endif	/*MULTI*/
#endif
#include "rext.h"

static
void
ERRALLOC(n)
unsigned int	n;
{
	fprintf(stderr, "\r\nromkan:  ---\r\n");
	fprintf(stderr, "\tError No.%d: Can't memory alloc.\r\n", n);
	longjmp(cur_rk->env0, 1);
}

/* �⡼�����ɽ���ɤ߹��ߤΤ���γƼ�δ����ơ��֥���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_modetable_struct(ptr, maxsize, memsize)
fast	modetable	*ptr;
int	maxsize;
int	memsize;
{
    if ((ptr->max = maxsize) > 0) {
	if ((ptr->point =
	    (char **) malloc(ptr->max * sizeof(char *))) == NULL) {
		ERRALLOC(0);
	}
	ptr->point[0] = NULL;
    } else {
	ptr->point = NULL;
    }
    ptr->count = 0;
    if ((ptr->size = maxsize) > 0) {
	if ((ptr->org = ptr->next = (char *) malloc(ptr->size)) == NULL) {
		ERRALLOC(0);
	}
	ptr->org[0] = '\0';
    } else {
	ptr->org = ptr->next = NULL;
    }
}

/* �⡼�����ɽ���ɤ߹��ߤΤ���γƼ�δ����ơ��֥���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_modetable_struct(ptr)
fast	modetable	*ptr;
{
	if (ptr->point != NULL) {
		free(ptr->point);
		ptr->max = ptr->count = 0;
		ptr->point = NULL;
	}
	if (ptr->org != NULL) {
		free(ptr->org);
		ptr->size = 0;
		ptr->org = ptr->next = NULL;
	}
}

/* �⡼�����ɽ���ɤ߹��ߤΤ���γƼ�δ����ơ��֥���ΰ�Υ����å� */
#ifdef OMRON_LIB
static
#endif
void
check_and_realloc_for_modetable_struct(ptr, n, s, maxsize, memsize)
fast	modetable *ptr;
int	n;
char	*s;
int	maxsize;
int	memsize;
{
	fast	int	i;
	fast	int	ptrdiff;
	fast	char	*oldptr;

	if ((ptr->count = n + 1) > ptr->max) {
	    if ((ptr->max += maxsize) > 0) {
		if (ptr->point == NULL) {
		    if ((ptr->point = 
			(char **) malloc( ptr->max * sizeof(char *))) == NULL) {
			    ERRALLOC(0);
		    }
		} else {
		    if ((ptr->point = 
			(char **) realloc(ptr->point,
			    ptr->max * sizeof(char *))) == NULL) {
			    ERRALLOC(0);
		    }
		}
	    }
	}
	while ((ptr->org + ptr->size) < (ptr->next + strlen(s) + 1)) {
		if ((ptr->org = 
		    (char *) realloc(oldptr = ptr->org, ptr->size += memsize))
								== NULL) {
			ERRALLOC(0);
		}
		ptr->next += (ptrdiff = ptr->org - oldptr);
		for (i = 0; i < n; i++) {
			ptr->point[i] += ptrdiff;
		}
	}
}

/* �⡼�ɤ� on, off �Υ����å��ơ��֥�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_modesw(ptr, size)
fast	modeswtable	*ptr;
int	size;
{
    if ((ptr->max = size) > 0) {
	if ((ptr->point =
	    (int *) malloc(ptr->max * sizeof(int))) == NULL) {
		ERRALLOC(0);
	}
    } else {
	ptr->point = NULL;
    }
    ptr->count = 0;
}

/* �⡼�ɤ� on, off �Υ����å��ơ��֥�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_modesw(ptr)
fast	modeswtable	*ptr;
{
	if (ptr->point != NULL) {
		free(ptr->point);
		ptr->max = ptr->count = 0;
		ptr->point = NULL;
	}
}

/* �⡼�ɤ� on, off �Υ����å��ơ��֥�Υ����������å� */
#ifdef OMRON_LIB
static
#endif
void
check_and_realloc_for_modesw(ptr, n, size)
fast	modeswtable	*ptr;
int	n;
int	size;
{
	if ((ptr->count = n + 1) > ptr->max) {
	    if ((ptr->max += size) > 0) {
		if (ptr->point == NULL) {
		    if ((ptr->point = 
			(int *) malloc(ptr->max * sizeof(int))) == NULL) {
			    ERRALLOC(0);
		    }
		} else {
		    if ((ptr->point = 
			(int *) realloc(ptr->point,
				    ptr->max * sizeof(int))) == NULL) {
			    ERRALLOC(0);
		    }
		}
	    }
	}
}

/* �⡼�����ɽ���ɤ߹��ߤΤ�����ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_modebuf(ptr, size)
fast	modebuftable	*ptr;
int	size;
{
	if ((ptr->org = (char *)malloc(ptr->size = size + 1)) == NULL) {
		ERRALLOC(0);
	}
}

/* �⡼�����ɽ���ɤ߹��ߤΤ�����ΰ�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_modebuf(ptr)
fast	modebuftable	*ptr;
{
	if (ptr->org != NULL) {
		free(ptr->org);
		ptr->size = 0;
		ptr->org = NULL;
	}
}

/* �⡼��ɽ�����������Υơ��֥�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_modenaibu(ptr, size)
fast	modenaibutable	*ptr;
int	size;
{
	if ((ptr->org = ptr->next =
	    (int *) malloc((ptr->size = size + 1) * sizeof(int)))
								== NULL) {
		ERRALLOC(0);
	}
}

/* �⡼��ɽ�����������Υơ��֥�Υ������ѹ� */
#ifdef OMRON_LIB
static
#endif
void
realloc_for_modenaibu(ptr)
fast	modenaibutable	*ptr;
{
	if ((ptr->org =
	    (int *) realloc(ptr->org,
		(ptr->size = ptr->next - ptr->org + 1)
						* sizeof(int))) == NULL) {
		ERRALLOC(0);
	}
}

/* �⡼��ɽ�����������Υơ��֥�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_modenaibu(ptr)
fast	modenaibutable	*ptr;
{
	if (ptr->org != NULL) {
		free(ptr->org);
		ptr->size = 0;
		ptr->org = ptr->next = NULL;
	}
}

/* �б�ɽ�δ����ơ��֥���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_hyo(ptr, size)
fast	hyotable	*ptr;
int	size;	/* �б�ɽ����� */
{
	hyo	*hyoptr;
	fast	int	i;

    if ((ptr->size = size) > 0) {
	if ((ptr->point = (hyo *) malloc(ptr->size * sizeof(hyo))) == NULL) {
		ERRALLOC(0);
	}
	for (i = 0, hyoptr = ptr->point; i < size; i++, hyoptr++) {
		hyoptr->hyoshu = 0;
		hyoptr->data = NULL;
		hyoptr->hensudef = NULL;
		hyoptr->size = 0;
		hyoptr->memory = NULL;
	}
    } else {
	ptr->point = NULL;
    }
}

/* �б�ɽ�δ����ơ��֥���ΰ�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_hyo(ptr)
fast	hyotable	*ptr;
{
	if (ptr->point != NULL) {
		free(ptr->point);
		ptr->size = 0;
		ptr->point = NULL;
	}
}

/* �����ܡ���������б�ɽ�δ����ơ��֥���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_usehyo(ptr, size)
fast	usehyotable	*ptr;
int	size;	/* �б�ɽ����� */
{
	if ((ptr->usemaehyo = (int *)
	    malloc(((ptr->size = size) + 1) * sizeof(int) * 3)) == NULL) {
		ERRALLOC(0);
	}
	ptr->usehyo = (int *) (ptr->usemaehyo) + ptr->size + 1;
	ptr->useatohyo = (int *) (ptr->usehyo) + ptr->size + 1;
}

/* �����ܡ���������б�ɽ�δ����ơ��֥���ΰ�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_usehyo(ptr)
fast	usehyotable	*ptr;
{
	if (ptr->usemaehyo != NULL) {
		free(ptr->usemaehyo);
		ptr->size = 0;
		ptr->usemaehyo = ptr->usehyo = ptr->useatohyo = NULL;
	}
}

/* �б�ɽ�μ¥ǡ����ơ��֥���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_hyo_area(ptr, n, datasize, memsize, dataptrp, memptrp)
fast	hyotable	*ptr;
fast	int	n;		/* �б�ɽ���ֹ� */
int	datasize;
int	memsize;
dat	**dataptrp;
letter	**memptrp;
{
	if ((*dataptrp = ptr->point[n].data =
	    (dat *) malloc((datasize + 1) * sizeof(dat)))
								== NULL) {
		ERRALLOC(0);
	}
	if ((*memptrp = ptr->point[n].memory =
	    (letter *) malloc((ptr->point[n].size = memsize + 1)
						* sizeof(letter))) == NULL) {
		ERRALLOC(0);
	}
}

/* �б�ɽ�μ¥ǡ����ơ��֥���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_hyo_area(ptr, n)
fast	hyotable	*ptr;
fast	int	n;		/* �б�ɽ���ֹ� */
{
	if (ptr->point[n].data != NULL) {
		free(ptr->point[n].data);
		ptr->point[n].data = NULL;
	}
	if (ptr->point[n].memory != NULL) {
		free(ptr->point[n].memory);
		ptr->point[n].memory = NULL;
	}
}

/* ���б�ɽ���ѿ��δ����ơ��֥�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_hensuudef(ptr, size)
fast	hensuudeftable	*ptr;
int	size;
{
    if ((ptr->max = size) > 0) {
	if ((ptr->point = (letter **)
	    malloc(ptr->max * sizeof(letter *))) == NULL) {
		ERRALLOC(0);
	}
    } else {
	ptr->point = NULL;
    }
    ptr->count = 0;
}

/* ���б�ɽ���ѿ��δ����ơ��֥�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_hensuudef(ptr)
fast	hensuudeftable	*ptr;
{
	if (ptr->point != NULL) {
		free(ptr->point);
		ptr->max = ptr->count = 0;
		ptr->point = NULL;
	}
}

/* ���б�ɽ���ѿ��δ����ơ��֥�β��� */
#ifdef OMRON_LIB
static
#endif
void
check_and_realloc_for_hensuudef(ptr, n, size)
fast	hensuudeftable	*ptr;
int	n;
int	size;
{
	if ((ptr->count = n + 1) > ptr->max) {
	    if ((ptr->max += size) > 0) {
		if (ptr->point == NULL) {
		    if ((ptr->point = (letter **)
			malloc(ptr->max * sizeof(letter *))) == NULL) {
			    ERRALLOC(0);
		    }
		} else {
		    if ((ptr->point =
			(letter **) realloc(ptr->point,
			    ptr->max * sizeof(letter *))) == NULL) {
			    ERRALLOC(0);
		    }
		}
	    }
	}
}

/* �б�ɽ���ѿ��δ����ơ��֥�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_hensuu(ptr, maxsize, memsize)
fast	hensuutable	*ptr;
int	maxsize;
int	memsize;
{
    if ((ptr->max = maxsize) > 0) {
	if ((ptr->point =
	    (hensuset *) malloc(ptr->max * sizeof(hensuset))) == NULL) {
		ERRALLOC(0);
	}
    } else {
	ptr->point = NULL;
    }
    ptr->count = 0;
    if ((ptr->org = ptr->next =
	(letter *) malloc(((ptr->size = memsize) + 1) * sizeof(letter)))
							    == NULL) {
	    ERRALLOC(0);
    }
}

/* �б�ɽ���ѿ��δ����ơ��֥�γ��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_hensuu(ptr)
fast	hensuutable	*ptr;
{
	if (ptr->point != NULL) {
		free(ptr->point);
		ptr->max = ptr->count = 0;
		ptr->point = NULL;
	}
	if (ptr->org != NULL) {
		free(ptr->org);
		ptr->size = 0;
		ptr->org = ptr->next = NULL;
	}
}

/* �б�ɽ���ѿ��δ����ơ��֥�Υ����������å� */
#ifdef OMRON_LIB
static
#endif
void
check_and_realloc_for_hensuu(ptr, n, name, maxsize, memsize)
fast	hensuutable	*ptr;
int	n;
letter	*name;
int	maxsize;
int	memsize;
{
	fast	int	i;		/* V3.1 */
	fast	int	ptrdiff;	/* V3.1 */
	fast	letter	*oldptr;	/* V3.1 */

	if ((ptr->count = n + 1) > ptr->max) {
	    if ((ptr->max += memsize) > 0) {
		if (ptr->point == NULL) {
		    if ((ptr->point = 
			(hensuset *) malloc(ptr->max * sizeof(hensuset)))
			== NULL) {
			    ERRALLOC(0);
		    }
		} else {
		    if ((ptr->point = 
			(hensuset *) realloc(ptr->point,
			    ptr->max * sizeof(hensuset))) == NULL) {
			    ERRALLOC(0);
		    }
		}
	    }
	}
	while ((ptr->org + ptr->size) <
			(ptr->next + (ltrlen(name) + 1 + 1))) {

		if ((ptr->org = 
		    (letter *) realloc(oldptr = ptr->org,
			((ptr->size += memsize) + 1) * sizeof(letter)))
								== NULL) {
			ERRALLOC(0);
		}
		ptr->next += (ptrdiff = ptr->org - oldptr);
		for (i = 0; i < n; i++) {
			ptr->point[i].name += ptrdiff;
		}
	}
}

/* �ޥå������ѿ��δ����ơ��֥�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_henmatch(ptr, size)
fast	matchtable	*ptr;
int	size;	/* �ѿ������ */
{
    if ((ptr->size = size) > 0) {
	if ((ptr->point = (matchpair *)
	    malloc(ptr->size * sizeof(matchpair))) == NULL) {
		ERRALLOC(0);
	}
    } else {
	ptr->point = NULL;
    }
}

/* �ޥå������ѿ��δ����ơ��֥�β��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_henmatch(ptr)
fast	matchtable	*ptr;
{
	if (ptr->point != NULL) {
		free(ptr->point);
		ptr->size = 0;
		ptr->point = NULL;
	}
}

/* �б�ɽ���ɤ߹���Хåե����ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_hyobuf(ptr, size)
fast	hyobuftable	*ptr;
int	size;
{
    if ((ptr->size = size) > 0) {
	if ((ptr->org = ptr->next = (char *)malloc(ptr->size)) == NULL) {
		ERRALLOC(0);
	}
    } else {
	ptr->org = NULL;
    }
}

/* �б�ɽ���ɤ߹���Хåե����ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_hyobuf(ptr)
fast	hyobuftable	*ptr;
{
	if (ptr->org != NULL) {
		free(ptr->org);
		ptr->size = 0;
		ptr->org = ptr->next = NULL;
	}
}

/* �б�ɽ���ɤ߹���Хåե����ΰ�Υ����������å� */
#ifdef OMRON_LIB
static
#endif
void
check_and_realloc_for_hyobuf(ptr, size)
fast	hyobuftable	*ptr;
int	size;
{
	/* �б�ɽ���ɤ߹��� */
	if ((size + 2) > ptr->size) {
	    if (ptr->org == NULL) {
		if ((ptr->org = (char *)malloc(ptr->size = size + 2)) == NULL) {
			ERRALLOC(0);
		}
	    } else {
		if ((ptr->org =
		    (char *)realloc(ptr->org, ptr->size = size + 2)) == NULL) {
			ERRALLOC(0);
		}
	    }
	}
}

/* �ѿ����Ѱ�Υ���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
malloc_for_heniki(ptr, size)
fast	henikitable	*ptr;
int	size;
{
	if ((ptr->org =
	    (letter *) malloc((ptr->size = size + 1) *
					sizeof(letter))) == NULL) {
		ERRALLOC(0);
	}
}

/* �ѿ����Ѱ�Υ���ΰ�γ��� */
#ifdef OMRON_LIB
static
#endif
void
free_for_heniki(ptr)
fast	henikitable	*ptr;
{
	if (ptr->org != NULL) {
		free(ptr->org);
		ptr->size = 0;
		ptr->org = NULL;
	}
}

/* �ѿ����Ѱ�Υ���ΰ�Υ����������å� */
#ifdef OMRON_LIB
static
#endif
void
check_and_realloc_for_heniki(ptr, size)
fast	henikitable	*ptr;
int	size;
{
	if ((size + 1) > ptr->size) {
	    if (ptr->org == NULL) {
		if ((ptr->org =
		    (letter *) malloc((ptr->size = size + 1) * sizeof(letter)))
		    == NULL) {
			ERRALLOC(0);
		}
	    } else {
		if ((ptr->org =
		    (letter *) realloc(ptr->org,
			(ptr->size = size + 1) * sizeof(letter))) == NULL) {
			ERRALLOC(0);
		}
	    }
	}
}


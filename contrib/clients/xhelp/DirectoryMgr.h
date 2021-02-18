/****************************************************************************
 Copyright 1990 by Brian Totty, Free Widet Foundation
 See MultiList.c for full copyright information.

	DirectoryMgr.h

	This file contains the C declarations and definitions for the
	DirectoryMgr system.

	This system is intended to managed filtered and sorted directory
	lists.

 ****************************************************************************/

#ifndef _DIRECTORY_MGR_H_
#define	_DIRECTORY_MGR_H_

#include "Directory.h"

/*---------------------------------------------------------------------------*

        Simple DirectoryMgr Interface

		DirectoryMgrSimpleOpen();
		DirectoryMgrSimpleRefilter();
		DirectoryMgrSimpleResort();

	Standard DirectoryMgr Interface

		DirectoryMgrOpen();
		DirectoryMgrClose();
		DirectoryMgrRefilter();
		DirectoryMgrRefresh();
		DirectoryMgrResort();

	Moving Around Items

		DirectoryMgrGotoItem();
		DirectoryMgrGotoNamedItem();
		DirectoryMgrRestart();
		DirectoryMgrGetIndex();
		DirectoryMgrCurrentEntry();
		DirectoryMgrNextEntry();
		DirectoryMgrPrevEntry();

	Utility Functions

		DirectoryMgrSimpleFilterFunc();
		DirectoryMgrSimpleSortingFunc();

	Comparison Functions

		DirectoryMgrCompareName();
		DirectoryMgrCompareSizeAscending();
		DirectoryMgrCompareSizeDescending();

	Macros

		DirectoryMgrDir();
		DirectoryMgrData();
		DirectoryMgrSortedPtrs();
		DirectoryMgrFilterFunc();
		DirectoryMgrCompFunc();
		DirectoryMgrFilterData();
		DirectoryMgrFreeFilterData();
		DirectoryMgrTotalCount();
		DirectoryMgrFilteredCount();
		DirectoryMgrCurrentIndex();


 *---------------------------------------------------------------------------*/

#ifndef PFI
typedef int (*PFI)();
#endif

typedef struct entry_cons
{
	DIR_ENTRY dir_entry;
	struct entry_cons *next;
} DIR_ENTRY_CONS;

typedef	DIR_ENTRY_CONS DirEntryCons;

typedef struct
{
	DIRECTORY dir;
	DIR_ENTRY *data;
	DIR_ENTRY **sorted_ptrs;
	int total_count;
	int filtered_count;
	PFI filter_func;
	char *filter_data;
	int free_filter_data;
	PFI comp_func;
	int current_index;
} DIRECTORY_MGR;

typedef	DIRECTORY_MGR DirectoryMgr;

#define	DIR_MGR_SORT_NONE			0
#define	DIR_MGR_SORT_NAME			1
#define	DIR_MGR_SORT_NAME_DIRS_FIRST		2
#define DIR_MGR_SORT_SIZE_ASCENDING		3
#define DIR_MGR_SORT_SIZE_DESCENDING		4
#define	DIR_MGR_SORT_ACCESS_ASCENDING		5
#define	DIR_MGR_SORT_ACCESS_DESCENDING		6

#define	DirectoryMgrDir(dm)		(&((dm)->dir))
#define	DirectoryMgrData(dm)		((dm)->data)
#define	DirectoryMgrSortedPtrs(dm)	((dm)->sorted_ptrs)
#define	DirectoryMgrFilterFunc(dm)	((dm)->filter_func)
#define	DirectoryMgrCompFunc(dm)	((dm)->comp_func)
#define	DirectoryMgrFilterData(dm)	((dm)->filter_data)
#define	DirectoryMgrFreeFilterData(dm)	((dm)->free_filter_data)
#define	DirectoryMgrTotalCount(dm)	((dm)->total_count)
#define	DirectoryMgrFilteredCount(dm)	((dm)->filtered_count)
#define	DirectoryMgrCurrentIndex(dm)	((dm)->current_index)

#ifndef __cplusplus

DirectoryMgr		*DirectoryMgrSimpleOpen();
int			DirectoryMgrSimpleRefilter();
int			DirectoryMgrSimpleResort();

DirectoryMgr		*DirectoryMgrOpen();
void			DirectoryMgrClose();
int			DirectoryMgrRefilter();
int			DirectoryMgrRefresh();
void			DirectoryMgrResort();

int			DirectoryMgrGotoItem();
int			DirectoryMgrGotoNamedItem();
void			DirectoryMgrRestart();
int			DirectoryMgrGetIndex();
DirEntry		*DirectoryMgrCurrentEntry();
DirEntry		*DirectoryMgrNextEntry();
DirEntry		*DirectoryMgrPrevEntry();

int			DirectoryMgrSimpleFilterFunc();
int			DirectoryMgrSimpleSortingFunc();

int			DirectoryMgrCompareName();
int			DirectoryMgrCompareNameDirsFirst();
int			DirectoryMgrCompareSizeAscending();
int			DirectoryMgrCompareSizeDescending();
int			DirectoryMgrCompareLastAccessAscending();
int			DirectoryMgrCompareLastAccessDescending();

int			DirectoryMgrFilterName();

#endif

#endif

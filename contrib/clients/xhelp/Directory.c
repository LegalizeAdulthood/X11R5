/****************************************************************************
 Copyright by Brian Totty, Free Widget Foundation.
 See MultiList.c for full copyright information.

        Directory.c

	This file contains the C code that implements the directory
	iteration and file information subsystem.

	This code is intended to be used as a convenient, machine
	independent interface to iterate through the contents of a
	directory.

 ****************************************************************************/

#include "Directory.h"
#include "RegExp.h"

/*--------------------------------------------------------------------------*

        L O W    L E V E L    D I R E C T O R Y    I N T E R F A C E

 *--------------------------------------------------------------------------*/

int DirectoryOpen(dir_name,dp)
char *dir_name;
Directory *dp;
{
	DirectoryDir(dp) = opendir(dir_name);
	if (DirectoryDir(dp) == NULL) return(FALSE);
	if (DirectoryPathExpand(dir_name,DirectoryPath(dp)) == NULL)
	{
		closedir(DirectoryDir(dp));
		return(FALSE);
	}
	return(TRUE);
} /* End DirectoryOpen */


void DirectoryRestart(dp)
Directory *dp;
{
	rewinddir(DirectoryDir(dp));
} /* End DirectoryRestart */


void DirectoryClose(dp)
Directory *dp;
{
	closedir(DirectoryDir(dp));
} /* End DirectoryClose */


long DirectoryTellPosition(dp)
Directory *dp;
{
	return(telldir(DirectoryDir(dp)));
} /* End DirectoryTellPosition */


void DirectorySetPosition(dp,pos)
Directory *dp;
long pos;
{
	seekdir(dp,pos);
} /* End DirectorySetPosition */


int DirectoryReadNextEntry(dp,de)
Directory *dp;
DirEntry *de;
{
	u_short orig_file_type;
	static struct dirent *_ep;
	static struct stat _lstats,_stats,_dstats;
	char full_path[MAXPATHLEN + 2];
	char temp_path[MAXPATHLEN + 2];

	_ep = readdir(DirectoryDir(dp));
	if (_ep == NULL) return(FALSE);
	strcpy(DirEntryFileName(de),_ep->d_name);
	strcpy(full_path,DirectoryPath(dp));
	strcat(full_path,DirEntryFileName(de));

	if (lstat(full_path,&_lstats) != 0) return(FALSE);

	orig_file_type = _lstats.st_mode & S_IFMT;
	switch (orig_file_type)
	{
	    case S_IFDIR:
		DirEntryType(de) = F_TYPE_DIR;
		break;
	    case S_IFREG:
		DirEntryType(de) = F_TYPE_FILE;
		break;
	    case S_IFCHR:
		DirEntryType(de) = F_TYPE_CHAR_SPECIAL;
		break;
	    case S_IFBLK:
		DirEntryType(de) = F_TYPE_BLOCK_SPECIAL;
		break;
	    case S_IFLNK:
		DirEntryType(de) = F_TYPE_SYM_LINK;
		break;
	    case S_IFSOCK:
		DirEntryType(de) = F_TYPE_SOCKET;
		break;
	    case S_IFIFO:
		DirEntryType(de) = F_TYPE_FIFO;
		break;
	    default:
		DirEntryType(de) = orig_file_type;
		break;
	}

	DirEntryIsBrokenLink(de) = FALSE;
	DirEntryIsDirectoryLink(de) = FALSE;
	if (DirEntryIsSymLink(de))			/* Symbolic Link */
	{
		if (stat(full_path,&_stats) != 0)	/* Can't Stat File */
		{
			DirEntryIsBrokenLink(de) = TRUE;
			_stats = _lstats;
		}
		    else				/* Link Not Broken */
		{
			if (DirectoryPathExpand(full_path,temp_path) != NULL)
				DirEntryIsDirectoryLink(de) = TRUE;
		}
	}
	    else					/* Not Symbolic Link */
	{
		_stats = _lstats;
	}

	FileInfoOrigMode(DirEntrySelfInfo(de)) = _lstats.st_mode;
	FileInfoProt(DirEntrySelfInfo(de)) = _lstats.st_mode & 0777;
	FileInfoUserID(DirEntrySelfInfo(de)) = _lstats.st_uid;
	FileInfoGroupID(DirEntrySelfInfo(de)) = _lstats.st_gid;
	FileInfoFileSize(DirEntrySelfInfo(de)) = _lstats.st_size;
	FileInfoLastAccess(DirEntrySelfInfo(de)) = _lstats.st_atime;
	FileInfoLastModify(DirEntrySelfInfo(de)) = _lstats.st_mtime;
	FileInfoLastStatusChange(DirEntrySelfInfo(de)) = _lstats.st_ctime;

	FileInfoOrigMode(DirEntryActualInfo(de)) = _stats.st_mode;
	FileInfoProt(DirEntryActualInfo(de)) = _stats.st_mode & 0777;
	FileInfoUserID(DirEntryActualInfo(de)) = _stats.st_uid;
	FileInfoGroupID(DirEntryActualInfo(de)) = _stats.st_gid;
	FileInfoFileSize(DirEntryActualInfo(de)) = _stats.st_size;
	FileInfoLastAccess(DirEntryActualInfo(de)) = _stats.st_atime;
	FileInfoLastModify(DirEntryActualInfo(de)) = _stats.st_mtime;
	FileInfoLastStatusChange(DirEntryActualInfo(de)) = _stats.st_ctime;

	return(TRUE);
} /* End DirectoryReadNextEntry */


char *DirectoryPathExpand(old_path,new_path)
char *old_path,*new_path;
{
	register char *p;
	char path[MAXPATHLEN + 2];

	if (getwd(path) == NULL) return(NULL);
	if (chdir(old_path) != 0) return(NULL);
	if (getwd(new_path) == NULL) return(NULL);
	if (chdir(path) != 0) return(NULL);
	for (p = new_path; *p != '\0'; p++);
	if ((p != new_path) && *(p - 1) != '/')
	{
		*p++ = '/';
		*p = '\0';
	}
	return(new_path);
} /* End DirectoryPathExpand */


/*---------------------------------------------------------------------------*

             D I R E C T O R Y    E N T R Y    R O U T I N E S

 *---------------------------------------------------------------------------*/

void DirEntryDump(fp,de)
FILE *fp;
DirEntry *de;
{
	fprintf(fp,"%20s, Size %7d, Prot %3o\n",
		DirEntryFileName(de),DirEntryFileSize(de),DirEntryProt(de));
} /* End DirEntryDump */

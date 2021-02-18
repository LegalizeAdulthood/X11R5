/*
 * Copyright (C) 1990 Regents of the University of California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of the University of
 * California not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  the University of California makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 */

static int c;

# if defined(sun)
# include <stdio.h>

# include <sys/file.h>
# include <sys/types.h>
# include <sys/param.h>
# include <sys/stat.h>

# include <sun/dkio.h>

# include <mntent.h>
# include <string.h>

# include <sys/buf.h>
# ifdef sun4c
# include <scsi/targets/srdef.h>
# else
# include <sundev/srreg.h>
# endif

# include <sys/time.h>

# include "debug.h"
# include "cdrom_sun.h"

# if defined(notdef)
extern void	cdrom_print_toc();
# endif /* defined(notdef) */

static char	cdrom[] =	"/dev/rsr0";

cdrom_info	cdi;

static int	cdrom_fd = -1;

int
cdrom_open() {
	int	n;

	if (cdrom_fd != -1)
		return(cdi.cdi_curtrack);

	if ((cdrom_fd = open(cdrom, O_RDONLY)) == -1) {
		fprintf(stderr, "open: ");
		perror(cdrom);
		return(-1);
	}

	if (cdrom_get_times() == -1)
		return(-1);

	if ((n = cdrom_get_curtrack()) == -1)
		return(-1);

	cdi.cdi_curtrack = n;

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE)
		shuffle_setup();

	return((cdi.cdi_curtrack == 0) ? 1 : cdi.cdi_curtrack);
}

void
cdrom_close() {
	if (cdrom_fd == -1)
		return;

	if (cdi.cdi_times != NULL) {
		free((char *) cdi.cdi_times);
		cdi.cdi_times = NULL;
	}

	(void) close(cdrom_fd);
	cdrom_fd = -1;
}

int
cdrom_get_times() {
	struct cdrom_tochdr	tochdr;
	extern unsigned short	*ushort_malloc();
	unsigned long		trk, trk_total, otime;

	if (cdrom_read_tochdr(&tochdr) == -1)
		return(-1);

	cdi.cdi_mintrack = tochdr.cdth_trk0;
	cdi.cdi_maxtrack = tochdr.cdth_trk1;

	cdi.cdi_times = ushort_malloc(cdi.cdi_maxtrack - cdi.cdi_mintrack + 1);

	otime = 0;

	for (trk = cdi.cdi_mintrack; trk <= cdi.cdi_maxtrack; trk++) {
		if (cdrom_get_msf(trk, &trk_total) == -1)
			return(-1);

		trk_total -= otime;
		otime += trk_total;

		if (trk != cdi.cdi_mintrack)
			cdi.cdi_times[trk - cdi.cdi_mintrack - 1] = trk_total;
	}

	if (cdrom_get_msf(CDROM_LEADOUT, &trk_total) == -1)
		return(-1);

	trk_total -= otime;
	otime += trk_total;

	cdi.cdi_times[trk - cdi.cdi_mintrack - 1] = trk_total;

	return(0);
}

# if defined(notdef)
static void
cdrom_print_toc() {
	unsigned long		trk, trk_total;

	for (trk = cdi.cdi_mintrack; trk <= cdi.cdi_maxtrack; trk++) {
		trk_total = cdi.cdi_times[trk - cdi.cdi_mintrack];
		debug_printf(1, "%02u:%02u\n", trk_total/60, trk_total%60);
	}
}
# endif /* defined(notdef) */

int
cdrom_get_curtrack() {
	struct cdrom_subchnl	subchnl;

	if (ioctl(cdrom_fd, CDROMSUBCHNL, (char *) &subchnl) == -1) {
		fprintf(stderr, "ioctl(cdromsubchnl): ");
		perror(cdrom);
		return(-1);
	}

	switch (subchnl.cdsc_audiostatus) {
		case CDROM_AUDIO_INVALID:
		return(-1);

		/* playing track subchnl.cdsc_trk */
		case CDROM_AUDIO_PLAY:
		return((int) subchnl.cdsc_trk);

		/* paused on track subchnl.cdsc_trk */
		case CDROM_AUDIO_PAUSED:
		return((int) subchnl.cdsc_trk);

		/* punt */
		case CDROM_AUDIO_COMPLETED:
		return(0);

		case CDROM_AUDIO_ERROR:
		return(-1);

		/* punt */
		case CDROM_AUDIO_NO_STATUS:
		debug_printf(1, "cdrom_get_curtrack: no status\n");
		return(0);
	}

	/* bad value in cdsc_audiostatus */
	return(-1);
}

int
cdrom_play_track(start_track, end_track)
	unsigned char		start_track;
	unsigned char		end_track;
{
	struct	cdrom_ti	ti;

	ti.cdti_trk0 = start_track;
	ti.cdti_ind0 = 1;
	ti.cdti_trk1 = end_track;
	ti.cdti_ind1 = 1;

	if (ioctl(cdrom_fd, CDROMPLAYTRKIND, &ti) == -1) {
		perror("ioctl(cdromplaytrkind)");
		return(-1);
	}

	return(0);
}

int
cdrom_start() {
	if (ioctl(cdrom_fd, CDROMSTART) == -1) {
		perror("ioctl(cdromstart)");
		return(-1);
	}

	return(0);
}

int
cdrom_stop() {
	if (ioctl(cdrom_fd, CDROMSTOP) == -1) {
		perror("ioctl(cdromstop)");
		return(-1);
	}

	return(0);
}

int
cdrom_eject() {
	if (ioctl(cdrom_fd, CDROMEJECT) == -1) {
		perror("ioctl(cdromeject)");
		return(-1);
	}

	return(0);
}

int
cdrom_read_tocentry(track, tocentry)
	unsigned int		track;
	struct cdrom_tocentry	*tocentry;
{
	tocentry->cdte_track = track;
	tocentry->cdte_format = CDROM_MSF;

	if (ioctl(cdrom_fd, CDROMREADTOCENTRY, (char *) tocentry) == -1) {
		perror("ioctl(cdromreadtocentry)");
		return(-1);
	}

	return(0);
}

int
cdrom_read_tochdr(tochdr)
	struct cdrom_tochdr	*tochdr;
{
	if (ioctl(cdrom_fd, CDROMREADTOCHDR, (char *) tochdr) == -1) {
		perror("ioctl(cdromreadtochdr)");
		return(-1);
	}

	return(0);
}

int
cdrom_pause() {
	if (ioctl(cdrom_fd, CDROMPAUSE) == -1) {
		perror("ioctl(cdrompause)");
		return(-1);
	}

	return(0);
}

int
cdrom_resume() {
	if (ioctl(cdrom_fd, CDROMRESUME) == -1) {
		perror("ioctl(cdromresume)");
		return(-1);
	}

	return(0);
}

int
cdrom_volume(left_vol, right_vol)
	int			left_vol;
	int			right_vol;
{
	struct cdrom_volctrl	vol;

	vol.channel0 = left_vol;
	vol.channel1 = right_vol;

	if (ioctl(cdrom_fd, CDROMVOLCTRL, &vol) == -1) {
		perror("ioctl(cdromvolctrl)");
		return(-1);
	}

	return(0);
}

int
cdrom_get_msf(track, msf)
	unsigned long		track;
	unsigned long		*msf;
{
	struct cdrom_tocentry	tocentry;

	if (cdrom_read_tocentry(track, &tocentry) == -1)
		return(-1);

	*msf = ((int) tocentry.cdte_addr.msf.minute * 60) +
		(int) tocentry.cdte_addr.msf.second;

	return(0);
}

# if defined(notused)
unsigned long
cdrom_get_relmsf() {
	struct cdrom_subchnl	subchnl;
	unsigned long		tmp;

	subchnl.cdsc_format = CDROM_MSF;

	if (ioctl(cdrom_fd, CDROMSUBCHNL, (char *) &subchnl) == -1) {
		perror("ioctl(cdromsubchnl)");
		return(-1);
	}

	tmp = ((int) subchnl.cdsc_reladdr.msf.minute * 60) +
		(int) subchnl.cdsc_reladdr.msf.second;

	return(tmp);
}
# endif /* defined(notused) */

int
cdrom_status() {
	struct cdrom_subchnl	subchnl;

	if (ioctl(cdrom_fd, CDROMSUBCHNL, (char *) &subchnl) == -1) {
		fprintf(stderr, "ioctl(cdromsubchnl): ");
		perror(cdrom);
		exit(1);
	}

	switch (subchnl.cdsc_audiostatus) {
		case CDROM_AUDIO_INVALID:
		return(CDROM_INVALID);

		case CDROM_AUDIO_PLAY:
		return(CDROM_PLAYING);

		case CDROM_AUDIO_PAUSED:
		return(CDROM_PAUSED);

		case CDROM_AUDIO_COMPLETED:
		return(CDROM_COMPLETED);

		case CDROM_AUDIO_ERROR:
		return(CDROM_ERROR);

		case CDROM_AUDIO_NO_STATUS:
		return(CDROM_NO_STATUS);
	}

	return(-1);
}

# if defined(notused)
int
cdrom_playing(track)
	int			*track;
{
	struct cdrom_subchnl	sc;

	sc.cdsc_format = CDROM_MSF;
	if (ioctl(cdrom_fd, CDROMSUBCHNL, &sc) == -1) {
		perror("ioctl(cdromsubchnl)");
		return(-1);
	}

	*track = sc.cdsc_trk;

	if (sc.cdsc_audiostatus == CDROM_AUDIO_PLAY)
		return(1);

	return(0);
}
# endif /* defined(notused) */

# if defined(notused)
int
cdrom_paused(track)
	int			*track;
{
	struct cdrom_subchnl	sc;

	sc.cdsc_format = CDROM_MSF;
	if (ioctl(cdrom_fd, CDROMSUBCHNL, &sc) == -1) {
		perror("ioctl(cdromsubchnl)");
		return(-1);
	}

	*track = sc.cdsc_trk;

	if (sc.cdsc_audiostatus == CDROM_AUDIO_PAUSED)
		return(1);

	return(0);
}
# endif /* defined(notused) */

# if defined(notused)
int
mounted(name)
	char		*name;
{
	char		buf[MAXPATHLEN], *cp;
	struct stat	st;
	dev_t		bdevno;
	FILE		*fp;
	struct mntent	*mnt;

	/*
	 * Get the block device corresponding to the raw device opened,
	 * and find its device number.
	 */
	if (stat(name, &st) != 0) {
		(void) fprintf(stderr, "stat: ");
		perror(name);
		return(UNMOUNTED);
	}

	/*
	 * If this is a raw device, we have to build the block device name.
	 */
	if ((st.st_mode & S_IFMT) == S_IFCHR) {
		if ((cp = strchr(name, 'r')) != NULL)
			cp++;

		(void) sprintf(buf, "/dev/%s", cp);
		if (stat(buf, &st) != 0) {
			(void) fprintf(stderr, "stat: ");
			perror(buf);
			return(UNMOUNTED);
		}
	}

	if ((st.st_mode & S_IFMT) != S_IFBLK)
		return(UNMOUNTED);

	bdevno = st.st_rdev & (dev_t)(~0x07);	/* Mask out partition. */

	/*
	 * Now go through the mtab, looking at all hsfs filesystems.
	 * Compare the device mounted with our bdevno.
	 */
	if ((fp = setmntent(MOUNTED, "r")) == NULL) {
		(void) fprintf(stderr, "couldn't open %s\n", MOUNTED);
		return(UNMOUNTED);
	}

	while ((mnt = getmntent(fp)) != NULL) {
		/* avoid obvious excess stat(2)'s */
		if (strcmp(mnt->mnt_type, "hsfs") != 0)
			continue;

		if (stat(mnt->mnt_fsname, &st) != 0)
			continue;

		if (((st.st_mode & S_IFMT) == S_IFBLK) &&
		    ((st.st_rdev & (dev_t)(~0x07)) == bdevno)) {
			(void) endmntent(fp);
			return(STILL_MOUNTED);
		}
	}

	(void) endmntent(fp);

	return(UNMOUNTED);
}
# endif /* defined(notused) */

unsigned short *
ushort_malloc(n)
	int		n;
{
	extern char	*calloc();
	unsigned short	*ptr;

	ptr = (unsigned short *) calloc(n, sizeof(unsigned short));
	if (ptr == NULL) {
		perror("calloc");
		exit(1);
	}

	return(ptr);
}
# endif /* defined(sun) */

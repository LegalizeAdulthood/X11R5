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

# define bit(n)			(1 << (n))

/* bits for cdrom_state */
# define CDROM_STATE_PLAY	bit(0)
# define CDROM_STATE_PAUSE	bit(1)
# define CDROM_STATE_EJECTED	bit(2)
# define CDROM_STATE_CYCLE	bit(3)
# define CDROM_STATE_SHUFFLE	bit(4)

/* return codes from cdrom_status() */
# define CDROM_INVALID		1
# define CDROM_PLAYING		2
# define CDROM_PAUSED		3
# define CDROM_COMPLETED	4
# define CDROM_ERROR		5
# define CDROM_NO_STATUS	6

#define	STILL_MOUNTED	1
#define	UNMOUNTED	0

typedef struct _cdrom_info {
	unsigned char	cdi_curtrack;	/* current track playing	*/
	unsigned char	cdi_mintrack;	/* first audio track		*/
	unsigned char	cdi_maxtrack;	/* last audio track		*/
	unsigned char	cdi_ntracks;	/* size of random track list	*/
	unsigned short	*cdi_times;	/* duration of each track	*/
	unsigned int	cdi_dur;	/* seconds played so far	*/
	int		cdi_state;	/* state of cd-rom drive	*/
	short		cdi_currand;	/* index into random track list */
} cdrom_info;

extern void		cdrom_close();
extern int		cdrom_open();
extern int		cdrom_start();
extern int		cdrom_stop();
extern int		cdrom_eject();
extern int		cdrom_play_track();
extern int		cdrom_read_tocentry();
extern int		cdrom_read_tochdr();
extern int		cdrom_lock_unlock();
extern int		cdrom_pause();
extern int		cdrom_resume();
extern int		cdrom_volume();
extern int		cdrom_play();
extern int		cdrom_paused();
extern int		cdrom_status();
extern int		cdrom_get_curtrack();
extern int		cdrom_get_msf();
extern int		cdrom_get_times();
extern unsigned long	cdrom_get_relmsf();

extern cdrom_info	cdi;

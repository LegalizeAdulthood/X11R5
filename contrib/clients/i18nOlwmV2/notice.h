/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) notice.h 50.4 90/12/12 Crucible */

typedef struct _noticeBox {
	int	numButtons;	/* number of buttons */
	int	defaultButton;	/* index into buttonText array */
#ifdef OW_I18N
	wchar_t	**buttonText;	/* array of strings for button text */
#else
	char	**buttonText;	/* array of strings for button text */
#endif
#ifdef OW_I18N
	wchar_t	*stringText;	/* strings for description (contains \n) */
#else
	int	numStrings;	/* number of descriptive strings */
	char	**stringText;	/* array of strings for description */
#endif
	int	boxX;		/* box origin (-1 =use default/centered) */
	int	boxY;		/* box origin (-1 =use default/centered) */
} NoticeBox;

/* function declarations */
extern int UseNoticeBox();

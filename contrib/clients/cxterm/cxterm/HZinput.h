/* $Id: HZinput.h,v 1.1 1991/10/01 04:54:26 ygz Exp $ */

/***********************************************************************
* Copyright 1990, 1991 by Purdue University, and Yongguang Zhang.
* All rights reserved.  Some individual files may be covered by other
* copyrights.
*
* Redistribution and use in source and binary forms are permitted
* provided that this entire copyright notice is duplicated in all such
* copies.  No charge, other than an "at-cost" distribution fee, may be
* charged for copies, derivations, or distributions of this material
* without the express written consent of the copyright holders.  Neither
* the name of the University, nor the name of the authors may be used to
* endorse or promote products derived from this material without
* specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
* MERCHANTIBILITY AND FITNESS FOR ANY PARTICULAR PURPOSE.
************************************************************************/

/* hzinput.h */

/*
 * Data Structure of the Chinese input lookup table -- TRIE model
 *
 * Trie Table:
 *	list of Trie nodes.
 *
 * HZ Table:
 *	2 bytes character sets.
 */

#ifndef	_HZINPUT_H_
#define	_HZINPUT_H_

#define	MAX_HZTBL	24	/* maxinum number of input table allowed */
#define	MAX_PROMPT	24	/* maximum prompt in HZ input erea */
#define	MAX_INBUF	16	/* maximum length of input conversion buffer */
#define MAX_KEYPROMPT	4	/* maximum bytes to display each input key */
#define MAX_CHOICE	16	/* maximum number of choices on one screen */

typedef	struct _keyPrompt {
    unsigned char	prompt[MAX_KEYPROMPT];	/* prompt string */
    unsigned short	ptlen;			/* len of the prompt */
} keyPrompt;

/* So organized to optimize access speed */
typedef	struct _trieNode {
    unsigned char	tn_key;		/* this input key */
    unsigned char	tn_numNextKeys;	/* number of possible next input key */
    unsigned short int	tn_numHZchoice;	/* number of HZ choices */
    unsigned int	tn_nextKeys;	/* table for further input key */
    unsigned int	tn_hzidx;	/* index to HZ list */
} trieNode;

typedef struct _HZinputTable {
    unsigned char	version;	/* version num of this .cit format */
    unsigned char	encode;		/* Chinese encoding: GB or BIG5 */
    unsigned char	builtin;	/* this table is builtin? */

    unsigned int	sizeTrieList;	/* size of the whole TRIE list */
    unsigned int	sizeHZList;	/* size of the whole HZ list */
    trieNode		*trieList;	/* start address of the Trie list */
    XChar2b		*hzList;	/* start address of the HZ list */

    unsigned char	multiChoice;	/* need multiple choices? */
    unsigned short	keytype[128];	/* valid type of the key */
    keyPrompt		keyprompt[128];	/* display this for the key */

    char	choicelb[MAX_CHOICE];	/* HZ choice label */
    int		maxchoice;		/* maximum number of HZ choice */
    char	prompt[MAX_PROMPT];
    int		lenPrompt;		/* len of the Prompt */

} HZinputTable;

/*
 * valid type or type mask of a key
 * mask can be OR -- possible for a key to be both input key and selection key
 *
 *	........
 *	   1#### <--- SELECTION_MASK
 *	  1      <--- INPUT_MASK
 *	01     0 <--- BACKSPACE
 *	01     1 <--- KILL
 *	10     0 <--- RIGHT
 *	10     1 <--- LEFT
 *	11     0 <--- REPEAT
 *
 * HZ_KEY_SELECTION_NUM must be (MAX_CHOICE - 1)
 */
#define	HZ_KEY_INVALID		0	/* not used to input HZ */
#define	HZ_KEY_SELECTION_MASK	0x10	/* used to select a choice */
#define	HZ_KEY_SELECTION_NUM	0x0f	/* num to select a choice */
#define	HZ_KEY_INPUT_MASK	0x20	/* used to convert to HZ */
#define HZ_KEY_BACKSPACE	0x40	/* edit key: backspace */
#define HZ_KEY_KILL		0x41	/* edit key: clear input buffer */
#define HZ_KEY_RIGHT		0x80	/* select key: more at right */
#define HZ_KEY_LEFT		0x81	/* select key: more at left */
#define HZ_KEY_REPEAT		0xc0	/* repeat key: repeat previous input */

/*
 * Magic number and version number of this .cit file.
 */
#define	MAGIC_CIT	"HZ"
#define	CIT_VERSION	1
#define	GB_ENCODE	0	/* must agree with HZ_GB in ptyx.h */
#define	BIG5_ENCODE	1	/* must agree with HZ_BIG5 in ptyx.h */
#define	CIT_SUFFIX	".cit"
#define	TIT_SUFFIX	".tit"

#endif /* _HZINPUT_H_ */

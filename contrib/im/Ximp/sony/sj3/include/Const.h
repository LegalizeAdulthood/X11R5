/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#define	HostNameLen		128
#define	UserNameLen		128
#define	ProgNameLen		128
#define	PathNameLen		1024
#define	PassWordLen		16

#define	YomiBufSize		1024
#define	KanjiBufSize		2048

#define	MinIdxLen		512
#define	MinSegLen		512
#define	MinSegNum		1
#define	MinStyNum		512
#define	MaxStyNum		0x7fff
#define	MinClStep		1
#define	MaxClStep		256
#define	MinClLen		512
#define	MaxClLen		0x7fff

#define	RunCmdFile		"/usr/sony/lib/sj3/serverrc"
#define	DebugOutFile		NULL
#define	DebugLevel		0
#define	ForkFlag		1
#define	MaxClientNum		64
#define	ErrorOutFile		"/dev/console"
#define	DictRootDir		"/usr/sony/dict/sj3"
#define	LogOutFile		NULL
#define	PortName		"sj3"
#define	PortNumber		3000
#define	SocketName		"/tmp/sj3sock"
#define	LockFile		"/tmp/SJ3SERV.EXIST"
#define	DirectryMode		0755
#define	DictFileMode		0644
#define	MODE_MASK		0777

#define	MainDictionary		"sj3main.dic"
#define	UserDictionary		"private.dic"
#define	StudyFile		"study.dat"
#define	DefIdxLen		2048
#define	DefSegLen		2048
#define	DefSegNum		256
#define	DefStyNum		2048
#define	DefClStep		1
#define	DefClLen		2048

#define	NotLocked		-1

#define	ERROR			-1

#ifndef	FALSE
#	define	TRUE		-1
#	define	FALSE		(!TRUE)
#endif

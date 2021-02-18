
/**/#!/bin/sh
/**/# Copyright 1991 NEC Corporation, Tokyo, Japan.
/**/#
/**/# Permission to use, copy, modify, and distribute this software and its
/**/# documentation for any purpose and without fee is hereby granted,
/**/# provided that the above copyright notice appear in all copies and that
/**/# both that copyright notice and this permission notice appear in
/**/# supporting documentation, and that the name of NEC Corporation
/**/# not be used in advertising or publicity pertaining to distribution
/**/# of the software without specific, written prior permission.  NEC 
/**/# Corporation makes no representations about the suitability of this
/**/# software for any purpose.  It is provided "as is" without express
/**/# or implied warranty.
/**/#
/**/# NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
/**/# INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
/**/# NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
/**/# CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
/**/# USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
/**/# OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
/**/# PERFORMANCE OF THIS SOFTWARE. 

/**/#ident  "@(#) NEC/V(386) PC-UX/V R3.0B irohakill 1.3 90/04/25 10:33:43"

#include "config.h"

#ifndef LOCKFILEPATH
#define LOCKFILEPATH /usr/spool/iroha/lock/.IROHALOCK
#endif

IROHALOCKFILE=LOCKFILEPATH

if [ $# -gt 1 ]
then
    echo 'Usage: irohakill [ServerNumber|-all|-help]'
    exit 2
fi

if [ -f $IROHALOCKFILE* ]
then
    if [ $1 ]
    then
	case $1 in
	       -h*) echo 'irohakill [ServerNumber|-all|-help]' ; exit 0 ;;
	       -a*) for i in `cat $IROHALOCKFILE*`
		    do
			kill $i
		    done; exit ;;
		 0) if [ -f $IROHALOCKFILE ]			
		    then					
			kill `cat $IROHALOCKFILE`	
		    else					
			echo 'Error: irohaserver is not running.'
		    fi; exit ;; 			
		 *) if [ -f $IROHALOCKFILE:$1 ] 		
		    then					
			kill `cat $IROHALOCKFILE:$1`		
		    else					
			echo 'Error: irohaserver('$1') is not running.'
		    fi; exit ;; 				
	esac
    else
	if [ -f $IROHALOCKFILE ]
	then
	    kill `cat $IROHALOCKFILE`
	else
	    echo 'Error: irohaserver is not running.'
	fi
    fi
else
	echo 'Error: irohaserver is not running.'
fi

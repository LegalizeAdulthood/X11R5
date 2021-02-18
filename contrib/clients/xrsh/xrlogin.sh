#!/bin/sh
# Some System V systems don't understand the #! construct.  
#       In this case replace the first line with just a colon (:)
#
# Copyright 1991 by James J. Dempsey <jjd@bbn.com>
# 
# Permission to use, copy, modify, distribute, and sell this software and its
# documentation for any purpose is hereby granted without fee, provided that
# the above copyright notice appear in all copies and that both that
# copyright notice and this permission notice appear in supporting
# documentation, James J. Dempsey makes no representations about the
# suitability of this software for any purpose.  It is provided "as is"
# without express or implied warranty.
#
# Starts a local xterm running rlogin or telnet
# Written by Stephen Gildea <gildea@expo.lcs.mit.edu> 
#     and Jim Dempsey <jjd@bbn.com>
#
# Usage: xrlogin [-l username] [-telnet] hostname

# Set some defaults.
progname=`basename $0`
usage="usage: $progname [-l logname] [-telnet] host"
netprog="rlogin"
netprogopts="-8"
termprog=xterm
termprogopts=
telnet=
user=
host=

# process arguments
while [ "$*" != "" ]; do
    case "$1" in 
      -telnet)
          telnet=t;
          ;;
      -l)
          if [ "$telnet" ]; then
              echo "$progname: Can't use -l with -telnet" 1>&2
              echo $usage 1>&2
              exit 1
          fi
          user=$2; shift;
          ;;
      *)
          if [ "$host" ]; then
              echo "$progname: More than one host supplied: $host and $1" 1>&2
              echo $usage 1>&2
              exit 1
          fi
          host=$1
          ;;
    esac
    shift
done

if [ "$user" -a "$telnet" ]; then
    echo "$progname: Can't use -l with -telnet" 1>&2
    echo $usage 1>&2
    exit 1
fi

if [ "$telnet" ]; then
    netprog=telnet;
    netprogopts= ;
fi
if [ "$user" ]; then
    netprogopts="$netprogopts -l $user"
fi

if [ ! "$host" ]; then
    echo "$progname: No host supplied" 1>&2
    echo $usage 1>&2
    exit 1
fi

if [ "$XRLOGIN_TERM_EMULATOR" != "" ]; then
    termprog=$XRLOGIN_TERM_EMULATOR
else
    case "$TERM" in
      *term)
	# TERM looks like it is an X11 terminal emulator, so use it.
	# Should also look for *terminalEmulatorName resource.
        termprog=$TERM;;
    esac
fi

if [ "$termprog" = "xterm" ]; then
    termprogopts=-ut
fi

# Remove domain part from hostname.
name=`echo $host | sed -e 's/\..*$//'`

# netprogopts are required to go after hostname on some SYS V hosts
exec $termprog $termprogopts -name $termprog-$name -title $name -e $netprog $host $netprogopts

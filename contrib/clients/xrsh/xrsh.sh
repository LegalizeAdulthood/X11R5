#!/bin/sh

# Some System V systems don't understand the #! construct.  
#   If your system does understand it, put ": " at the beginning of the line.
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
#
# xrsh:  start an X11 client on another host
#
# Usage: xrsh [-l username] [-auth authtype] [-screen #] [-pass envlist] \
#	[-debug] [-debug2] hostname [X-command [arguments...]]
#
# Example usage: xrsh expo.lcs.mit.edu xterm
#
# $Header$ xrsh version 5.1
#
# Runs the given command on another host with $DISPLAY set in the environment.
#    Handles authentication using xhost, xauth, or $XAUTHORITY (or none).
#    Xrsh is very careful not to leave any extra processes waiting
#       around on either machine for the client to exit.  In order
#       to achieve this, csh is used on the remote host and all inputs
#       and outputs are redirected to /dev/null.
#    The -debug switch allows you to see errors, but doesn't worry about
#       leaving extra processes around.
#    The -debug2 switch sets -x so that you can see every command
#       executed by the script  (very verbose)
#    The -l switch allows you to use a different login name on the
#       remote host.  -l and its argument are passed directly to rsh.
#    The -help switch prints out the usage message.
#    The -screen switch allows you to start a client on a different screen.
#    If no X command is given, it defaults to xterm.  This means that
#         'xrsh remotehost' will start an xterm on remotehost.
#    If the given command is a path that ends in "xterm", it adds the 
#        arguments "-name xterm-$hostname" where $hostname is the name
#        of the remote host.  This allows you to customize your server's
#        xrdb database to set attributes, such as the window color, based 
#        on the name of remote host.  If you dislike this behaviour, specify
#        your own -name argument to xterm on the xrsh command line.
#
#    Xrsh handles authentication based on the value of $XRSH_AUTH_TYPE or
#        the value of the -auth command line switch.  The value can
#        be xhost, xauth, environment or none.  It defaults to xhost.
#             xhost -- run xhost to enable the remote host to open window
#                      run xhost on the host where the server is running
#             xhost-xterminal -- 
#                      run xhost to enable the remote host to open windows
#                      run xhost on the host where xrsh was first run
#             xauth -- merge the auth entry from this host on the remote
#                        host using xauth via rsh
#             environment -- pass the $XAUTHORITY environment variable from the
#                       local host to the remote host so that a common
#                       NFS accessable authority file can be used everywhere
#             none -- do nothing.  Assume the user has handled authentication.
#
#    If the environment variable XRSH_RSH_ERRORS is set to the name of a file, 
#        any rsh errors will appear in that file on the remote host.
#        If that variable is unset, error messages will be thrown away unless
#        the -debug switch is given. (Note: don't use ~ in the filename
#        because it will expand to ~ on the local host, but try to put the
#        errors in that file on the remote host.)    
#
#
#  COMMON PROBLEMS:
#     * Make sure your PATH environment variable on the remote host is
#           set in your .cshrc or .bashrc so that rsh programs have
#           access to it.  (/bin/sh and /bin/ksh users have a hard time
#           time here since their shells don't execute any init files
#           under rsh.  You will have to type a full path to xrsh in that case.
#           E.g.  xrsh remote-host /usr/bin/X11/xterm
#     * Make sure your PATH environment variable on the remote host
#           includes the directory containing the X programs.  This is 
#           often /usr/bin/X11 or /usr/local/bin/X11.
#     * Make sure you have rsh configured to work on the remote host.
#           You can test this by typing:  rsh remote-host echo '$PATH'
#           This will prove that rsh works and show you the PATH that
#           will be used on the remote host.  If you get "Permission 
#           denied." you probably need to update your ~/.rhosts file
#           on the remote host.  See rlogin(1).
#
# Until X11R5, this command used to be called "xon".  The name was
# changed in order to be more descriptive, to have a name that better
# matches its cousin (xrlogin) and to not collide with a similar command
# written by Keith Packard distributed with X11R5.
#
# Written by Jim Dempsey <jjd@bbn.com> with help from Stephen Gildea
#   <gildea@bbn.com> based on an idea by Dave Mankins <dm@think.com>.
# Some additional features due to dgreen@cs.ucla.edu,
#   "David B. Rosen" <rosen@park.bu.edu>, Eero Simoncelli 
#    <eero@whitechapel.media.mit.edu> and  Martin Friedmann 
#    <martin@whitechapel.media.mit.edu>
#   Originally written around 1987.
#   Last modification October 1991 by jjd@bbn.com.
#   Version 5.1 of xrsh is distributed with X11R5.


# initialize some variables
authenv=
foundarg=
progname=`basename $0`
rshoptions=""
passenvs=                # environment variables to pass along
xhostvar=                # used by auth type xhost-xterminal
usage="usage: $progname [-l username] [-auth authtype] [-screen #] [-debug] [-debug2] host [cmd [args ...]]"

if [ -z "$DISPLAY" ]; then
    echo "$progname: "'$DISPLAY must be set' 1>&2 ;
    exit 1
fi

if [ -z "$XRSH_RSH_ERRORS" ]; then
    XRSH_RSH_ERRORS=/dev/null
fi

# process command line arguments
until [ "$foundarg" = "no" ]
do
    foundarg=no
    case $1 in
        -debug)
            debug=t; foundarg=yes; shift;
            ;;
        -debug2)
            set -x; foundarg=yes; shift; 
            ;;
	-help)
            echo $usage; exit 0;
            ;;
        -l)
            shift; rshoptions="-l $1"; foundarg=yes; shift;
            ;;
	-auth)
            shift; XRSH_AUTH_TYPE=$1; foundarg=yes; shift;
            ;;
	-pass)
            shift; XRSH_ENVS_TO_PASS=$1; foundarg=yes; shift;
            ;;
        -screen)
            shift; 
            # this line was blatantly stolen from Keith Packard's xon
            DISPLAY=`echo $DISPLAY|sed 's/:\\([0-9][0-9]*\\)\\.[0-9]/:\1/'`.$1
            foundarg=yes;
            shift;
            ;;
    esac
done

if [ "$#" = "0" ]; then
    echo $usage 1>&2; exit 1;
fi

clienthost="$1"; shift      # The full remote host name (as full as possible)

command=$1
if [ -z "$command" ]; then  # default command to xterm if none specified
    command=xterm
else
    shift
fi

# Grab the arguments to the command here so that we don't have to worry
# about restoring them when doing IFS hacking below
xcmdargs=$@

# Try to avoid running hostname.  
# Some shells set $HOST or $HOSTNAME automatically.
localhost=${HOST-${HOSTNAME-`hostname`}}           # The local host name

# Remove domain part from hostname.

# The following paragraph of IFS based code replaces sed and 
#    runs faster because it doesn't fork.
#       clientshort is remote host name without domain
#clientshort=`echo $clienthost | sed -e 's/\..*$//'` 
oldIFS=$IFS
IFS=.
set -- $clienthost
clientshort=$1
IFS=$oldIFS

# Find display host

# The following paragraph of IFS based code replaces sed and 
#    runs faster because it doesn't fork.
#displayhost=`echo $DISPLAY | sed 's/:.*//'`
oldIFS=$IFS
IFS=:
set -- $DISPLAY
case "$#" in
    1) displayhost=$localhost;;
    *) displayhost=$1;;
esac
IFS=$oldIFS

case "$displayhost" in
    "unix"|"local"|"")
        displayhost=$localhost;;
esac

# People use the -name resource to specify host-specific resources
# such as window color.
if [ `basename "$command"` = "xterm" ]; then
    command="$command -name xterm-$clientshort -title 'xterm@$clientshort'"
fi
if [ $# -ge 1 ]; then
    command="$command $xcmdargs"
fi

# Some System V hosts have rsh as "restricted shell" and ucb rsh is remsh
if [ -r /usr/bin/remsh ]; then
    rsh=remsh
elif [ -f /usr/bin/rcmd ]; then       # SCO Unix uses "rcmd" instead of rsh
    rsh=rcmd
else
    rsh=rsh
fi

# Construct the new $DISPLAY for the remote client

# The following paragraph of IFS based code replaces sed and 
#    runs faster because it doesn't fork.  It also handles the arp stuff.
# newdisplay="`echo $DISPLAY | sed \"s/^[^:]*:/${localhost}:/\"`"
oldIFS=$IFS
IFS=:
set -- $DISPLAY
if [ $# = 2 ]; then 
    shift
fi
case "$clienthost" in
    # If the remote host is the localhost, then don't put the hostname
    #  in $DISPLAY and let X find the fastest display path
    "$localhost")
        newdisplay=:$@
        ;;
    # if a full internet domain name is used, set $DISPLAY
    # using IP address so remote host will be guaranteed return path
    *.*)
        if [ -f /usr/etc/arp ]; then
            arp=/usr/etc/arp
        elif [ -f /etc/arp ] ; then
            arp=/etc/arp
        else
            arp=arp
        fi
        address=`$arp $displayhost |tr ')' '(' | awk  -F'(' '{print $2}'`
        if [ "$address" ]; then
            newdisplay="$address:$@"
        else
            newdisplay="${displayhost}:$@"
        fi
        ;; 
    # A host in the local domain
    *)
        newdisplay="${displayhost}:$@"
        ;;
esac
IFS=$oldIFS

# Use $XRSH_AUTH_TYPE to determine whether to run xhost, xauth, 
# propagate $XAUTHORITY to the remote host, or do nothing
case ${XRSH_AUTH_TYPE-xhost} in
  xhost)      
      if [ "$newdisplay" != "$DISPLAY" ]; then
      # If run on the same host as server, allow remote host access X server.
          if [ "$debug" ]; then
              xhost +$clienthost
          else
              xhost +$clienthost > /dev/null 2>&1
          fi
      else
          # If run on a different host as the server, use rsh to allow
          #    access to the host in DISPLAY.
          xhostcmd="setenv DISPLAY $DISPLAY; exec xhost +$clienthost"

          if [ "$debug" ]; then
              echo "Executing \"xhost +$clienthost\" on $displayhost with DISPLAY=$DISPLAY"
              $rsh "$displayhost"  exec /bin/csh -cf "\"$xhostcmd\"" < /dev/null
          else
              $rsh "$displayhost"  exec /bin/csh -cf "\"$xhostcmd < /dev/null >& /dev/null\""
          fi
      fi
      ;;
  xhost-xterminal)
      # If run on an X terminal, rsh to the XDMCP host to run xhost
      # if this is the first time, we are on the XDMCP host and 
      # we can just run xhost.
      if [ "$XHOST" = "" ]; then
          xhostvar="setenv XHOST $localhost; "
          if [ "$debug" ]; then
              xhost +$clienthost
          else
              xhost +$clienthost > /dev/null 2>&1 
          fi
      else
          xhostvar="setenv XHOST $XHOST; "
          xhostcmd="setenv DISPLAY $DISPLAY; exec xhost +$clienthost"
          if [ "$debug" ]; then
              echo "Executing \"xhost +$clienthost\" on $XHOST with DISPLAY=$DISPLAY"
              $rsh "$XHOST"  exec /bin/csh -cf "\"$xhostcmd\"" < /dev/null
          else
              $rsh "$XHOST"  exec /bin/csh -cf "\"$xhostcmd < /dev/null >& /dev/null\""
          fi
      fi
      ;;
  xauth)
      # the "xauth remove" is theoretically unnecessary below,
      # but due to a bug in the initial X11R5 (through at least fix-03) xauth,
      # entries for MIT-MAGIC-COOKIE-1 get lost if
      # you do merge twice without the remove.  The remove
      # command can be removed when xauth gets fixed.
      if [ "$debug" ]; then
          xauth extract - $newdisplay | \
          $rsh $clienthost xauth remove $newdisplay \;xauth merge -
      else
          xauth extract - $newdisplay | \
          $rsh $clienthost xauth remove $newdisplay \;xauth merge - >/dev/null 2>&1
      fi
      ;;
  none)
      ;;
  environment)
      authenv="setenv XAUTHORITY $XAUTHORITY; "
      ;;
  *)
      echo -n "$progname: "'$XRSH_AUTH_TYPE must be set to "xhost", ' 1>&2
      echo '"xauth", "environment" or left unset' 1>&2
      exit 1
      ;;
esac

for variable in $XRSH_ENVS_TO_PASS
do
    if eval [ \""\${$variable-Xyzzy}"\" != "Xyzzy" ]; then
        eval passenvs="\"setenv $variable '\$$variable'; $passenvs"\"
    fi
done


# Do the real work using rsh.
# Don't use -n to rsh because SG IRIX doesn't support it.  
# Use < /dev/null instead.
if [ "$debug" ]; then
    echo "Executing \"$command\" on $clienthost with DISPLAY=$newdisplay"
    $rsh $clienthost $rshoptions /bin/csh -cf "\"setenv DISPLAY $newdisplay; $authenv $xhostvar $passenvs $command\"" < /dev/null
else
#
# The redirection inside the innermost quotes is done by csh.  The outer
# redirection is done by whatever shell the user uses on the remote end.
# The inner redirection is necessary for an error log -- the outer 
# redirection is necessary to make sure that the local rsh doesn't hang around.
#
# The outermost < /dev/null is to simulate the -n argument to rsh which 
# we don't want to use for portability reasons.
#
# The three sets of redirections are for the local shell, the login shell
# on the remote host and the csh on the remote host.
#
    exec $rsh  $clienthost $rshoptions exec /bin/csh -cf "\"setenv DISPLAY $newdisplay; $authenv $xhostvar $passenvs exec $command < /dev/null >>& $XRSH_RSH_ERRORS \" < /dev/null > /dev/null" < /dev/null
fi




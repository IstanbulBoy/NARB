#!/bin/sh
#

PREFIX=/usr/local/dragon

# This script must be run as root.
WHOAMI=`whoami`
if test "$WHOAMI" != "root"; then
  echo '' && \
   echo "You must be root to run this script." && \
   echo ''
  exit 1
fi  

case `uname` in
  *BSD)
    echo This is BSD Unix

    echo '' && \
        echo "installing rce..."
    cd rce
    make install
    if test $? != 0; then
	echo "narb-sw-installer: rce make install error!"
	exit 1
    fi

    echo '' && \
        echo "installing narb..."
    cd ../narb
    make install
    if test $? != 0; then
	echo "narb-sw-installer: narb make install error!"
	exit 1
    fi

    echo '' && \
     echo "narb-sw-installer install finished."
    ;;
  *[lL]inux*|*IX*)
    echo This is some other Unix, likely Linux

    echo '' && \
        echo "installing rce..."
    cd rce
    make install
    if test $? != 0; then
	echo "narb-sw-installer: rce make install error!"
	exit 1
    fi

    echo '' && \
        echo "installing narb..."
    cd ../narb
    make install
    if test $? != 0; then
        echo "narb-sw-installer: narb make install error!"
        exit 1
    fi

    echo '' && \
     echo "narb-sw-installer install finished."
    ;;
  *)
    echo Do not know what kind of system this is, do it by hand
    ;;
esac

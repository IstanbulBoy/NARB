#!/bin/sh
#

if test "$DRAGON_PREFIX" = ""; then
    PREFIX=/usr/local/dragon
else
    PREFIX=$DRAGON_PREFIX
fi

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
  *Darwin)
    echo This is Darwin

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

echo ""
echo "    #######################################################"
echo "    #                                                     #"
echo "    #      Instructions for configuration and running     #"
echo "    #                                                     #"
echo "    #######################################################"
echo ""
echo "Samples of configuration files have been installed under $PREFIX/etc/."
echo "Before running, customize your configuration files following the samples."
echo ""
echo "You need zebra.conf, ospfd-intra.conf, ospfd-inter.conf and narb.conf."
echo ""
echo "To run NARB, you must have installed the dragon software suite."
echo ""
echo "After configuration, use $PREFIX/bin/dragon.sh to start the service."
echo "To start NARB separately, use $PREFIX/bin/run_narb.sh."
echo ""

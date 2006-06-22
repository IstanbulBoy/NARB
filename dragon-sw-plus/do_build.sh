#!/bin/sh
#

PREFIX=/usr/local/dragon

if test ! -f $PREFIX/bin/dragon.sh; then
	echo "narb-sw-builder: $PREFIX/bin/dragon.sh does not exit. Install dragon-sw first."
	exit 1
fi

case `uname` in
  *BSD)
    echo This is BSD Unix

    echo '' && \
	echo 'configuring rce...'
    cd rce
    ./configure --with-ext-attr CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
	echo "narb-sw-builder: rce configure error!"
	exit 1
    fi

    echo '' && \
	echo 'making rce...'
    make
    if test $? != 0; then
	echo "narb-sw-builder: rce make error!"
	exit 1
    fi

    echo '' && \
	echo 'configuring narb...'
    cd ../narb
    ./configure CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
	echo "narb-sw-builder: narb configure error!"
	exit 1
    fi

    echo '' && \
	echo 'making narb...'
    make
    if test $? != 0; then
	echo "narb-sw-builder: narb make error!"
	exit 1
    fi

    echo '' && \
	echo 'narb-sw-builder build finished.'

    echo '' && \
        echo "Now, as root, run 'sh do_install.sh' to complete the installation."
    ;;
  *[lL]inux*|*IX*)
    echo This is some other Unix, likely Linux

    echo '' && \
	echo 'configuring rce...'
    cd rce
    ./configure --prefix=$PREFIX --with-ext-attr CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
	echo "narb-sw-builder: rce configure error!"
	exit 1
    fi

    echo '' && \
	echo 'making rce...'
    make
    if test $? != 0; then
	echo "narb-sw-builder: rce make error!"
	exit 1
    fi

    echo '' && \
	echo 'configuring narb...'
    cd ../narb
    ./configure --prefix=$PREFIX CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
        echo "narb-sw-builder: narb configure error!"
        exit 1
    fi

    echo '' && \
	echo 'making narb...'
    make
    if test $? != 0; then
        echo "narb-sw-builder-builder: narb make error!"
        exit 1
    fi

    echo '' && \
	echo 'dragon-sw build finished.'

    echo '' && \
        echo "Now, as root, run 'sh do_install.sh' to complete the installation."
    ;;
  *)
    echo Do not know what kind of system this is, do it by hand
    ;;
esac

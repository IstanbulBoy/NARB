#!/bin/sh
#

PREFIX=/usr/local/dragon

case `uname` in
  *BSD)
    echo This is BSD Unix

    echo '' && \
	echo 'configuring rce...'
    cd rce
    ./configure --with-ext-attr CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
	echo "dragon-sw-plus: rce configure error!"
	exit 1
    fi

    echo '' && \
	echo 'making rce...'
    make
    if test $? != 0; then
	echo "dragon-sw-plus: rce make error!"
	exit 1
    fi

    echo '' && \
	echo 'configuring narb...'
    cd ../narb
    ./configure CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
	echo "dragon-sw-plus: narb configure error!"
	exit 1
    fi

    echo '' && \
	echo 'making narb...'
    make
    if test $? != 0; then
	echo "dragon-sw-plus: narb make error!"
	exit 1
    fi

    echo '' && \
	echo 'dragon-sw-plus build finished.'

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
	echo "dragon-sw-plus: rce configure error!"
	exit 1
    fi

    echo '' && \
	echo 'making rce...'
    make
    if test $? != 0; then
	echo "dragon-sw-plus: rce make error!"
	exit 1
    fi

    echo '' && \
	echo 'configuring narb...'
    cd ../narb
    ./configure --prefix=$PREFIX CFLAG=-g CPPFLAG=-g
    if test $? != 0; then
        echo "dragon-sw-plus: narb configure error!"
        exit 1
    fi

    echo '' && \
	echo 'making narb...'
    make
    if test $? != 0; then
        echo "dragon-sw-plus: narb make error!"
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

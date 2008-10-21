#!/bin/sh
#

if test "$DRAGON_PREFIX" = ""; then
    PREFIX=/usr/local/dragon
else
    PREFIX=$DRAGON_PREFIX
fi

if test ! -f $PREFIX/bin/dragon.sh; then
	echo "narb-sw-builder: $PREFIX/bin/dragon.sh does not exit. Install dragon software swuite first."
	echo "Go http://dragon.east.isi.edu/twiki/bin/view/Main/VLSR to download dragon-sw-snapshot.xxxx.tar.gz."
	exit 1
fi

case `uname` in
  *BSD)
    echo This is BSD Unix

    echo '' && \
	echo 'configuring rce...'
    cd rce
    ./configure  --prefix=$PREFIX --enable-ext-attr CFLAGS=-g CPPFLAGS=-g
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
    ./configure  --prefix=$PREFIX CFLAGS=-g CPPFLAGS=-g
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
    ./configure --prefix=$PREFIX --enable-ext-attr CFLAGS=-g CPPFLAGS=-g
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
    ./configure --prefix=$PREFIX CFLAGS=-g CPPFLAGS=-g
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
  *Darwin)
    echo This is Darwin

    echo '' && \
	echo 'configuring rce...'
    cd rce
    ./configure  --prefix=$PREFIX --enable-ext-attr CFLAGS=-g CPPFLAGS=-g
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
    ./configure  --prefix=$PREFIX CFLAGS=-g CPPFLAGS=-g
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
  *)
    echo Do not know what kind of system this is, do it by hand
    ;;
esac

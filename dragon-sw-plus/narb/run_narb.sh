#!/bin/sh
#

if test "$DRAGON_PREFIX" = ""; then
    PREFIX=/usr/local/dragon
else
    PREFIX=$DRAGON_PREFIX
fi

PREFIX2=/usr/local

if test ! -f $PREFIX/bin/dragon.sh; then
    echo "narb-sw: $PREFIX/bin/dragon.sh does not exit. Install dragon-sw first."
    exit 1
fi

if test -f $PREFIX/etc/schema_combo.rsd; then
    SCHEMA_FILE=$PREFIX/etc/schema_combo.rsd
elif test -f $PREFIX2/etc/schema_combo.rsd; then
    SCHEMA_FILE=$PREFIX2/etc/schema_combo.rsd
elif test -f ./rce/schema_combo.rsd; then
    SCHEMA_FILE=./rce/schema_combo.rsd
else
    echo 'narb-sw: RCE needs schema_combo.rsd!  -- refer to rce/schema_combo.rsd.sample...'
    exit 1
fi 

if test -f $PREFIX/etc/narb.conf; then
    NARB_CONF=$PREFIX/etc/narb.conf
elif test -f $PREFIX2/etc/narb.conf; then
    NARB_CONF=$PREFIX2/etc/narb.conf
elif test -f ./narb/narb.conf; then
    NARB_CONF=./narb/narb.conf
else
    echo 'narb-sw: NARB needs narb.conf!  -- refer to narb/narb.conf.sample...'
    exit 1
fi      

ospf_intra_pid=`ps axwww | grep -v awk | awk '{if (match($0, ".*/ospfd-intra")  || match($0, "ospfd -d -f")) print $1}'`
ospf_inter_pid=`ps axwww | grep -v awk | awk '{if (match($0, ".*/ospfd-inter")  || match($0, "ospfd -d -P 2614 -I -f")) print $1}'`

if test "$ospf_intra_pid" = ""; then
    echo '****** WARNING: IntRA-domain OSPFd not running! ******'
fi

if test "$ospf_inter_pid" = ""; then
    echo '****** WARNING: IntER-domain OSPFd not running! ******'
fi

if test -f $PREFIX/sbin/rce; then
    killall -9 rce
    sleep 3
    $PREFIX/sbin/rce -d -s $SCHEMA_FILE
elif test -f $PREFIX2/sbin/rce; then
    killall -9 rce
    sleep 3
    $PREFIX2/sbin/rce -d -s $SCHEMA_FILE
elif test -f ./rce/rce; then
    killall -9 rce
    sleep 3
    ./rce/rce -d -s $SCHEMA_FILE
else
    echo '******* ERROR: RCE executable does not exist *******'
    exit 1
fi

if test -f $PREFIX/sbin/narb; then
    killall -9 narb
    sleep 3
    $PREFIX/sbin/narb -d -f $NARB_CONF
elif test -f $PREFIX2/sbin/narb; then
    killall -9 narb
    sleep 3
    $PREFIX2/sbin/narb -d -f $NARB_CONF
elif test -f ./narb/narb; then
    killall -9 narb
    sleep 3
    ./narb/narb -d -f $NARB_CONF
else
    echo '******* ERROR: NARB executable does not exist *******'
    exit 1
fi

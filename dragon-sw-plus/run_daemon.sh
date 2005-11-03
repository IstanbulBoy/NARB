#!/bin/sh
#

if test -f ./rce/schema_combo.rsd; then
    SCHEMA_FILE=$SCHEMA_FILE
elif test -f /usr/local/etc/schema_combo.rsd; then
    SCHEMA_FILE=/usr/local/etc/schema_combo.rsd
else
    echo 'dragon-sw-plus: RCE needs schema_combo.rsd!  -- refer to rce/schema_combo.rsd.sample...'
    exit 1
fi 

if ! test -f ./narb/narb.conf -o -f /usr/local/etc/narb.conf; then
    echo 'dragon-sw-plus: NARB needs narb.conf!  -- refer to narb/narb.conf.sample...'
    exit 1
fi      

ospf_intra_pid=`ps axwww | grep -v awk | awk '{if (match($0, "ospfd \-d \-f")) print $1}'`
ospf_inter_pid=`ps axwww | grep -v awk | awk '{if (match($0, "ospfd \-d \-P 2614 \-I \-f")) print $1}'`

if ! test "$ospf_intra_pid" = ""; then
    echo 'WARNING! dragon-sw-plus: IntRA-domain OSPFd not running!'
fi

if ! test "$ospf_inter_pid" = ""; then
    echo 'WARNING! dragon-sw-plus: IntER-domain OSPFd not running!'
fi

if test -f ./rce/rce; then
    killall -9 rce
    ./rce/rce -d -s $SCHEMA_FILE
elif test -f /usr/local/dragon/sbin/rce; then
    killall -9 rce
    /usr/local/dragon/sbin/rce -d -s $SCHEMA_FILE
else
    echo 'dragon-sw-plus: RCE executable does not exist...'
    exit 1
fi

if test -f ./narb/narb; then
    killall -9 narb
    ./narb/narb -d
elif test -f /usr/local/dragon/sbin/narb; then
    killall -9 narb
    /usr/local/dragon/sbin/narb -d
else
    echo 'dragon-sw-plus: NARB executable does not exist...'
    exit 1
fi

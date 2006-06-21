#!/bin/sh
#

if test -f ./rce/schema_combo.rsd; then
    SCHEMA_FILE=./rce/schema_combo.rsd
elif test -f /usr/local/etc/schema_combo.rsd; then
    SCHEMA_FILE=/usr/local/etc/schema_combo.rsd
elif test -f /usr/local/dragon/etc/schema_combo.rsd; then
    SCHEMA_FILE=/usr/local/dragon/etc/schema_combo.rsd
else
    echo 'dragon-sw-plus: RCE needs schema_combo.rsd!  -- refer to rce/schema_combo.rsd.sample...'
    exit 1
fi 

if test -f ./narb/narb.conf; then
    NARB_CONF=./narb/narb.conf
elif test -f /usr/local/etc/narb.conf; then
    NARB_CONF=/usr/local/etc/narb.conf
elif test -f /usr/local/dragon/etc/narb.conf; then
    NARB_CONF=/usr/local/dragon/etc/narb.conf
else
    echo 'dragon-sw-plus: NARB needs narb.conf!  -- refer to narb/narb.conf.sample...'
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

if test -f ./rce/rce; then
    killall -9 rce
    sleep 3
    ./rce/rce -d -s $SCHEMA_FILE
elif test -f /usr/local/dragon/sbin/rce; then
    killall -9 rce
    sleep 3
    /usr/local/dragon/sbin/rce -d -s $SCHEMA_FILE
else
    echo '******* ERROR: RCE executable does not exist *******'
    exit 1
fi

if test -f ./narb/narb; then
    killall -9 narb
    sleep 3
    ./narb/narb -d -f $NARB_CONF
elif test -f /usr/local/dragon/sbin/narb; then
    killall -9 narb
    sleep 3
    /usr/local/dragon/sbin/narb -d -f $NARB_CONF
else
    echo '******* ERROR: NARB executable does not exist *******'
    exit 1
fi

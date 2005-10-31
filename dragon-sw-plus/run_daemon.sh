#!/bin/sh
#

SCHEMA_FILE=./rce/schema_combo.rsd

if test -f ./rce/schema_combo.rsd; then
    SCHEMA_FILE=$SCHEMA_FILE
elif test -f /usr/local/etc/schema_combo.rsd; then
    SCHEMA_FILE=/usr/local/etc/schema_combo.rsd
else
    echo 'dragon-sw-plus: RCE needs schema_combo.rsd!  -- refer to schema_combo.rsd.sample...'
    exit 1
fi 

if ! test -f ./narb/narb.conf -o -f /usr/local/etc/narb.conf; then
    echo 'dragon-sw-plus: NARB needs narb.conf!  -- refer to narb.conf.sample...'
    exit 1
fi      

if test -f ./rce/rce; then
    ./rce/rce -d -s $SCHEMA_FILE
else
    echo 'dragon-sw-plus: RCE executable does not exist...'
    exit 1
fi

if test -f ./narb/narb; then
    ./narb/narb -d
else
    echo 'dragon-sw-plus: NARB executable does not exist...'
    exit 1
fi
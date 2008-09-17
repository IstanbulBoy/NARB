#!/bin/sh

cat ./$1.xml | nc -q6 127.0.0.1 8080

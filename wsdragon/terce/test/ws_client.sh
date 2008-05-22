#!/bin/sh

cat ./$1.xml | nc -q1 127.0.0.1 8080

#!/bin/bash

if [ -z "$1" ] ; then
    find . -name '*.h' -exec $0 \{\} \;
else
 param=$1
 temp=`mktemp /tmp/fixincludes.XXXXXXXX`
 cat $1 | sed -e s/\#include\ *\<\\\(.*\\\)\>/\#include\ \"\\1\"/  > $temp
 mv -f $temp $1
 rm -f $temp
fi

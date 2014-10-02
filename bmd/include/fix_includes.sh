#!/bin/bash

if [ -z "$1" ] ; then
    echo Usage: $0 /where/to/start && exit 1
else
    DIR="$1"
fi

temp="`mktemp /tmp/find_incl.XXXXXXXX`"
# first, find C/H files...

FILES=`find $DIR -name '*.h' -or -name '*.c' `


for plik in $FILES ; do
    # find #include tags and get header file names from them
    included=`cat $plik | dos2unix |sed -re '/^ *#include *<(.*)> *$/s/^ *#include *<(.*)> *$/\1/p' \
    -e '/.*/d'`
    if [ -z "$included" ] ; then
	continue;
    fi
    echo "====== $plik ======"
    for inc in $included ; do 

	found="`find . -name $inc | head -n1 | cut -d/ -f2-`" 
	echo $inc '=' $found
	if [ -z "$found" ] ; then
	    continue;
	fi

	#update file: escape path for use with sed...
	found="`echo $found | sed -e 's/\//\\\\\//g'`"
	#...and do it :)
	cat $plik | dos2unix |sed -re 's/^ *#include *<'$inc'> *$/#include <'$found'>/' > $temp
	cat $temp > $plik
	
    done
    
    
    # now find correct filename relative to $INCDIR... 
    # and replace all include's with a proper value :)
done

rm $temp

#! /bin/bash
 
release=$1

if [ -z "$release" ]; then
	release="1.3"
fi


temp=`mktemp /tmp/relXXXXXX`
for plik in src/lib*/Makefile.am    src/libbmdlog/*/Makefile.am    src/  src/libbmddb/*/Makefile.am; do 
	echo -n Processing $plik... 
	echo -e "AM_LDFLAGS=-release $release " > $temp
	grep -v 'AM_LDFLAGS=-release' $plik >> $temp
	mv -f $temp $plik
	
	echo done
done


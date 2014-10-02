#!/bin/sh
MAKEFLAGS="-j"
echo Usuwam niepotrzebne pliki.
make clean
rm -rf depcomp configure
rm -rf client.*
find . -name Makefile.in -exec rm -f \{\} \;

echo Produkuje slowniki bledow
pushd .
cd src/libbmderr/ErrorDict
rm -f make_err_dict
make
if [ $? != 0 ]
then
	exit
fi
./make_err_dict ../err_dict.c
if [ $? != 0 ]
then
	exit
fi
popd



echo Przygotowuje BMD do budowania.
(
	find `pwd` -name "*.[ch]" >cscope.files &&
	find `pwd` -name "*.cpp" >>cscope.files &&
	cscope -b -u cscope.files
) &
aclocal -I src/aclocal || exit 1
autoheader || exit 1
autoconf || exit 1
libtoolize --force || exit 1
automake -a || exit 1
echo Konfiugruje BMD.
./configure --prefix /usr/bmd/bmd -C --disable-postgres --disable-oracle --enable-db2 --disable-mssql --disable-client --enable-xml --enable-esp  --enable-vdebug $@

# perl src/scripts/generate_magics.pl > include/bmd/common/bmd-magic.h
# echo ' const char * bmd_error_strings [] = {}; ' > include/bmd/libbmderr/bmd-error-magic.h
make -C src/libbmderr
# perl src/scripts/generate_errors.pl > include/bmd/libbmderr/bmd-error-magic.h

if [ cat Makefile | grep distcc ]; then
	make -j
else
	make -j3
fi

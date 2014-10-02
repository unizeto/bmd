#
# spec file for package BMD
#
# Copyright  (c) 2010 BMD Team, Unizeto
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#


%define is_mandrake %(test -e /etc/mandrake-release && echo 1 || echo 0)
%define is_suse %(test -e /etc/SuSE-release && echo 1 || echo 0)
%define is_fedora %(test -e /etc/fedora-release && echo 1 || echo 0)
%define is_pld %(test -e /etc/pld-release && echo 1 || echo 0)
%define is_redhat %(test -e /etc/redhat-release && echo 1 || echo 0)


%define dist redhat
%define disttag rh
%define build_client 0

%define is_sles9 0

%if %is_suse
    %if %(test -z `cat /etc/SuSE-release | grep 'SUSE LINUX Enterprise Server 9'` && echo 1 || echo 0)
    	%define is_sles9 0
        %define is_suse 1
    %else
        %define is_sles9 1
        %define is_suse 0
    %endif
%endif

%if %is_redhat
%define build_client 0
%endif 

%if %is_mandrake
%define dist mandrake
%define disttag mdk
%endif

%if %is_suse
%define dist suse
%define disttag suse
%define kde_path /opt/kde3
%endif

%if %is_sles9
%define dist sles9
%define disttag sles9
%endif

%if %is_fedora
%define dist fedora
%define disttag rhfc
%endif

%if %is_pld
%define dist pld
%define disttag pld
%endif 

%if %{undefined release_suffix}
    %define release_suffix t
%endif

%define distver %(release="`rpm -q --queryformat='%{VERSION}' %{dist}-release 2> /dev/null | tr . : | sed s/://g`" ; if test $? != 0 ; then release="" ; fi ; echo "$release")
%define distlibsuffix %(%_bindir/kde-config --libsuffix 2>/dev/null)
%define _lib lib%distlibsuffix

%define reldate %(date '+%y%m%d')

%define baeversion 2.3

%define _rpmfilename  %%{ARCH}/%%{NAME}-%%{VERSION}-%%{RELEASE}-bmd_%baeversion.%%{ARCH}.rpm


Name:      bmd_p1_%baeversion
Summary:   BMD 
Group: Other/unizeto
Version:   2.3
Release:   %{reldate}
Distribution: %{disttag}
%define libver %baeversion
License:   propertiary
Vendor:    Unizeto Technologies S.A.
Packager:  Artur Karczmarczyk, Łukasz Klimek, Tomasz Wojciechowski
Source:   bmd.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-build
Requires: libesmtp
Requires: curl-libs
Requires: pcre >= 6.3 
%if %build_client
BuildRequires: wxGTK-devel >= 2.5
%endif
BuildRequires: postgresql-devel 
BuildRequires: pcre-devel >= 6.3 
BuildRequires: libesmtp-devel libesmtp
BuildRequires: libesmtp-devel curl-devel pcre-devel
BuildRequires: glibc libtool gcc gcc-c++ libstdc++-devel libxml2-devel
BuildRequires: automake >= 1.8
BuildRequires: autoconf >= 2.59
BuildRequires: openssl >= 0.9.7
BuildRequires: libxml2-devel 
BuildRequires: pkgconfig
BuildRequires: xmlsec1-devel
BuildRequires: xmlsec1-openssl-devel
BuildRequires: flex
BuildRequires: java-1.7.0-openjdk-devel

Prefix: /usr
Patch0: bmd-build-rh6.patch


#Conflicts: bmd = trunk
#Obsoletes: bmd = 1.7
Provides: bmd_p1 = %version

%description
BMD libraries


%package main
Group: Other/unizeto
Summary: BMD binaries
Requires: bmd_p1-libs

%description main
Main package with binaries

%files main
%defattr(-,root,root)
%prefix/bin


%package libs
Group: Other/unizeto
Summary: BMD libraries
Requires: libxml2
Requires: xmlsec1 xmlsec1-nss xmlsec1-nss-devel
Requires: postgresql-libs >= 8.2
#Obsoletes: bmd-libs

%description libs
BMD libraries 

%post libs

/sbin/ldconfig

%postun libs

/sbin/ldconfig


%files libs
%defattr(-,root,root)
%prefix/lib/lib*.so*

%package devel
Group: Other/unizeto
Summary: BMD development stuff

%description devel
development files

%files devel 
%defattr(-,root,root)
%prefix/include
%prefix/lib/*.a
%prefix/lib/*.la


%prep
if [ ! -f /opt/ibm/db2/V10.5/include/sqlcli1.h ] ; then
    echo IBM DB2 MUST be installed in /opt/ibm/db2/V10.5 in order to 
    echo build this package. Please install \(or symlink\) it.
    exit 1
fi

echo %_target
echo %_target_alias
echo %_target_cpu
echo %_target_os
echo %_target_vendor
echo Building %{name}-%{version}-%{release}

rm -fr %buildroot/*

%setup -b 0 -q -n bmd
%patch0 -p2

%build
#aclocal || exit 1
#autoheader || exit 1
#autoconf || exit 1
#libtoolize || exit 1
#automake -a || exit 1

echo Usuwam niepotrzebne pliki.
rm -rf depcomp configure
rm -rf client.*
find . -name Makefile.in -exec rm -f \{\} \;

echo Produkuje slowniki bledow
pushd .
cd src/libbmderr/ErrorDict
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


#grep -v 'XMLSEC' configure.in > configure.in2
#grep -v 'xmlsec' configure.in2 > configure.in

#sed -e 's/libbmdxades//' src/Makefile.am > tempaaa
#mv -f tempaaa src/Makefile.am

sh ./rob_release.sh %{libver}

echo Przygotowuje BMD do budowania.
aclocal -I src/aclocal || exit 1
autoheader || exit 1
autoconf || exit 1
libtoolize --force || exit 1
automake -a || exit 1
echo Konfiugruje BMD.

./configure --enable-vdebug -C  --enable-server  \
    --enable-libs --prefix=%prefix \
    --enable-xml --enable-esp --enable-pgsql \
    --with-db2-prefix=/opt/ibm/db2/V10.5 \
%if %build_client 
    --enable-client  \
    --with-wxconfig=%prefix/bin/wx-config \
%else
    --disable-client \
%endif


make  -s -C src/libbmdglobals
perl src/scripts/generate_magics.pl > include/bmd/common/bmd-magic.h
echo ' const char * bmd_error_strings [] = {}; ' > include/bmd/libbmderr/bmd-error-magic.h
echo -n -e '\n' >> include/bmd/libbmderr/bmd-error-magic.h
make -s -C src/libbmderr
perl src/scripts/generate_errors.pl > include/bmd/libbmderr/bmd-error-magic.h
echo -n -e '\n' >> include/bmd/libbmderr/bmd-error-magic.h

make -s 

%if "%run_tests" == "1"
    sh ./run_tests.sh
%endif

%install
make -s DESTDIR=%buildroot install
mkdir -p %buildroot/%prefix/include
cp -rf include/bmd %buildroot/%prefix/include/bmd 
cp -rf include/pkcs11 %buildroot/%prefix/include/pkcs11 



%clean
[ ${RPM_BUILD_ROOT} != "/" ] && rm -rf ${RPM_BUILD_ROOT}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig


%changelog

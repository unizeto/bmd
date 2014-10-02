AC_DEFUN([AM_OPTIONS_DB2CONFIG],
[

do_enable_db2="no"

AC_ARG_ENABLE(db2,[  --enable-db2	Enable db2 support],
    do_enable_db2=$enableval
)

AC_ARG_WITH(db2,[  --with-db2 With DB2 support],
	if test  "x$do_enable_db2" = "x" ; then
		do_enable_db2=$withval
	fi
)

enable_db2=$do_enable_db2


AC_ARG_WITH(db2-prefix,[  --with-db2-prefix=PFX   Prefix where db2 OCI libs are installed (optional)],
            [ db2_prefix="$withval"; enable_db2=yes ] , [ db2_prefix=""; enable_db2=no])
])


dnl Define OCISQL_CFLAGS and OCISQL_LIBS
AC_DEFUN([AM_PATH_DB2CONFIG],
[dnl
dnl Set the cflags and libraries from subversion for those who haven't oracle install client installed.
dnl
if test "x$db2_prefix" != "x" ; then
    DB2_PREFIX="$db2_prefix"
else
    DB2_PREFIX=/usr/lib/db2
fi

     AS_CASE([$target_cpu],
	[x86_64],   [DB2_LIBS="-ldb2 -L$DB2_PREFIX/lib64"],
        [DB2_LIBS="-ldb2 -L$DB2_PREFIX/lib32"]
    )
    DB2_INCLUDE="-I$DB2_PREFIX/include"
    DB2_CFLAGS="-I$DB2_PREFIX/include"
    DB2_CPPFLAGS="-I$DB2_PREFIX/include"

    DB2_CXXFLAGS="-I$DB2_PREFIX/include"

    AC_SUBST(DB2_INCLUDE)
    AC_SUBST(DB2_CFLAGS)
    AC_SUBST(DB2_CPPFLAGS)
    AC_SUBST(DB2_CXXFLAGS)
    AC_SUBST(DB2_LIBS)

    DB2_BMDDB_LIBS="${DB2_LIBS}"
    AC_SUBST(DB2_BMDDB_LIBS)
])

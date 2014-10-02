

AC_DEFUN([AM_OPTIONS_MSSQLCONFIG],
[

do_enable_mssql="no"

AC_ARG_ENABLE(mssql,[  --enable-mssql	Enable Microsoft SQL Server support],
    do_enable_mssql=$enableval
)

AC_ARG_WITH(mssql,[  --with-mssql With Microsoft SQL Server support],
	if test  "x$do_enable_mssql" = "x" ; then
		do_enable_mssql=$withval
	fi
)

enable_mssql=$do_enable_mssql


dnl AC_ARG_WITH(oracle-prefix,[  --with-oracle-prefix=PFX   Prefix where Oracle OCI libs are installed (optional)],
dnl            [ oracle_prefix="$withval"; enable_oracle=yes ] , [ oracle_prefix=""; enable_oracle=no])
])


dnl Define OCISQL_CFLAGS and OCISQL_LIBS
AC_DEFUN([AM_PATH_MSSQLCONFIG],
[dnl
dnl Set the cflags and libraries from subversion for those who haven't oracle install client installed.
dnl
    MSSQL_PREFIX=\$\{top_srcdir\}/../../common/oci10

    MSSQL_INCLUDE="-I$MSSQL_PREFIX/rdbms/public"
    MSSQL_CFLAGS="-I$MSSQL_PREFIX/rdbms/public"
    MSSQL_CPPFLAGS="-I$MSSQL_PREFIX/rdbms/public"

    MSSQL_CXXFLAGS="-I$MSSQL_PREFIX/rdbms/public"
    MSSQL_LIBS="-lsybdb -ltds"
    
dnl    -L$MSSQL_PREFIX/lib"

    AC_SUBST(MSSQL_INCLUDE)
    AC_SUBST(MSSQL_CFLAGS)
    AC_SUBST(MSSQL_CPPFLAGS)
    AC_SUBST(MSSQL_CXXFLAGS)
    AC_SUBST(MSSQL_LIBS)

dnl    BMDDB_LIBS="-lbmddb_oracle ${ORACLE_LIBS}"
dnl    AC_SUBST(BMDDB_LIBS)

])

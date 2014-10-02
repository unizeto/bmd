

AC_DEFUN([AM_OPTIONS_ORACLECONFIG],
[

do_enable_oracle="no"

AC_ARG_ENABLE(oracle,[  --enable-oracle	Enable Oracle support],
    do_enable_oracle=$enableval
)

AC_ARG_WITH(oracle,[  --with-oracle With Oracle support],
	if test  "x$do_enable_oracle" = "x" ; then
		do_enable_oracle=$withval
	fi
)

enable_oracle=$do_enable_oracle


AC_ARG_WITH(oracle-prefix,[  --with-oracle-prefix=PFX   Prefix where Oracle OCI libs are installed (optional)],
            [ oracle_prefix="$withval"; enable_oracle=yes ] , [ oracle_prefix=""; enable_oracle=no])
])


dnl Define OCISQL_CFLAGS and OCISQL_LIBS
AC_DEFUN([AM_PATH_ORACLECONFIG],
[dnl
dnl Set the cflags and libraries from subversion for those who haven't oracle install client installed.
dnl
if test "x$oracle_prefix" != "x" ; then
    ORACLE_PREFIX="$oracle_prefix"
    ORACLE_RDBMS_PREFIX="$oracle_prefix"
else
    ORACLE_PREFIX=/usr/lib/oracle/11.2/client64/
    ORACLE_RDBMS_PREFIX=/usr/oracle/app/product/11.2.0/dbhome_1/
fi
    ORACLE_INCLUDE="-I$ORACLE_RDBMS_PREFIX/rdbms/public -I$ORACLE_PREFIX/include"
    ORACLE_CFLAGS="-I$ORACLE_RDBMS_PREFIX/rdbms/public -I$ORACLE_PREFXI/include"
    ORACLE_CPPFLAGS="-I$ORACLE_RDBMS_PREFIX/rdbms/public -I$ORACLE_PREFIX/include"

    ORACLE_CXXFLAGS="-I$ORACLE_RDBMS_PREFIX/rdbms/public -I$ORACLE_PREFIX/include"
    ORACLE_LIBS="-lociei -lclntsh -lnnz11 -L$ORACLE_PREFIX -L$ORACLE_PREFIX/lib"



    AC_SUBST(ORACLE_INCLUDE)
    AC_SUBST(ORACLE_CFLAGS)
    AC_SUBST(ORACLE_CPPFLAGS)
    AC_SUBST(ORACLE_CXXFLAGS)
    AC_SUBST(ORACLE_LIBS)

    ORACLE_BMDDB_LIBS="${ORACLE_LIBS}"
    AC_SUBST(ORACLE_BMDDB_LIBS)
])

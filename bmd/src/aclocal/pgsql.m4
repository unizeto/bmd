AC_DEFUN([AM_OPTIONS_PGCONFIG],
[

do_enable_pgsql=no

AC_ARG_ENABLE(postgres,[  --enable-postgres	Enable Postgresql support (default=no)],
    do_enable_pgsql="yes"
)

AC_ARG_ENABLE(pgsql,[],
    do_enable_pgsql="yes"
)


AC_ARG_WITH(pgsql,[--with-pgsql		With PostgreSQL suppoer (default=no)],
    do_enable_pgsql="yes"
)


enable_pgsql=$do_enable_pgsql

enable_pgsql=no
    
AC_ARG_WITH(postgres-prefix,[  --with-postgres-prefix=PFX	Prefix where PostgreSQL is installed (optional)],
    [ pgsql_prefix="$withval" ; enable_pgsql="yes" ] , 
    pgsql_prefix="")
    
AC_ARG_WITH(postgres-exec-prefix,[  --with-postgres-exec-prefix=PFX	Exec prefix where PostgreSQL is installed (optional)],
    [ pgsql_exec_prefix="$withval" ; enable_pgsql="yes" ] , 
    pgsql_exec_prefix="")
    
AC_ARG_WITH(pg-config,[  --with-pgconfig=CONFIG	pg_config script to use (optional)],
    [ pg_config_name="$withval" ; enable_pgsql="yes" ] , 
    pg_config_name="")
])

dnl AM_PATH_PGCONFIG([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for PGSQL, and define PGSQL_CFLAGS and PGSQL_LIBS
AC_DEFUN([AM_PATH_PGCONFIG],
[dnl
dnl Get the cflags and libraries from the pg_config script
dnl
  if test "x$pg_config_name" != "x" ; then
     if test "x${PGSQL_CONFIG+set}" != "xset" ; then
        PGSQL_CONFIG="$pg_config_name"
     fi
  fi

  if test "x$pgsql_exec_prefix" != "x" ; then
     if test "x${PGSQL_CONFIG+set}" != "xset" ; then
        PGSQL_CONFIG="$pgsql_exec_prefix/bin/pg_config"
     fi
  fi
  if test "x$pgsql_prefix" != "x" ; then
     if test "x${PGSQL_CONFIG+set}" != "xset" ; then
        PGSQL_CONFIG="$pgsql_prefix/bin/pg_config"
     fi
  fi

if test "$enable_pgsql" = "yes" ; then
  AC_REQUIRE([AC_CANONICAL_TARGET])
  AC_PATH_PROG(PGSQL_CONFIG, pg_config, no, [$PATH:/usr/lib/postgresql/bin])
  AC_MSG_CHECKING(for PGSQL)
  no_pgsql=""
  if test "$PGSQL_CONFIG" = "no" ; then
    PGSQL_CFLAGS=""
    PGSQL_LIBS=""
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([Postgresql requested, but pg_config cannot be found. Please check --with-pgsql-prefix and try again.])
     ifelse([$2], , :, [$2])
  else
    PGSQL_INCLUDE=-I`$PGSQL_CONFIG --includedir`
    PGSQL_CFLAGS=-I`$PGSQL_CONFIG --includedir`
    PGSQL_CPPFLAGS=-I`$PGSQL_CONFIG --includedir`
    PGSQL_CXXFLAGS=-I`$PGSQL_CONFIG --includedir`
    PGSQL_LIBS="-lpq -L`$PGSQL_CONFIG --libdir`"
    AC_MSG_RESULT(yes)
    ifelse([$1], , :, [$1])
  fi
  AC_SUBST(PGSQL_INCLUDE)
  AC_SUBST(PGSQL_CFLAGS)
  AC_SUBST(PGSQL_CPPFLAGS)

  AC_SUBST(PGSQL_CXXFLAGS)
  AC_SUBST(PGSQL_LIBS)
  POSTGRES_BMDDB_LIBS="${PGSQL_LIBS}"
  AC_SUBST(POSTGRES_BMDDB_LIBS)
fi
])

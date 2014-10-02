AM_LDFLAGS=-release 2.3 

AM_CPPFLAGS = -Werror -Wall

noinst_PROGRAMS = str2int
str2int_LDADD = libbmderr.la
str2int_CPPFLAGS = -Werror -Wall


lib_LTLIBRARIES = libbmderr.la
libbmderr_la_SOURCES=libbmderr.c err_stack.c err_dict.c
libbmderr_la_LIBADD=../libbmdglobals/libbmdglobals.la

libbmderr_la_CPPFLAGS = -Werror -Wall


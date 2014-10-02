AM_LDFLAGS=-release 2.3 
AM_LDFLAGS =-release 2.3.0
lib_LTLIBRARIES = libCppJavaApiWrapper.la

libCppJavaApiWrapper_la_SOURCES = JvmEnvironment.cpp CppJavaApiWrapper.cpp proCertumJvmEnv.cpp
libCppJavaApiWrapper_la_CPPFLAGS = -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux

libCppJavaApiWrapper_la_LIBADD = -lpthread -ldl

# Microsoft Developer Studio Project File - Name="libbmdutils_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libbmdutils_static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbmdutils_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdutils_static.mak" CFG="libbmdutils_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdutils_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libbmdutils_static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdutils_static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "libbmdutils_static___Win32_Release"
# PROP BASE Intermediate_Dir "libbmdutils_static___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../Release_static"
# PROP Intermediate_Dir "Release_static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "../../include/libbmdutils" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdasn1_core" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdpr/prlib-common" /I "../../include/libbmdpr/bmd_req/asn1clib" /I "../../include/libbmdpr/bmd_resp/asn1clib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Release_static\libbmdutils.lib"

!ELSEIF  "$(CFG)" == "libbmdutils_static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libbmdutils_static___Win32_Debug"
# PROP BASE Intermediate_Dir "libbmdutils_static___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../Debug_static"
# PROP Intermediate_Dir "Debug_static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /ZI /Od /I "../../include/libbmdutils" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdasn1_core" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdpr/prlib-common" /I "../../include/libbmdpr/bmd_req/asn1clib" /I "../../include/libbmdpr/bmd_resp/asn1clib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../../Debug_static\libbmdutils.lib"

!ENDIF 

# Begin Target

# Name "libbmdutils_static - Win32 Release"
# Name "libbmdutils_static - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\libbmdutils.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\libbmdutils\libbmdutils.h
# End Source File
# End Group
# End Target
# End Project

# Microsoft Developer Studio Project File - Name="libbmdpr" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libbmdpr - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbmdpr.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdpr.mak" CFG="libbmdpr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdpr - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libbmdpr - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdpr - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "libbmdpr - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "libbmdpr___Win32_Debug"
# PROP BASE Intermediate_Dir "libbmdpr___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../exe_trunk"
# PROP Intermediate_Dir "libbmdpr___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include" /I "../../../../../common/win32/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libbmdpr - Win32 Release"
# Name "libbmdpr - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\bmd_datagram\bmd_datagram.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\bmd_datagram_internal.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\bmd_datagram_set.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\bmd_datagram_set_internal.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\bmd_datagram_set_utils.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\bmd_datagram_utils.c
# End Source File
# Begin Source File

SOURCE=..\bmd_resp\bmd_resp.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\asn1clib\BMDDatagram.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\asn1clib\BMDDatagramSet.c
# End Source File
# Begin Source File

SOURCE=..\bmd_resp\asn1clib\Kontrolka.c
# End Source File
# Begin Source File

SOURCE=..\bmd_resp\asn1clib\Kontrolki.c
# End Source File
# Begin Source File

SOURCE=.\libbmdpr.def
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\asn1clib\MetaData.c
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\asn1clib\MetaDataSequence.c
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_ANY.c"
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_BIT_STRING.c"
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_INTEGER.c"
# End Source File
# Begin Source File

SOURCE=..\bmd_resp\prlib\PR_Kontrolka.c
# End Source File
# Begin Source File

SOURCE=..\bmd_resp\prlib\PR_Kontrolki.c
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_lib.c"
# End Source File
# Begin Source File

SOURCE=..\bmd_datagram\prlib\PR_MetaDataSequence.c
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_OBJECT_IDENTIFIER.c"
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_OCTET_STRING.c"
# End Source File
# Begin Source File

SOURCE="..\prlib-common\PR_Utils.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\bmd_datagram.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\bmd_datagram_set.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_resp\bmd_resp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\asn1clib\BMDDatagram.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\asn1clib\BMDDatagramSet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_resp\asn1clib\Kontrolka.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_resp\asn1clib\Kontrolki.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\asn1clib\MetaData.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\asn1clib\MetaDataSequence.h
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_ANY.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_BIT_STRING.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_INTEGER.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_resp\prlib\PR_Kontrolka.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_resp\prlib\PR_Kontrolki.h
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_lib.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdpr\bmd_datagram\prlib\PR_MetaDataSequence.h
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_OBJECT_IDENTIFIER.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_OCTET_STRING.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdpr\prlib-common\PR_Utils.h"
# End Source File
# End Group
# End Target
# End Project

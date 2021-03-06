# Microsoft Developer Studio Project File - Name="libbmdutils" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libbmdutils - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbmdutils.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdutils.mak" CFG="libbmdutils - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdutils - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libbmdutils - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdutils - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../win_release_klient"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDUTILS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /O2 /I "../../inc/bmd/libbmdutils" /I "../../inc/bmd/common" /I "../../inc/win" /I "../../inc/cl322" /I "../../inc/bmd/libbmdasn1_core" /I "../../inc/bmd/libbmdaa/asn1" /I "../../inc/bmd/libbmdpr/prlib-common" /I "../../inc/bmd/libbmdpr/bmd_resp/asn1clib" /I "../../inc/bmd/libbmdpr/bmd_datagram/prlib" /I "../../inc/bmd/libbmdpr/bmd_datagram/asn1clib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDUTILS_EXPORTS" /D "INC_CHILD" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cl32.lib libpcre.lib libbmdasn1_core.lib libeay32.lib ssleay32.lib /nologo /dll /machine:I386 /libpath:"../../win_release_klient" /libpath:"../../libs/win"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "libbmdutils - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../exe_trunk"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDUTILS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "../../../../../common/win32/include" /I "../../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDUTILS_EXPORTS" /D "INC_CHILD" /D "_LIB" /D "CURL_STATICLIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libpcre.lib libbmdasn1_core.lib libeay32.lib ssleay32.lib Ws2_32.lib libbmdnet.lib libbmderr.lib libcurl.lib Winmm.lib libbmdasn1_common.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"../../../../../common/win32/libs" /libpath:"../../../exe_trunk"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "libbmdutils - Win32 Release"
# Name "libbmdutils - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\aa_conf_utils.c
# End Source File
# Begin Source File

SOURCE=..\asn1_utils.c
# End Source File
# Begin Source File

SOURCE=..\kontrolki_utils.c
# End Source File
# Begin Source File

SOURCE=..\libbmdutils.c
# End Source File
# Begin Source File

SOURCE=.\libbmdutils.def
# End Source File
# Begin Source File

SOURCE=..\mem_utils.c
# End Source File
# Begin Source File

SOURCE=..\oid.c
# End Source File
# Begin Source File

SOURCE=..\os_specific.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdutils\libbmdutils.h
# End Source File
# End Group
# End Target
# End Project

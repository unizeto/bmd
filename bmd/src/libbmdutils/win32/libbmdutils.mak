# Microsoft Developer Studio Generated NMAKE File, Based on libbmdutils.dsp
!IF "$(CFG)" == ""
CFG=libbmdutils - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmdutils - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmdutils - Win32 Release" && "$(CFG)" != "libbmdutils - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdutils - Win32 Release"

OUTDIR=.\../../Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../../Release
# End Custom Macros

ALL : "$(OUTDIR)\libbmdutils.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdutils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmdutils.dll"
	-@erase "$(OUTDIR)\libbmdutils.exp"
	-@erase "$(OUTDIR)\libbmdutils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /O2 /I "../.." /I "../../include/libbmdutils" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdasn1_core" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdpr/prlib-common" /I "../../include/libbmdpr/bmd_req/asn1clib" /I "../../include/libbmdpr/bmd_resp/asn1clib" /I "../../include\libbmdpr\bmd_datagram\asn1clib" /I "../../include\libbmdpr\bmd_datagram\prlib" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDUTILS_EXPORTS" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdutils.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cl32.lib libpcre.lib libbmdasn1_core.lib libeay32.lib ssleay32.lib libbmdpr.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\libbmdutils.pdb" /machine:I386 /def:".\libbmdutils.def" /out:"$(OUTDIR)\libbmdutils.dll" /implib:"$(OUTDIR)\libbmdutils.lib" /libpath:"../../../pcre/Release" /libpath:"../../Release" /libpath:"../../../CryptLib/binaries" /libpath:"../../../openssl/out32dll" /libpath:"../../libs" 
DEF_FILE= \
	".\libbmdutils.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdutils.obj"

"$(OUTDIR)\libbmdutils.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmdutils - Win32 Debug"

OUTDIR=.\../../Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\../../Debug
# End Custom Macros

ALL : "$(OUTDIR)\libbmdutils.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdutils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmdutils.dll"
	-@erase "$(OUTDIR)\libbmdutils.exp"
	-@erase "$(OUTDIR)\libbmdutils.ilk"
	-@erase "$(OUTDIR)\libbmdutils.lib"
	-@erase "$(OUTDIR)\libbmdutils.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "../../include/libbmdaa" /I "../.." /I "../../include/libbmdutils" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdasn1_core" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdpr/prlib-common" /I "../../include/libbmdpr/bmd_req/asn1clib" /I "../../include/libbmdpr/bmd_resp/asn1clib" /I "../../include\libbmdpr\bmd_datagram\asn1clib" /I "../../include\libbmdpr\bmd_datagram\prlib" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDUTILS_EXPORTS" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdutils.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib cl32.lib libpcre.lib libbmdasn1_core.lib libeay32.lib ssleay32.lib libbmdpr.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libbmdutils.pdb" /debug /machine:I386 /def:".\libbmdutils.def" /out:"$(OUTDIR)\libbmdutils.dll" /implib:"$(OUTDIR)\libbmdutils.lib" /pdbtype:sept /libpath:"../../libs" /libpath:"../../../pcre/Debug" /libpath:"../../../CryptLib/binaries" /libpath:"../../../openssl/out32dll" /libpath:"../../Debug" 
DEF_FILE= \
	".\libbmdutils.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdutils.obj"

"$(OUTDIR)\libbmdutils.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("libbmdutils.dep")
!INCLUDE "libbmdutils.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmdutils.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmdutils - Win32 Release" || "$(CFG)" == "libbmdutils - Win32 Debug"
SOURCE=.\libbmdutils.c

"$(INTDIR)\libbmdutils.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


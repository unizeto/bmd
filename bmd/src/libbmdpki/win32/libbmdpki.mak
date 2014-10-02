# Microsoft Developer Studio Generated NMAKE File, Based on libbmdpki.dsp
!IF "$(CFG)" == ""
CFG=libbmdpki - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmdpki - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmdpki - Win32 Release" && "$(CFG)" != "libbmdpki - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdpki.mak" CFG="libbmdpki - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdpki - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libbmdpki - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "libbmdpki - Win32 Release"

OUTDIR=.\../../Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../../Release
# End Custom Macros

ALL : "$(OUTDIR)\libbmdpki.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdpki.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmdpki.dll"
	-@erase "$(OUTDIR)\libbmdpki.exp"
	-@erase "$(OUTDIR)\libbmdpki.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /O2 /I "../../include/libbmdpki" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdasn1_core" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../include/libbmdpkcs12" /I "../../include/libbmddialogs" /I "../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDPKI_EXPORTS" /D "INC_CHILD" /Fp"$(INTDIR)\libbmdpki.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdpki.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libeay32.lib ssleay32.lib libbmddialogs.lib libbmdpkcs12.lib libcurl.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\libbmdpki.pdb" /machine:I386 /def:".\libbmdpki.def" /out:"$(OUTDIR)\libbmdpki.dll" /implib:"$(OUTDIR)\libbmdpki.lib" /libpath:"../../../CryptLib/binaries" /libpath:"../../../openssl/out32dll" /libpath:"../../Release" /libpath:"../../libs" 
DEF_FILE= \
	".\libbmdpki.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdpki.obj"

"$(OUTDIR)\libbmdpki.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmdpki - Win32 Debug"

OUTDIR=.\../../Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\../../Debug
# End Custom Macros

ALL : "$(OUTDIR)\libbmdpki.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdpki.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmdpki.dll"
	-@erase "$(OUTDIR)\libbmdpki.exp"
	-@erase "$(OUTDIR)\libbmdpki.ilk"
	-@erase "$(OUTDIR)\libbmdpki.lib"
	-@erase "$(OUTDIR)\libbmdpki.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "../.." /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdasn1_core" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../include/libbmdpkcs12" /I "../../include/libbmddialogs" /I "../../include/libbmdpki" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDPKI_EXPORTS" /D "INC_CHILD" /Fp"$(INTDIR)\libbmdpki.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdpki.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=cl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libeay32.lib ssleay32.lib libbmddialogs.lib libbmdpkcs12.lib libcurl.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libbmdpki.pdb" /debug /machine:I386 /def:".\libbmdpki.def" /out:"$(OUTDIR)\libbmdpki.dll" /implib:"$(OUTDIR)\libbmdpki.lib" /pdbtype:sept /libpath:"../../libs" /libpath:"../../../CryptLib/binaries" /libpath:"../../../openssl/out32dll" /libpath:"../../Debug" 
DEF_FILE= \
	".\libbmdpki.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdpki.obj"

"$(OUTDIR)\libbmdpki.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("libbmdpki.dep")
!INCLUDE "libbmdpki.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmdpki.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmdpki - Win32 Release" || "$(CFG)" == "libbmdpki - Win32 Debug"
SOURCE=.\libbmdpki.c

"$(INTDIR)\libbmdpki.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


# Microsoft Developer Studio Generated NMAKE File, Based on libbmdpkcs11.dsp
!IF "$(CFG)" == ""
CFG=libbmdpkcs11 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmdpkcs11 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmdpkcs11 - Win32 Release" && "$(CFG)" != "libbmdpkcs11 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdpkcs11.mak" CFG="libbmdpkcs11 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdpkcs11 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libbmdpkcs11 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "libbmdpkcs11 - Win32 Release"

OUTDIR=.\../../Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../../Release
# End Custom Macros

ALL : "$(OUTDIR)\libbmdpkcs11.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdpkcs11.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmdpkcs11.dll"
	-@erase "$(OUTDIR)\libbmdpkcs11.exp"
	-@erase "$(OUTDIR)\libbmdpkcs11.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /O2 /I "../../include/libbmdpkcs11" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdasn1_core" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../.." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDPKCS11_EXPORTS" /D "INC_CHILD" /Fp"$(INTDIR)\libbmdpkcs11.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdpkcs11.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libeay32.lib ssleay32.lib cl32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\libbmdpkcs11.pdb" /machine:I386 /def:".\libbmdpkcs11.def" /out:"$(OUTDIR)\libbmdpkcs11.dll" /implib:"$(OUTDIR)\libbmdpkcs11.lib" /libpath:"../../../openssl/out32dll" /libpath:"../../../CryptLib/binaries" /libpath:"../../libs" 
DEF_FILE= \
	".\libbmdpkcs11.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdpkcs11.obj"

"$(OUTDIR)\libbmdpkcs11.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmdpkcs11 - Win32 Debug"

OUTDIR=.\../../Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\../../Debug
# End Custom Macros

ALL : "$(OUTDIR)\libbmdpkcs11.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdpkcs11.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmdpkcs11.dll"
	-@erase "$(OUTDIR)\libbmdpkcs11.exp"
	-@erase "$(OUTDIR)\libbmdpkcs11.ilk"
	-@erase "$(OUTDIR)\libbmdpkcs11.lib"
	-@erase "$(OUTDIR)\libbmdpkcs11.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "../.." /I "../../include/libbmdpkcs11" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdasn1_core" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDPKCS11_EXPORTS" /D "INC_CHILD" /Fp"$(INTDIR)\libbmdpkcs11.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdpkcs11.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libeay32.lib ssleay32.lib cl32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libbmdpkcs11.pdb" /debug /machine:I386 /def:".\libbmdpkcs11.def" /out:"$(OUTDIR)\libbmdpkcs11.dll" /implib:"$(OUTDIR)\libbmdpkcs11.lib" /pdbtype:sept /libpath:"../../libs" /libpath:"../../../openssl/out32dll" /libpath:"../../../CryptLib/binaries" 
DEF_FILE= \
	".\libbmdpkcs11.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdpkcs11.obj"

"$(OUTDIR)\libbmdpkcs11.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("libbmdpkcs11.dep")
!INCLUDE "libbmdpkcs11.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmdpkcs11.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmdpkcs11 - Win32 Release" || "$(CFG)" == "libbmdpkcs11 - Win32 Debug"
SOURCE=.\libbmdpkcs11.c

"$(INTDIR)\libbmdpkcs11.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


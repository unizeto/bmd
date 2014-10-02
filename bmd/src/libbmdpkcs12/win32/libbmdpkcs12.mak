# Microsoft Developer Studio Generated NMAKE File, Based on libbmdpkcs12.dsp
!IF "$(CFG)" == ""
CFG=libbmdpkcs12 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmdpkcs12 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmdpkcs12 - Win32 Release" && "$(CFG)" != "libbmdpkcs12 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdpkcs12.mak" CFG="libbmdpkcs12 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdpkcs12 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libbmdpkcs12 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "libbmdpkcs12 - Win32 Release"

OUTDIR=.\../../Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../../Release
# End Custom Macros

ALL : "$(OUTDIR)\libbmdpkcs12.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdpkcs12.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmdpkcs12.dll"
	-@erase "$(OUTDIR)\libbmdpkcs12.exp"
	-@erase "$(OUTDIR)\libbmdpkcs12.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /O2 /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdasn1_core" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../include/libbmdpkcs12" /I "../../include/libbmddialogs" /I "../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDPKCS12_EXPORTS" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdpkcs12.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libeay32.lib ssleay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libbmddialogs.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\libbmdpkcs12.pdb" /machine:I386 /def:".\libbmdpkcs12.def" /out:"$(OUTDIR)\libbmdpkcs12.dll" /implib:"$(OUTDIR)\libbmdpkcs12.lib" /libpath:"../../../openssl/out32dll" /libpath:"../../Release" /libpath:"../../libs" 
DEF_FILE= \
	".\libbmdpkcs12.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdpkcs12.obj"

"$(OUTDIR)\libbmdpkcs12.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmdpkcs12 - Win32 Debug"

OUTDIR=.\../../Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\../../Debug
# End Custom Macros

ALL : "$(OUTDIR)\libbmdpkcs12.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmdpkcs12.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmdpkcs12.dll"
	-@erase "$(OUTDIR)\libbmdpkcs12.exp"
	-@erase "$(OUTDIR)\libbmdpkcs12.ilk"
	-@erase "$(OUTDIR)\libbmdpkcs12.lib"
	-@erase "$(OUTDIR)\libbmdpkcs12.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "../.." /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdasn1_core" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../include/libbmdpkcs12" /I "../../include/libbmddialogs" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDPKCS12_EXPORTS" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdpkcs12.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libeay32.lib ssleay32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libbmddialogs.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libbmdpkcs12.pdb" /debug /machine:I386 /def:".\libbmdpkcs12.def" /out:"$(OUTDIR)\libbmdpkcs12.dll" /implib:"$(OUTDIR)\libbmdpkcs12.lib" /pdbtype:sept /libpath:"../../libs" /libpath:"../../../openssl/out32dll" /libpath:"../../Debug" 
DEF_FILE= \
	".\libbmdpkcs12.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmdpkcs12.obj"

"$(OUTDIR)\libbmdpkcs12.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("libbmdpkcs12.dep")
!INCLUDE "libbmdpkcs12.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmdpkcs12.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmdpkcs12 - Win32 Release" || "$(CFG)" == "libbmdpkcs12 - Win32 Debug"
SOURCE=.\libbmdpkcs12.c

"$(INTDIR)\libbmdpkcs12.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


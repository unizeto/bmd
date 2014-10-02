# Microsoft Developer Studio Generated NMAKE File, Based on libbmdutils_static.dsp
!IF "$(CFG)" == ""
CFG=libbmdutils_static - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmdutils_static - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmdutils_static - Win32 Release" && "$(CFG)" != "libbmdutils_static - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdutils_static - Win32 Release"

OUTDIR=.\../../Release_static
INTDIR=.\Release_static
# Begin Custom Macros
OutDir=.\../../Release_static
# End Custom Macros

ALL : "$(OUTDIR)\libbmdutils.lib"


CLEAN :
	-@erase "$(INTDIR)\libbmdutils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmdutils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /O2 /I "../../include/libbmdutils" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdasn1_core" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdpr/prlib-common" /I "../../include/libbmdpr/bmd_req/asn1clib" /I "../../include/libbmdpr/bmd_resp/asn1clib" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdutils_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libbmdutils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\libbmdutils.obj"

"$(OUTDIR)\libbmdutils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmdutils_static - Win32 Debug"

OUTDIR=.\../../Debug_static
INTDIR=.\Debug_static
# Begin Custom Macros
OutDir=.\../../Debug_static
# End Custom Macros

ALL : "$(OUTDIR)\libbmdutils.lib"


CLEAN :
	-@erase "$(INTDIR)\libbmdutils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmdutils.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /ZI /Od /I "../../include/libbmdutils" /I "../../include/common" /I "../../../openssl/inc32" /I "../../../CryptLib" /I "../../../pcre" /I "../../../muscleframework/libmusclepkcs11/include" /I "../../../curl/include" /I "../../include/libbmdasn1_core" /I "../../include/libbmdaa/asn1" /I "../../include/libbmdpr/prlib-common" /I "../../include/libbmdpr/bmd_req/asn1clib" /I "../../include/libbmdpr/bmd_resp/asn1clib" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmdutils_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libbmdutils.lib" 
LIB32_OBJS= \
	"$(INTDIR)\libbmdutils.obj"

"$(OUTDIR)\libbmdutils.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("libbmdutils_static.dep")
!INCLUDE "libbmdutils_static.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmdutils_static.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmdutils_static - Win32 Release" || "$(CFG)" == "libbmdutils_static - Win32 Debug"
SOURCE=.\libbmdutils.c

"$(INTDIR)\libbmdutils.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


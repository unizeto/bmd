# Microsoft Developer Studio Generated NMAKE File, Based on libbmddialogs_static.dsp
!IF "$(CFG)" == ""
CFG=libbmddialogs_static - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmddialogs_static - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmddialogs_static - Win32 Release" && "$(CFG)" != "libbmddialogs_static - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmddialogs_static.mak" CFG="libbmddialogs_static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmddialogs_static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libbmddialogs_static - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "libbmddialogs_static - Win32 Release"

OUTDIR=.\../../Release_static
INTDIR=.\Release_static
# Begin Custom Macros
OutDir=.\../../Release_static
# End Custom Macros

ALL : "$(OUTDIR)\libbmddialogs.lib"


CLEAN :
	-@erase "$(INTDIR)\libbmddialogs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmddialogs.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /O2 /I "../../../wxWidgets/include" /I "../../../wxWidgets/include/msvc" /I "../../include/libbmddialogs" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmddialogs_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libbmddialogs.lib" 
LIB32_OBJS= \
	"$(INTDIR)\libbmddialogs.obj"

"$(OUTDIR)\libbmddialogs.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmddialogs_static - Win32 Debug"

OUTDIR=.\../../Debug_static
INTDIR=.\Debug_static
# Begin Custom Macros
OutDir=.\../../Debug_static
# End Custom Macros

ALL : "$(OUTDIR)\libbmddialogs.lib"


CLEAN :
	-@erase "$(INTDIR)\libbmddialogs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmddialogs.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /ZI /Od /I "../../../wxWidgets/include" /I "../../../wxWidgets/include/msvc" /I "../../include/libbmddialogs" /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "INC_CHILD" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmddialogs_static.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\libbmddialogs.lib" 
LIB32_OBJS= \
	"$(INTDIR)\libbmddialogs.obj"

"$(OUTDIR)\libbmddialogs.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("libbmddialogs_static.dep")
!INCLUDE "libbmddialogs_static.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmddialogs_static.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmddialogs_static - Win32 Release" || "$(CFG)" == "libbmddialogs_static - Win32 Debug"
SOURCE=.\libbmddialogs.cpp

"$(INTDIR)\libbmddialogs.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


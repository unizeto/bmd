# Microsoft Developer Studio Generated NMAKE File, Based on libbmddialogs.dsp
!IF "$(CFG)" == ""
CFG=libbmddialogs - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbmddialogs - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbmddialogs - Win32 Release" && "$(CFG)" != "libbmddialogs - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmddialogs.mak" CFG="libbmddialogs - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmddialogs - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libbmddialogs - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "libbmddialogs - Win32 Release"

OUTDIR=.\../../Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\../../Release
# End Custom Macros

ALL : "$(OUTDIR)\libbmddialogs.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmddialogs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\libbmddialogs.dll"
	-@erase "$(OUTDIR)\libbmddialogs.exp"
	-@erase "$(OUTDIR)\libbmddialogs.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /O2 /I "../../../wxWidgets/include" /I "../../../wxWidgets/include/msvc" /I "../../include/libbmddialogs" /I "../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDDIALOGS_EXPORTS" /D "WXUSINGDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmddialogs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wxmsw24.lib comctl32.lib rpcrt4.lib winmm.lib advapi32.lib wsock32.lib png.lib zlib.lib jpeg.lib tiff.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\libbmddialogs.pdb" /machine:I386 /def:".\libbmddialogs.def" /out:"$(OUTDIR)\libbmddialogs.dll" /implib:"$(OUTDIR)\libbmddialogs.lib" /libpath:"../../libs" /libpath:"../../../wxWidgets/lib/vc_lib" 
DEF_FILE= \
	".\libbmddialogs.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmddialogs.obj"

"$(OUTDIR)\libbmddialogs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbmddialogs - Win32 Debug"

OUTDIR=.\../../Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\../../Debug
# End Custom Macros

ALL : "$(OUTDIR)\libbmddialogs.dll"


CLEAN :
	-@erase "$(INTDIR)\libbmddialogs.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\libbmddialogs.dll"
	-@erase "$(OUTDIR)\libbmddialogs.exp"
	-@erase "$(OUTDIR)\libbmddialogs.ilk"
	-@erase "$(OUTDIR)\libbmddialogs.lib"
	-@erase "$(OUTDIR)\libbmddialogs.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "../.." /I "../../../wxWidgets/include/msvc" /I "../../include/libbmddialogs" /I "../../../wxWidgets/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LIBBMDDIALOGS_EXPORTS" /D "WXUSINGDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbmddialogs.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wxmsw24d.lib winmm.lib advapi32.lib wsock32.lib pngd.lib zlibd.lib jpegd.lib tiffd.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\libbmddialogs.pdb" /debug /machine:I386 /def:".\libbmddialogs.def" /out:"$(OUTDIR)\libbmddialogs.dll" /implib:"$(OUTDIR)\libbmddialogs.lib" /pdbtype:sept /libpath:"../../libs" /libpath:"../../../wxWidgets/lib/vc_lib" 
DEF_FILE= \
	".\libbmddialogs.def"
LINK32_OBJS= \
	"$(INTDIR)\libbmddialogs.obj"

"$(OUTDIR)\libbmddialogs.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("libbmddialogs.dep")
!INCLUDE "libbmddialogs.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbmddialogs.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbmddialogs - Win32 Release" || "$(CFG)" == "libbmddialogs - Win32 Debug"
SOURCE=.\libbmddialogs.cpp

"$(INTDIR)\libbmddialogs.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 


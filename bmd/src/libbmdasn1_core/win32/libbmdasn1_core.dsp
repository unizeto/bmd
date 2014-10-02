# Microsoft Developer Studio Project File - Name="libbmdasn1_core" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libbmdasn1_core - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbmdasn1_core.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbmdasn1_core.mak" CFG="libbmdasn1_core - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbmdasn1_core - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libbmdasn1_core - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbmdasn1_core - Win32 Release"

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

!ELSEIF  "$(CFG)" == "libbmdasn1_core - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../exe_trunk"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
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

# Name "libbmdasn1_core - Win32 Release"
# Name "libbmdasn1_core - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\ANY.c
# End Source File
# Begin Source File

SOURCE=..\asn_codecs_prim.c
# End Source File
# Begin Source File

SOURCE=..\asn_SEQUENCE_OF.c
# End Source File
# Begin Source File

SOURCE=..\asn_SET_OF.c
# End Source File
# Begin Source File

SOURCE=..\ber_decoder.c
# End Source File
# Begin Source File

SOURCE=..\ber_tlv_length.c
# End Source File
# Begin Source File

SOURCE=..\ber_tlv_tag.c
# End Source File
# Begin Source File

SOURCE=..\BIT_STRING.c
# End Source File
# Begin Source File

SOURCE=..\BMPString.c
# End Source File
# Begin Source File

SOURCE=..\BOOLEAN.c
# End Source File
# Begin Source File

SOURCE=..\constr_CHOICE.c
# End Source File
# Begin Source File

SOURCE=..\constr_SEQUENCE.c
# End Source File
# Begin Source File

SOURCE=..\constr_SEQUENCE_OF.c
# End Source File
# Begin Source File

SOURCE=..\constr_SET.c
# End Source File
# Begin Source File

SOURCE=..\constr_SET_OF.c
# End Source File
# Begin Source File

SOURCE=..\constr_TYPE.c
# End Source File
# Begin Source File

SOURCE=..\constraints.c
# End Source File
# Begin Source File

SOURCE=..\der_encoder.c
# End Source File
# Begin Source File

SOURCE=..\ENUMERATED.c
# End Source File
# Begin Source File

SOURCE=..\GeneralizedTime.c
# End Source File
# Begin Source File

SOURCE=..\GeneralString.c
# End Source File
# Begin Source File

SOURCE=..\GraphicString.c
# End Source File
# Begin Source File

SOURCE=..\IA5String.c
# End Source File
# Begin Source File

SOURCE=..\INTEGER.c
# End Source File
# Begin Source File

SOURCE=..\ISO646String.c
# End Source File
# Begin Source File

SOURCE=.\libbmdasn1_core.def
# End Source File
# Begin Source File

SOURCE=..\linuxwindows.c
# End Source File
# Begin Source File

SOURCE=..\NativeEnumerated.c
# End Source File
# Begin Source File

SOURCE=..\NativeInteger.c
# End Source File
# Begin Source File

SOURCE=..\NativeReal.c
# End Source File
# Begin Source File

SOURCE=..\NULL.c
# End Source File
# Begin Source File

SOURCE=..\NumericString.c
# End Source File
# Begin Source File

SOURCE=..\OBJECT_IDENTIFIER.c
# End Source File
# Begin Source File

SOURCE=..\ObjectDescriptor.c
# End Source File
# Begin Source File

SOURCE=..\OCTET_STRING.c
# End Source File
# Begin Source File

SOURCE=..\per_decoder.c
# End Source File
# Begin Source File

SOURCE=..\per_support.c
# End Source File
# Begin Source File

SOURCE=..\PrintableString.c
# End Source File
# Begin Source File

SOURCE=..\REAL.c
# End Source File
# Begin Source File

SOURCE="..\RELATIVE-OID.c"
# End Source File
# Begin Source File

SOURCE=..\T61String.c
# End Source File
# Begin Source File

SOURCE=..\TeletexString.c
# End Source File
# Begin Source File

SOURCE=..\UniversalString.c
# End Source File
# Begin Source File

SOURCE=..\UTCTime.c
# End Source File
# Begin Source File

SOURCE=..\UTF8String.c
# End Source File
# Begin Source File

SOURCE=..\VideotexString.c
# End Source File
# Begin Source File

SOURCE=..\VisibleString.c
# End Source File
# Begin Source File

SOURCE=..\xer_decoder.c
# End Source File
# Begin Source File

SOURCE=..\xer_encoder.c
# End Source File
# Begin Source File

SOURCE=..\xer_support.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ANY.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_application.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_codecs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_codecs_prim.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_internal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_SEQUENCE_OF.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_SET_OF.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\asn_system.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ber_decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ber_tlv_length.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ber_tlv_tag.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\BIT_STRING.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\BMPString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\BOOLEAN.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constr_CHOICE.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constr_SEQUENCE.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constr_SEQUENCE_OF.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constr_SET.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constr_SET_OF.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constr_TYPE.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\constraints.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\der_encoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ENUMERATED.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\GeneralizedTime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\GeneralString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\GraphicString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\IA5String.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\INTEGER.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ISO646String.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\linuxwindows.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\NativeEnumerated.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\NativeInteger.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\NativeReal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\NULL.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\NumericString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\OBJECT_IDENTIFIER.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\ObjectDescriptor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\OCTET_STRING.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\per_decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\per_support.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\PrintableString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\REAL.h
# End Source File
# Begin Source File

SOURCE="..\..\..\include\bmd\libbmdasn1_core\RELATIVE-OID.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\T61String.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\TeletexString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\UniversalString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\UTCTime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\UTF8String.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\VideotexString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\VisibleString.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\xer_decoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\xer_encoder.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\bmd\libbmdasn1_core\xer_support.h
# End Source File
# End Group
# End Target
# End Project

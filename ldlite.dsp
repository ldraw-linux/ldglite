# Microsoft Developer Studio Project File - Name="ldlite" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ldlite - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ldlite.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ldlite.mak" CFG="ldlite - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ldlite - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ldlite - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ldlite", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ldlite - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /Z<none> /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /profile /machine:I386
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ldlite - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Zp1 /MTd /W3 /Gm /GX /ZI /Od /Ob0 /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "ldlite - Win32 Release"
# Name "ldlite - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dibkernel.cpp
# End Source File
# Begin Source File

SOURCE=.\ldlite.cpp
# End Source File
# Begin Source File

SOURCE=.\ldlite.rc
# End Source File
# Begin Source File

SOURCE=.\ldliteCommandLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ldliteDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ldliteView.cpp
# End Source File
# Begin Source File

SOURCE=.\ldliteVR_main.c
# End Source File
# Begin Source File

SOURCE=.\lex.yy.c
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\qbuf.c
# End Source File
# Begin Source File

SOURCE=.\ScaleDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stub.c

!IF  "$(CFG)" == "ldlite - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "ldlite - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\y.tab.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dibkernel.h
# End Source File
# Begin Source File

SOURCE=.\ldlite.h
# End Source File
# Begin Source File

SOURCE=.\ldliteCommandLineInfo.h
# End Source File
# Begin Source File

SOURCE=.\ldliteDoc.h
# End Source File
# Begin Source File

SOURCE=.\ldliteView.h
# End Source File
# Begin Source File

SOURCE=.\ldliteVR.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDialog.h
# End Source File
# Begin Source File

SOURCE=.\qbuf.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScaleDialog.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\y.tab.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\ldlite.ico
# End Source File
# Begin Source File

SOURCE=.\res\ldlite.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ldliteDoc.ico
# End Source File
# End Group
# Begin Source File

SOURCE=".\ldl-logo.dat"
# End Source File
# Begin Source File

SOURCE=.\ldlite.l
# End Source File
# Begin Source File

SOURCE=.\ldlite.y
# End Source File
# Begin Source File

SOURCE=.\notes.txt
# End Source File
# Begin Source File

SOURCE=..\..\ldraw\models\wood2.dat
# End Source File
# End Target
# End Project

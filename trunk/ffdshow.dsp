# Microsoft Developer Studio Project File - Name="ffdshow" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ffdshow - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ffdshow.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ffdshow.mak" CFG="ffdshow - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ffdshow - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ffdshow - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /GX /O2 /Ob2 /I "src" /I "src\settings" /I "src\dialog" /I "src\imgFilters" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "ARCH_X86" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib msvcrt.lib advapi32.lib winmm.lib ole32.lib uuid.lib strmbase.lib oleaut32.lib comctl32.lib gdi32.lib shell32.lib comdlg32.lib libcpmt.lib /nologo /entry:"DllEntryPoint@12" /dll /pdb:none /machine:I386 /nodefaultlib /out:"bin\ffdshow.ax"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /w /W0 /Gm /Gi /GX /ZI /Od /I "src" /I "src\settings" /I "src\dialog" /I "src\imgFilters" /D "DEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "ARCH_X86" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib msvcrtd.lib advapi32.lib winmm.lib ole32.lib uuid.lib strmbasd.lib oleaut32.lib comctl32.lib gdi32.lib shell32.lib comdlg32.lib libcpmtd.lib /nologo /entry:"DllEntryPoint@12" /dll /debug /machine:I386 /nodefaultlib /out:"bin\ffdshow.ax" /pdbtype:sept
# SUBTRACT LINK32 /profile /pdb:none

!ENDIF 

# Begin Target

# Name "ffdshow - Win32 Release"
# Name "ffdshow - Win32 Debug"
# Begin Group "doc"

# PROP Default_Filter ""
# Begin Group "help"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\helpAspectNcrop.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpBlur.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpCodecs.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpFont.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpInfo.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpMisc.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpNoise.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpPictprop.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpPostproc.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpResize.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpSharpen.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpSubtitles.txt
# End Source File
# Begin Source File

SOURCE=.\src\helpTray.txt
# End Source File
# End Group
# Begin Source File

SOURCE=.\authors.txt
# End Source File
# Begin Source File

SOURCE=.\copying.txt
# End Source File
# Begin Source File

SOURCE=.\konfiguracia.txt
# End Source File
# Begin Source File

SOURCE=.\src\readme.txt
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\xvid\utils\mem_align.c
# End Source File
# Begin Source File

SOURCE=.\src\Tconfig.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TcpuUsage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TffDecoder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Tlibavcodec.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Tpostproc.cpp

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\trayIcon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\idct\xIdctref.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\ffdebug.h
# End Source File
# Begin Source File

SOURCE=.\src\IffDecoder.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\utils\mem_align.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\Tconfig.h
# End Source File
# Begin Source File

SOURCE=.\src\TcpuUsage.h
# End Source File
# Begin Source File

SOURCE=.\src\Tdll.h
# End Source File
# Begin Source File

SOURCE=.\src\TffDecoder.h
# End Source File
# Begin Source File

SOURCE=.\src\Tlibavcodec.h
# End Source File
# Begin Source File

SOURCE=.\src\Tpostproc.h
# End Source File
# Begin Source File

SOURCE=.\src\trayIcon.h
# End Source File
# Begin Source File

SOURCE=.\src\TresizeCtx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\src\arrows.bmp
# End Source File
# Begin Source File

SOURCE=.\src\arrows_mask_d.bmp
# End Source File
# Begin Source File

SOURCE=.\src\arrows_mask_u.bmp
# End Source File
# Begin Source File

SOURCE=.\src\arrows_mask_ud.bmp
# End Source File
# Begin Source File

SOURCE=.\src\checked.bmp
# End Source File
# Begin Source File

SOURCE=.\src\empty.bmp
# End Source File
# Begin Source File

SOURCE=.\src\ffdshow.ico
# End Source File
# Begin Source File

SOURCE=.\src\ffdshow.rc
# End Source File
# Begin Source File

SOURCE=.\src\mask.bmp
# End Source File
# End Group
# Begin Group "Linker Defs"

# PROP Default_Filter "def"
# Begin Source File

SOURCE=.\src\ffdshow.def
# End Source File
# End Group
# Begin Group "subtitles"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\subtitles\subreader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\subtitles\subreader.h
# End Source File
# Begin Source File

SOURCE=.\src\subtitles\Tfont.cpp

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# ADD CPP /FAs

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\subtitles\Tfont.h
# End Source File
# Begin Source File

SOURCE=.\src\subtitles\Tsubtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\src\subtitles\Tsubtitles.h
# End Source File
# End Group
# Begin Group "xvid"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\xvid\image\colorspace.c
# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\colorspace.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\utils\x86_asm\cpuid.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\utils\x86_asm\cpuid.asm
InputName=cpuid

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\utils\x86_asm\cpuid.asm
InputName=cpuid

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\utils\emms.c
# End Source File
# Begin Source File

SOURCE=.\src\xvid\utils\emms.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\dct\idct.c
# End Source File
# Begin Source File

SOURCE=.\src\xvid\dct\idct.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\dct\x86_asm\idct_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\dct\x86_asm\idct_mmx.asm
InputName=idct_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\image.c
# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\image.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\portab.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\x86_asm\rgb_to_yv12_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\image\x86_asm\rgb_to_yv12_mmx.asm
InputName=rgb_to_yv12_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\image\x86_asm\rgb_to_yv12_mmx.asm
InputName=rgb_to_yv12_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\xvid.c
# End Source File
# Begin Source File

SOURCE=.\src\xvid\xvid.h
# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\x86_asm\yuv_to_yv12_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\image\x86_asm\yuv_to_yv12_mmx.asm
InputName=yuv_to_yv12_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\image\x86_asm\yuv_to_yv12_mmx.asm
InputName=yuv_to_yv12_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\x86_asm\yuyv_to_yv12_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\image\x86_asm\yuyv_to_yv12_mmx.asm
InputName=yuyv_to_yv12_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\image\x86_asm\yuyv_to_yv12_mmx.asm
InputName=yuyv_to_yv12_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\x86_asm\yv12_to_rgb24_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\image\x86_asm\yv12_to_rgb24_mmx.asm
InputName=yv12_to_rgb24_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\image\x86_asm\yv12_to_rgb24_mmx.asm
InputName=yv12_to_rgb24_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\x86_asm\yv12_to_rgb32_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\image\x86_asm\yv12_to_rgb32_mmx.asm
InputName=yv12_to_rgb32_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\image\x86_asm\yv12_to_rgb32_mmx.asm
InputName=yv12_to_rgb32_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\xvid\image\x86_asm\yv12_to_yuyv_mmx.asm

!IF  "$(CFG)" == "ffdshow - Win32 Release"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Release
InputPath=.\src\xvid\image\x86_asm\yv12_to_yuyv_mmx.asm
InputName=yv12_to_yuyv_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "ffdshow - Win32 Debug"

# Begin Custom Build - Assembling $(InputPath)
IntDir=.\Debug
InputPath=.\src\xvid\image\x86_asm\yv12_to_yuyv_mmx.asm
InputName=yv12_to_yuyv_mmx

"$(IntDir)\$(InputName).obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	nasm -f win32 -DPREFIX -o $(IntDir)\$(InputName).obj $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "imgFilters"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\imgFilters\noise_avih_template.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\noise_template.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\noise_template_mmx.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilter.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterBlur.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterBlur.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterChroma.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterChroma.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterLuma.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterLuma.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterNoise.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilters.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilters.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterSharpen.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterSharpen.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterSubtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TimgFilterSubtitles.h
# End Source File
# Begin Source File

SOURCE=.\src\imgFilters\TtempPicture.h
# End Source File
# End Group
# Begin Group "dialog"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\dialog\Cabout.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cabout.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\CaspectNcrop.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\CaspectNcrop.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cblur.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cblur.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Ccodecs.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Ccodecs.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cfont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cfont.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cinfo.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cmisc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cmisc.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cnoise.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cnoise.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\CpictProp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\CpictProp.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\CpostProc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\CpostProc.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cpresets.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cpresets.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cresize.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Cresize.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Csharpen.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Csharpen.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Csubtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Csubtitles.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Ctray.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\Ctray.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\TconfPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\TconfPage.h
# End Source File
# Begin Source File

SOURCE=.\src\dialog\TffdshowPage.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dialog\TffdshowPage.h
# End Source File
# End Group
# Begin Group "settings"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\settings\presets_template.h
# End Source File
# Begin Source File

SOURCE=.\src\settings\reg.h
# End Source File
# Begin Source File

SOURCE=.\src\settings\TdialogSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\src\settings\TdialogSettings.h
# End Source File
# Begin Source File

SOURCE=.\src\settings\TglobalSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\src\settings\TglobalSettings.h
# End Source File
# Begin Source File

SOURCE=.\src\settings\Tpresets.cpp
# End Source File
# Begin Source File

SOURCE=.\src\settings\Tpresets.h
# End Source File
# Begin Source File

SOURCE=.\src\settings\TpresetSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\src\settings\TpresetSettings.h
# End Source File
# End Group
# End Target
# End Project

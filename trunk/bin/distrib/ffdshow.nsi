; ffdshow.nsi
;
; This script is based on example2.nsi.
;

; The name of the installer
Name "ffdshow"
Caption "ffdshow"

;InstType Typical
Icon "..\..\src\ffdshow.ico"

!define FFDATE "20020704"

; The file to write
OutFile "ffdshow-${FFDATE}.exe"

BrandingText "ffdshow build ${FFDATE}"

LicenseText "This will install ffdshow MPEG4 decoder. Please read the license terms below before installing."
LicenseData "..\..\copying.txt"

; The default installation directory
InstallDir $PROGRAMFILES\ffdshow
; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\GNU\ffdshow "pth"

; The text to prompt the user to enter a directory
ComponentText "This will install ffdhow DirectShow filter on your computer."
; The text to prompt the user to enter a directory
DirText "Choose a directory to install in to:"

Function .onInit
  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "xvid"
  IfErrors xvidMore
  IntCmp $0 0 xvidEqual xvidLess 
 xvidMore:
  SectionSetFlags 4 0x80000000
  Goto xvidEnd
 xvidEqual:
 xvidLess:
  SectionSetFlags 4 0
 xvidEnd:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "div3"
  IfErrors div3More
  IntCmp $0 0 div3Equal div3Less 
 div3More:
  SectionSetFlags 5 0x80000000
  Goto div3End
 div3Equal:
 div3Less:
  SectionSetFlags 5 0
 div3End:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "divx"
  IfErrors divxMore
  IntCmp $0 0 divxEqual divxLess 
 divxMore:
  SectionSetFlags 6 0x80000000
  Goto divxEnd
 divxEqual:
 divxLess:
  SectionSetFlags 6 0
 divxEnd:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "dx50"
  IfErrors dx50More
  IntCmp $0 0 dx50Equal dx50Less 
 dx50More:
  SectionSetFlags 7 0x80000000
  Goto dx50End
 dx50Equal:
 dx50Less:
  SectionSetFlags 7 0
 dx50End:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "mp43"
  IntCmp $0 0 mp43Equal mp43Less 
  SectionSetFlags 8 0x80000000
  Goto mp43End
 mp43Equal:
 mp43Less:
  SectionSetFlags 8 0
 mp43End:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "mp42"
  IntCmp $0 0 mp42Equal mp42Less 
  SectionSetFlags 9 0x80000000
  Goto mp42End
 mp42Equal:
 mp42Less:
  SectionSetFlags 9 0
 mp42End:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "mp41"
  IntCmp $0 0 mp41Equal mp41Less 
  SectionSetFlags 10 0x80000000
  Goto mp41End
 mp41Equal:
 mp41Less:
  SectionSetFlags 10 0
 mp41End:

  ReadRegDWORD $0 HKLM SOFTWARE\GNU\ffdshow "wmv1"
  IntCmp $0 0 wmv1Equal wmv1Less 
  SectionSetFlags 11 0x80000000
  Goto wmv1End
 wmv1Equal:
 wmv1Less:
  SectionSetFlags 11 0
 wmv1End:

FunctionEnd


; 0 - The stuff to install
Section "DirectShow Filter (required)"
  SectionIn 1
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "..\ffdshow.ax"
  File "..\..\src\ffmpeg\libavcodec.dll"
  UnRegDll $INSTDIR\ffdshow.ax
  RegDll $INSTDIR\ffdshow.ax
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\GNU\ffdshow "pth" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow" "DisplayName" "ffdshow (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow" "UninstallString" '"$INSTDIR\uninstall.exe"'

  WriteUninstaller "uninstall.exe"
  CreateDirectory "$SMPROGRAMS\ffdshow"
  CreateShortCut "$SMPROGRAMS\ffdshow\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\ffdshow\Configuration.lnk" "rundll32.exe" "ffdshow.ax,configure" "regedit.exe" 0
SectionEnd

; 1
Section "Postprocessing (recommended)"
  SectionIn 1
  File "..\..\src\mplayer\libpostproc.dll"
SectionEnd

; 2
Section "Documentation (optional)"
  SectionIn 1
  File "..\..\copying.txt"
  File "..\..\src\readme.txt"
  SetOutPath $INSTDIR\help
  File "..\help\ff.css"
  File "..\help\ffmpeg-button.gif"
  File "..\help\index.html"
  File "..\help\presets.html"
  File "..\help\compilation.html"
SectionEnd

; 3
SectionDivider "Supported codecs"

; 4
Section "XVID"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "xvid" 1
SectionEnd
; 5
Section "DIVX 3"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "div3" 1
SectionEnd
; 6
Section "DIVX 4"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "divx" 1
SectionEnd
; 7
Section "DIVX 5"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "dx50" 1
SectionEnd
; 8
Section "MS MPEG4v3"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "mp43" 1
SectionEnd
; 9
Section "MS MPEG4v2"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "mp42" 1
SectionEnd                                     
; 10
Section "MS MPEG4v1"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "mp41" 1
SectionEnd
; 11
Section "Windows Media Video 1"
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "wmv1" 1
SectionEnd

; uninstall stuff

UninstallText "This will uninstall ffdshow. Hit next to continue."

; special uninstall section.
Section "Uninstall"
  ; remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow"
  DeleteRegKey HKLM SOFTWARE\GNU\ffdshow
  DeleteRegKey HKCU SOFTWARE\GNU\ffdshow
  UnRegDll $INSTDIR\ffdshow.ax
  ; remove shortcuts, if any.
  Delete "$SMPROGRAMS\ffdshow\*.*"
  ; remove directories used.
  RMDir "$SMPROGRAMS\ffdshow"
  ; remove files
  RMDir "$INSTDIR\help"
  Delete "$INSTDIR\ffdshow.ax"
  Delete "$INSTDIR\libavcodec.dll"
  Delete "$INSTDIR\libavcodec_normal.dll"
  Delete "$INSTDIR\libavcodec_simple.dll"
  Delete "$INSTDIR\libpostproc.dll"
  Delete "$INSTDIR\copying.txt"
  Delete "$INSTDIR\readme.txt"
  ; MUST REMOVE UNINSTALLER, too
  Delete $INSTDIR\uninstall.exe
  RMDir "$INSTDIR"
SectionEnd

; eof

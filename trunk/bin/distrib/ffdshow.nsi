; ffdshow.nsi
;
; This script is based on example2.nsi.
;

; The name of the installer
Name "ffdshow"
Caption "ffdshow"

;InstType Typical
Icon "..\..\src\ffdshow.ico"

!define FFDATE "20020605"

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

; The stuff to install
Section "DirectShow Filter (required)"
  SectionIn 1
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
  File "..\ffdshow.ax"
  File "..\..\src\ffmpeg\libavcodec\libavcodec.dll"
  UnRegDll $INSTDIR\ffdshow.ax
  RegDll $INSTDIR\ffdshow.ax
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\GNU\ffdshow "pth" "$INSTDIR"
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow" "DisplayName" "ffdshow (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ffdshow" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "xvid" 1
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "div3" 1
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "divx" 1
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "dx50" 1
  WriteRegDWORD HKLM SOFTWARE\GNU\ffdshow "mp43" 1

  WriteUninstaller "uninstall.exe"
  CreateDirectory "$SMPROGRAMS\ffdshow"
  CreateShortCut "$SMPROGRAMS\ffdshow\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\ffdshow\Configuration.lnk" "rundll32.exe" "ffdshow.ax,configure" "regedit.exe" 0
SectionEnd

Section "Postprocessing (recommended)"
  SectionIn 1
  File "..\..\src\mplayer\libpostproc.dll"
SectionEnd

Section "Documentation (optional)"
  SectionIn 1
  File "..\..\copying.txt"
  File "..\..\src\readme.txt"
  SetOutPath $INSTDIR\help
  File "..\help\ff.css"
  File "..\help\ffmpeg-button.gif"
  File "..\help\index.html"
  File "..\help\presets.html"
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
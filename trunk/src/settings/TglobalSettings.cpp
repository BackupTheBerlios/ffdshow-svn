/*
 * Copyright (c) 2002 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <windows.h>
#pragma hdrstop
#include "Tconfig.h"
#include "TglobalSettings.h"
#include "TpresetSettings.h"
#include "reg.h"
#include "ffdshow_mediaguids.h"
#include "ffmpeg\libavcodec\avcodec.h"

void TglobalSettings::load(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 REG_GET_N("xvid",xvid,1);if (!config.isXviD && xvid==2) xvid=1;if (!config.isLibavcodec && config.isXviD && xvid) xvid=2;if (!config.isLibavcodec && !config.isXviD) xvid=0;
 REG_GET_N("div3",div3,0);if (!config.isLibavcodec) div3=0;
 REG_GET_N("divx",divx,0);if (!config.isXviD && divx==2) divx=1;if (!config.isLibavcodec && config.isXviD && divx) divx=2;if (!config.isLibavcodec && !config.isXviD) divx=0;
 REG_GET_N("dx50",dx50,0);if (!config.isXviD && dx50==2) dx50=1;if (!config.isLibavcodec && config.isXviD && dx50) dx50=2;if (!config.isLibavcodec && !config.isXviD) dx50=0;
 REG_GET_N("mp43",mp43,0);if (!config.isLibavcodec) mp43=0;
 REG_GET_N("mp42",mp42,0);if (!config.isLibavcodec) mp42=0;
 REG_GET_N("mp41",mp41,0);if (!config.isLibavcodec) mp41=0;
 REG_GET_N("h263",h263,0);if (!config.isLibavcodec) h263=0;
 REG_GET_N("wmv1",wmv1,0);if (!config.isLibavcodec) wmv1=0;
 REG_GET_N("rawv",rawv,0);
 RegCloseKey(hKey);
 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 REG_GET_N("autoPreset",autoPreset,0);
 REG_GET_N("trayIcon",trayIcon,0);
 REG_GET_N("autoPresetFileFirst",autoPresetFileFirst,0);
 REG_GET_S("activePreset",defaultPreset,FFPRESET_DEFAULT);
 REG_GET_N("showMV",isShowMV,0);
 RegCloseKey(hKey); 
}

void TglobalSettings::save(void)
{
 HKEY hKey;DWORD dispo;
 if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT "\\" FFDSHOW_REG_CHILD,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)==ERROR_SUCCESS)
  {
   REG_SET_N("xvid",xvid,0);
   REG_SET_N("div3",div3,0);
   REG_SET_N("divx",divx,0);
   REG_SET_N("dx50",dx50,0);
   REG_SET_N("mp43",mp43,0);
   REG_SET_N("mp42",mp42,0);
   REG_SET_N("mp41",mp41,0);
   REG_SET_N("h263",h263,0);
   REG_SET_N("wmv1",wmv1,0);
   REG_SET_N("rawv",rawv,0);
   RegCloseKey(hKey);
  } 
 if (RegCreateKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)==ERROR_SUCCESS)
  {
   REG_SET_N("autoPreset",autoPreset,0);
   REG_SET_N("autoPresetFileFirst",autoPresetFileFirst,0);
   REG_SET_N("trayIcon",trayIcon,0);
   REG_SET_S("activePreset",defaultPreset,"");
   REG_SET_N("showMV",isShowMV,0);
   RegCloseKey(hKey);
  }; 
}

#define FF_FOURCC(fourCC1,fourCC2,testVar,codecId)          \
 case FOURCC_##fourCC1:                                     \
 case FOURCC_##fourCC2:                                     \
  if (testVar)                                              \
   {                                                        \
    if (AVIfourCC)                                          \
     {                                                      \
      strcpy(AVIfourCC,#fourCC1);                           \
      if (testVar==2) strcat(AVIfourCC," (xvid)");          \
     }                                                      \
    return (testVar==2)?codecId+CODEC_ID_XVID_MASK:codecId; \
   }                                                        \
  else return CODEC_ID_NONE;
  
int TglobalSettings::codecSupported(DWORD fourCC,char *AVIfourCC)
{
 switch(fourCC)
  {
   FF_FOURCC(XVID,xvid,xvid,CODEC_ID_MPEG4)
   FF_FOURCC(DIVX,divx,divx,CODEC_ID_MPEG4)
   FF_FOURCC(DX50,dx50,dx50,CODEC_ID_MPEG4)
   FF_FOURCC(DIV3,div3,div3,CODEC_ID_MSMPEG4V3)
   FF_FOURCC(MP43,mp43,mp43,CODEC_ID_MSMPEG4V3);
   FF_FOURCC(MP42,mp42,mp42,CODEC_ID_MSMPEG4V2);
   FF_FOURCC(MP41,mp41,mp41,CODEC_ID_MSMPEG4V1);
   FF_FOURCC(H263,h263,h263,CODEC_ID_H263);
   FF_FOURCC(WMV1,wmv1,wmv1,CODEC_ID_WMV1);
   FF_FOURCC(YUY2,yuy2,rawv,CODEC_ID_YUY2);
   FF_FOURCC(RGB2,rgb2,rawv,CODEC_ID_RGB2);
   FF_FOURCC(RGB3,rgb3,rawv,CODEC_ID_RGB3);
   FF_FOURCC(RGB5,rgb5,rawv,CODEC_ID_RGB5);
   FF_FOURCC(RGB6,rgb6,rawv,CODEC_ID_RGB6);
   default:return CODEC_ID_NONE;
  }; 
}

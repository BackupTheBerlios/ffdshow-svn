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
#include "TglobalSettings.h"
#include "reg.h"

void TglobalSettings::load(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 REG_GET_N("xvid",xvid,1);
 REG_GET_N("div3",div3,0);
 REG_GET_N("divx",divx,0);
 REG_GET_N("dx50",dx50,0);
 REG_GET_N("mp43",mp43,0);
 REG_GET_N("mp42",mp42,0);
 REG_GET_N("mp41",mp41,0);
 REG_GET_N("h263",h263,0);
 RegCloseKey(hKey);
 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 REG_GET_N("autoPreset",autoPreset,0);
 REG_GET_N("trayIcon",trayIcon,0);
 REG_GET_N("autoPresetFileFirst",autoPresetFileFirst,0);
 REG_GET_S("activePreset",activePreset,FFPRESET_DEFAULT);
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
   RegCloseKey(hKey);
  } 
 if (RegCreateKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)==ERROR_SUCCESS)
  {
   REG_SET_N("autoPreset",autoPreset,0);
   REG_SET_N("autoPresetFileFirst",autoPresetFileFirst,0);
   REG_SET_N("trayIcon",trayIcon,0);
   REG_SET_S("activePreset",activePreset,"");
   RegCloseKey(hKey);
  }; 
}

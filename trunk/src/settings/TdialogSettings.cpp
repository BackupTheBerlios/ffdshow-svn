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
#include "reg.h"
#include "TdialogSettings.h"

void TdialogSettings::load(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 REG_GET_N("lastPage",lastPage,0);
 REG_GET_N("dlgRestorePos",dlgRestorePos,0);
 REG_GET_N("dlgPosX",dlgPosX,0);
 REG_GET_N("dlgPosY",dlgPosY,0);
 REG_GET_N("lvWidth0",lvWidth0,300);
 RegCloseKey(hKey); 
}

void TdialogSettings::save(void)
{
 HKEY hKey;DWORD dispo;
 if (RegCreateKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE,KEY_WRITE,0,&hKey,&dispo)==ERROR_SUCCESS)
  {
   REG_SET_N("lastPage",lastPage,0);
   REG_SET_N("dlgRestorePos",dlgRestorePos,0);
   REG_SET_N("dlgPosX",dlgPosX,0);
   REG_SET_N("dlgPosY",dlgPosY,0);
   REG_SET_N("lvWidth0",lvWidth0,0);
   RegCloseKey(hKey);
  }
}

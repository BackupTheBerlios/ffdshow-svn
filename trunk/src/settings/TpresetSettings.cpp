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
#include <stdio.h>
#include <string.h>
#pragma hdrstop
#include "reg.h"
#include "TpresetSettings.h"
#include "ffdebug.h"

const int TpresetSettings::min_order=1;
const int TpresetSettings::orderPostprocDef=1;
const int TpresetSettings::orderPictPropDef=2;
const int TpresetSettings::orderOffsetDef=3;
const int TpresetSettings::orderBlurDef=4;
const int TpresetSettings::orderSharpenDef=5;
const int TpresetSettings::orderNoiseDef=6;
const int TpresetSettings::orderSubtitlesDef=7;
const int TpresetSettings::max_order=7;

TpresetSettings::TpresetSettings(const char *IpresetName) 
{
 strcpy(presetName,IpresetName);
 autoLoadedFromFile=0;
};
void TpresetSettings::loadDefault(void)
{
 strcpy(presetName,FFPRESET_DEFAULT);
 loadReg();
}
void TpresetSettings::loadReg(void)
{
 char presetRegStr[256];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",presetName);
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_CURRENT_USER, presetRegStr, 0, KEY_READ, &hKey);
 
 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_GET_N
 #define _REG_OP_S REG_GET_S
 #include "presets_template.h"
 
 RegCloseKey(hKey);
}
void TpresetSettings::saveReg(void)
{
 char presetRegStr[256];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",presetName);
 HKEY hKey;DWORD dispo;
 if (RegCreateKeyEx(HKEY_CURRENT_USER,presetRegStr,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE, KEY_WRITE,0,&hKey, &dispo)!=ERROR_SUCCESS) return;

 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_SET_N
 #define _REG_OP_S REG_SET_S
 #include "presets_template.h"

 RegCloseKey(hKey);
}
void TpresetSettings::loadFile(const char *flnm)
{
 char sections[4096]="";
 GetPrivateProfileSectionNames(sections,4095,flnm);
 if (sections[0]=='\0') return;
 _splitpath(flnm,NULL,NULL,presetName,NULL);
 char pomS[256],propS[256];
 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_GET_N_FILE
 #define _REG_OP_S REG_GET_S_FILE
 #include "presets_template.h"
}
void TpresetSettings::saveFile(const char *flnm)
{
 char pomS[256];

 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_SET_N_FILE
 #define _REG_OP_S REG_SET_S_FILE
 #include "presets_template.h"
}

bool TpresetSettings::isValidPresetName(const char *presetName)
{
 if (presetName[0]=='\0') return false;
 for (unsigned int i=0;i<strlen(presetName);i++)
  if (presetName[i]=='*' || presetName[i]=='?') return false;
 return true; 
}

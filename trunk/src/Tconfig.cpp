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
#include "Tconfig.h"
#include "xvid\xvid.h"
#include <stdlib.h>
#include "settings\reg.h"

Tconfig config;

int Tconfig::cpu_flags=0;

void Tconfig::init(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 char sysdir[256];
 GetSystemDirectory(sysdir,255);
 REG_GET_S("pth",pth,sysdir);
 RegCloseKey(hKey);
 
 if (strlen(pth) && pth[strlen(pth)-1]!='\\') strcat(pth,"\\");
 XVID_INIT_PARAM xip;
 xip.cpu_flags=0;
 xvid_init(NULL, 0, &xip, NULL);
 cpu_flags=xip.cpu_flags;
}
/*
vector<string> *Tconfig::presets=NULL;

Tconfig::Tconfig(const char *IpresetName)
{
 init();
 //isDlg=0;inPlayer=1;
 sub=NULL;
 strcpy(presetName,IpresetName);
 //loadLM();
 loadPreset(NULL);
}

void Tconfig::init(void)
{
 if (!presets)
  {
   XVID_INIT_PARAM xip;
   xip.cpu_flags=0;
   xvid_init(NULL, 0, &xip, NULL);
   cpu_flags=xip.cpu_flags;
   presets=new vector<string>;
   listPresets();
  }; 
 autoloadedfromreg=0;
}
void Tconfig::done(bool isSave)
{
 if (isSave)
  savePreset(NULL);
}
/*
void Tconfig::loadLM(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_LOCAL_MACHINE, FFDSHOW_REG_PARENT "\\" FFDSHOW_REG_CHILD, 0, KEY_READ, &hKey);
 char sysdir[256];
 GetSystemDirectory(sysdir,255);
 REG_GET_S("pth",pth,sysdir);
 if (strlen(pth) && pth[strlen(pth)-1]!='\\') strcat(pth,"\\");
 REG_GET_N("xvid",xvid,1);
 REG_GET_N("div3",div3,0);
 REG_GET_N("divx",divx,0);
 REG_GET_N("dx50",dx50,0);
 REG_GET_N("mp43",mp43,0);
 REG_GET_N("mp42",mp42,0);
 REG_GET_N("mp41",mp41,0);
 REG_GET_N("h263",h263,0);
 RegCloseKey(hKey);
}
void Tconfig::saveLM(void)
{
 HKEY hKey;DWORD dispo;
 if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,FFDSHOW_REG_PARENT "\\" FFDSHOW_REG_CHILD,0,   FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE, KEY_WRITE,0,&hKey, &dispo) != ERROR_SUCCESS)
  return;
 REG_SET_S("pth",pth,"");
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
bool Tconfig::savePreset(const char *IpresetName)
{
 if (!presetShouldBeSaved) return true;
 char presetRegStr[256];
 if (IpresetName)
  {
   if (IpresetName[0]=='\0') return false;
   strcpy(presetName,IpresetName);
  }; 
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",presetName);
 HKEY hKey;DWORD dispo;
 if (RegCreateKeyEx(HKEY_CURRENT_USER,presetRegStr,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE, KEY_WRITE,0,&hKey, &dispo) != ERROR_SUCCESS)
  return false;

 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_SET_N
 #define _REG_OP_S REG_SET_S
 #include "settings\presets_template.h"

 RegCloseKey(hKey);

 if (RegCreateKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,FFDSHOW_REG_CLASS,REG_OPTION_NON_VOLATILE, KEY_WRITE,0,&hKey,&dispo)==ERROR_SUCCESS)
  {
   if (!autoloadedfromreg) REG_SET_S("activePreset",presetName,"");
   //REG_SET_N("autoPreset",autoPreset,0);
   //REG_SET_N("autoPresetFileFirst",autoPresetFileFirst,0);
   //REG_SET_N("lastPage",lastPage,0);
   //REG_SET_N("trayIcon",trayIcon,0);
   RegCloseKey(hKey);
  }; 
 
 vector<string>::iterator i=find(presets->begin(),presets->end(),presetName);
 if (i==presets->end()) 
  presets->push_back(presetName);
 //saveLM();
 return true;
}
void Tconfig::loadPreset(const char *IpresetName)
{
 char presetRegStr[256];
 if (IpresetName) strcpy(presetName,IpresetName);
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",presetName);
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_CURRENT_USER, presetRegStr, 0, KEY_READ, &hKey);
 
 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_GET_N
 #define _REG_OP_S REG_GET_S
 #include "settings\presets_template.h"
 
 RegCloseKey(hKey);

 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT "\\" FFDSHOW_REG_CHILD, 0, KEY_READ, &hKey);
 //REG_GET_N("autoPreset",autoPreset,0);
 //REG_GET_N("lastPage",lastPage,0);
 //REG_GET_N("trayIcon",trayIcon,0);
 //REG_GET_N("autoPresetFileFirst",autoPresetFileFirst,0);
 RegCloseKey(hKey); 
 presetShouldBeSaved=true;
}

void Tconfig::loadActivePreset(void)
{
 HKEY hKey;DWORD size;
 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT "\\" FFDSHOW_REG_CHILD, 0, KEY_READ, &hKey);
 char actPresetName[256];
 REG_GET_S("activePreset",actPresetName,PRESET_DEFAULT);
 RegCloseKey(hKey); 
 loadPreset(actPresetName);
}

void Tconfig::loadAutoPresetFromReg(char *AVIname)
{
 char name2[1024];
 normalizeName(name2,AVIname);
 char presetRegStr[1024];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",name2);
 HKEY hKey;
 if (RegOpenKeyEx(HKEY_CURRENT_USER, presetRegStr, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
  {
   loadPreset(name2);
   autoloadedfromreg=1;
  }; 
 RegCloseKey(hKey);
}

void Tconfig::autoPresetLoad(char *AVIname)
{
 if (autoPresetFileFirst)
  {
   char drive[10],path[255],name[256];
   _splitpath(AVIname,drive,path,name,NULL);
   char presetFlnm[1024];
   _makepath(presetFlnm,drive,path,name,FFPRESET);
   if (GetFileAttributes(presetFlnm)!=INVALID_FILE_ATTRIBUTES)
    {
     loadPresetFromFile(presetFlnm);
     presetShouldBeSaved=false;
     vector<string>::iterator i=find(presets->begin(),presets->end(),presetName);
     if (i==presets->end()) 
      presets->push_back(presetName);
     return;
    } 
  }
 loadAutoPresetFromReg(AVIname); 
}
/*
void Tconfig::listPresets(void)
{
 HKEY hKey;
 RegOpenKeyEx(HKEY_CURRENT_USER, FFDSHOW_REG_PARENT "\\" FFDSHOW_REG_CHILD, 0, KEY_READ, &hKey);
 for (int i=0,retCode=ERROR_SUCCESS;retCode==ERROR_SUCCESS;i++) 
  { 
   char keyName[256];DWORD keyNameSize=255;
   FILETIME ftLastWriteTime;
   retCode = RegEnumKeyEx(hKey, 
                          i, 
                          keyName, 
                          &keyNameSize, 
                          NULL, 
                          NULL, 
                          NULL, 
                          &ftLastWriteTime
                         ); 
   if (retCode==ERROR_SUCCESS) 
   presets->push_back(string(keyName));
  };
 RegCloseKey(hKey);
 if (presets->size()==0)
  {
   Tconfig defCfg(PRESET_DEFAULT);
   defCfg.savePreset(NULL);
   //presets->insert(presets->begin(),PRESET_DEFAULT);;
  }
}

bool Tconfig::removePreset(const char *name)
{
 if (strcmp(name,PRESET_DEFAULT)==0) return false;
 vector<string>::iterator i=find(presets->begin(),presets->end(),name);
 if (i==presets->end()) return false;
 char presetRegStr[256];presetRegStr[0]='\0';
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",name);
 if (presetRegStr[0]!='\0') RegDeleteKey(HKEY_CURRENT_USER,presetRegStr);
 presets->erase(i);
 return true;
}

void Tconfig::normalizeName(char *dst,char *src)
{
 char c;
 do
 {
  c=*(src++);
  if (c=='\\') c='/';
  *(dst++)=c;
 } while (c);
}

//file operations with presets

bool Tconfig::savePresetToFile(const char *flnm)
{
 char pomS[256];

 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_SET_N_FILE
 #define _REG_OP_S REG_SET_S_FILE
 #include "settings\presets_template.h"
 
 return TRUE;
}

void Tconfig::loadPresetFromFile(const char *flnm)
{
 char sections[4096]="";
 GetPrivateProfileSectionNames(sections,4095,flnm);
 if (sections[0]=='\0') return;
 normalizeName(presetName,sections);
 char pomS[256],propS[256];
 #undef _REG_OP_N
 #undef _REG_OP_S
 #define _REG_OP_N REG_GET_N_FILE
 #define _REG_OP_S REG_GET_S_FILE
 #include "settings\presets_template.h"
 presetShouldBeSaved=true;
 vector<string>::iterator i=find(presets->begin(),presets->end(),presetName);
 if (i==presets->end()) 
  presets->push_back(presetName);
}

// descriptions

void Tconfig::getPostProcDescription(char *buf)
{
 char pomS[256];
 strcpy(buf,"Postprocessing (");
 if (ppIsCustom)
  {
   strcat(buf,"custom");
  }
 else
  {
   strcat(buf,"presets: ");
   sprintf(pomS,"strength %i",ppqual);strcat(buf,pomS);
   if (autoq) strcat(buf," automatic");
  } 
 if (levelFixChrom || levelFixLum) strcat(buf,", level fix"); 
 strcat(buf,")");
}
void Tconfig::getPictPropDescription(char *buf)
{
 __asm emms;
 sprintf(buf,"Picture properties (lum gain:%i, lum offset:%i, gamma:%5.2f, hue:%i, saturation:%i)",lumGain,lumOffset,float(gammaCorrection/100.0),hue,saturation);
}
void Tconfig::getNoiseDescription(char *buf)
{
 sprintf(buf,"Noise (method:%s, %sluma strength:%i, chroma strength:%i)",(noiseMethod==0)?"old":"avih",uniformNoise?"uniform, ":"",noiseStrength,noiseStrengthChroma);
}
void Tconfig::getBlurDescription(char *buf)
{
 sprintf(buf,"Blur (strength:%i)",blurStrength);
}
void Tconfig::getSharpenDescription(char *buf)
{
 char tstr[256];
 sprintf(tstr," threshold: %i",xsharp_threshold);
 sprintf(buf,"Sharpen (method:%s, strength:%i%s)",(sharpenMethod==0)?"xsharpen":"unsharp mask",(sharpenMethod==0)?xsharp_strength:unsharp_strength,(sharpenMethod==0)?tstr:"");
}
void Tconfig::getCropDescription(char *buf)
{
 strcpy(buf,"Crop && zoom (");
 char pomS[256];
 if (isZoom)
  if (magnificationLocked) 
   sprintf(pomS,"zoom: %i%%)",magnificationX);
  else
   sprintf(pomS,"horizontal zoom: %i%%, vertical zoom: %i%%)",magnificationX,magnificationY);
 else
  sprintf(pomS,"crop: left:%i, top:%i, right:%i, bottom:%i)",cropLeft,cropTop,cropRight,cropBottom);
 strcat(buf,pomS);
}
*/
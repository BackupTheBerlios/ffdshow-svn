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
#include "Tpresets.h"
#include "reg.h"

using namespace std;

Tpresets::~Tpresets()
{
 done();
}
void Tpresets::listRegKeys(std::vector<string> &list)
{
 list.clear();
 
 HKEY hKey;
 RegOpenKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
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
    list.push_back(string(keyName));
   else
    break;
  };
 RegCloseKey(hKey);
}
void Tpresets::init(void)
{
 TpresetSettings *def=new TpresetSettings;
 def->loadDefault();
 push_back(def);

 vector<string> keys;
 listRegKeys(keys);
 for (vector<string>::iterator i=keys.begin();i!=keys.end();i++)
  if (findPreset(i->c_str())==end()) 
   {
    TpresetSettings *preset=new TpresetSettings(i->c_str());
    preset->loadReg();
    push_back(preset);
   }
 if (keys.size()==0)
  (*begin())->saveReg();
}
void Tpresets::done(void)
{
 for (iterator i=begin();i!=end();i++)
  delete *i;
 clear(); 
}
Tpresets::iterator Tpresets::findPreset(const char *presetName)
{
 for (iterator i=begin();i!=end();i++)
  if (_stricoll(presetName,(*i)->presetName)==0)
   return i;
 return end();  
}

void Tpresets::storePreset(TpresetSettings *preset)
{
 iterator i=findPreset(preset->presetName);
 if (i!=end())
  *i=preset;
 else
  push_back(preset);
}

TpresetSettings* Tpresets::getPreset(const char *presetName)
{
 iterator i=findPreset(presetName);
 if (i!=end()) return *i;
 else return NULL;
}

void Tpresets::savePreset(TpresetSettings *preset,const char *presetName)
{
 if (presetName)
  strcpy(preset->presetName,presetName);
 preset->saveReg();
 storePreset(preset);
}

void Tpresets::removePreset(const char *presetName)
{
 iterator i=findPreset(presetName);
 if (i!=begin() && i!=end())
  {
   delete *i;
   erase(i);
  }
}

void Tpresets::nextUniqueName(TpresetSettings *preset)
{
 iterator i=findPreset(preset->presetName);
 if (i==end()) return;
 for (int ii=1;;ii++)
  {
   char pomS[260];
   sprintf(pomS,"%s %i",preset->presetName,ii);
   if (findPreset(pomS)==end())
    {
     strcpy(preset->presetName,pomS);
     return;
    }
  }
}
void Tpresets::saveRegAll(void)
{
 for (iterator ii=begin();ii!=end();ii++)
  (*ii)->saveReg();

 vector<string> keys;
 listRegKeys(keys);
 for (vector<string>::iterator i=keys.begin();i!=keys.end();i++)
  if (i->c_str()[0]!='\0' && findPreset(i->c_str())==end())
   {
    char presetRegStr[256];
    sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",i->c_str());
    RegDeleteKey(HKEY_CURRENT_USER,presetRegStr);
   }
}

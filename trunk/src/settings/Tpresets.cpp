#include <windows.h>
#include <stdio.h>
#include <string.h>
#pragma hdrstop
#include "Tpresets.h"
#include "reg.h"

Tpresets::~Tpresets()
{
 for (iterator i=begin();i!=end();i++)
  delete *i;
}
void Tpresets::init(void)
{
 TpresetSettings *def=new TpresetSettings;
 def->loadDefault();
 push_back(def);

 HKEY hKey;
 RegOpenKeyEx(HKEY_CURRENT_USER,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD,0,KEY_READ,&hKey);
 int i,retCode;
 for (i=0,retCode=ERROR_SUCCESS;retCode==ERROR_SUCCESS;i++) 
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
    {
     TpresetSettings *preset=new TpresetSettings(keyName);
     preset->loadReg();
     push_back(preset);
    }
   else break;
  };
 RegCloseKey(hKey);
 if (i==0)
  (*begin())->saveReg();
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
}

#include "Tpresets.h"
#include <string.h>
#include <windows.h>
#include "reg.h"

void Tpresets::init(void)
{
 TpresetSettings def;def.loadDefault();
 push_back(def);

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
    {
     TpresetSettings preset(keyName);
     preset.loadReg();
     push_back(preset);
    };
  };
 RegCloseKey(hKey);
 if (size()==0)
  (*begin()).saveReg();
}

Tpresets::iterator Tpresets::findPreset(const char *presetName)
{
 for (iterator i=begin();i!=end();i++)
  if (_stricoll(presetName,i->presetName)==0)
   return i;
 return end();  
}

void Tpresets::storePreset(const TpresetSettings &preset)
{
 iterator i=findPreset(preset.presetName);
 if (i!=end())
  *i=preset;
 else
  push_back(preset);
}

TpresetSettings Tpresets::loadPreset(const char *presetName)
{
 iterator i=findPreset(presetName);
 if (i!=end()) return *i;
 else 
  {
   TpresetSettings presetSettings=*begin();
   strcpy(presetSettings.presetName,presetName);
   return presetSettings;
  }
}

void Tpresets::savePreset(TpresetSettings &preset,const char *presetName)
{
 if (presetName)
  strcpy(preset.presetName,presetName);
 preset.saveReg();
 storePreset(preset);
}

void Tpresets::removePreset(const char *presetName)
{
}

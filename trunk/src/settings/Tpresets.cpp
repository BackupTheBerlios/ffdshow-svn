#include "Tpresets.h"
#include <string.h>

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
   TpresetSettings presetSettings;
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

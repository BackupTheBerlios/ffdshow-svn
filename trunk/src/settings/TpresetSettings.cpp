#include <windows.h>
#include "reg.h"
#include "TpresetSettings.h"
#include <stdio.h>

void TpresetSettings::loadReg (char *IpresetName)
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
void TpresetSettings::saveReg (void)
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
 normalizePresetName(presetName,sections);
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

// descriptions

void TpresetSettings::getPostProcDescription(char *buf)
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
void TpresetSettings::getPictPropDescription(char *buf)
{
 __asm emms;
 sprintf(buf,"Picture properties (lum gain:%i, lum offset:%i, gamma:%5.2f, hue:%i, saturation:%i)",lumGain,lumOffset,float(gammaCorrection/100.0),hue,saturation);
}
void TpresetSettings::getNoiseDescription(char *buf)
{
 sprintf(buf,"Noise (method:%s, %sluma strength:%i, chroma strength:%i)",(noiseMethod==0)?"old":"avih",uniformNoise?"uniform, ":"",noiseStrength,noiseStrengthChroma);
}
void TpresetSettings::getBlurDescription(char *buf)
{
 sprintf(buf,"Blur (strength:%i)",blurStrength);
}
void TpresetSettings::getSharpenDescription(char *buf)
{
 char tstr[256];
 sprintf(tstr," threshold: %i",xsharp_threshold);
 sprintf(buf,"Sharpen (method:%s, strength:%i%s)",(sharpenMethod==0)?"xsharpen":"unsharp mask",(sharpenMethod==0)?xsharp_strength:unsharp_strength,(sharpenMethod==0)?tstr:"");
}
void TpresetSettings::getCropDescription(char *buf)
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

#ifndef _TCONFIG_H_
#define _TCONFIG_H_

#define FFMPEG_IDCT "ffmpeg idct"
#define FFPRESET "ffpreset"
static const char PRESET_DEFAULT[]="default";
#include <vector>

struct subtitle;
class Tconfig
{
private:
 void loadLM(void);
 void saveLM(void);
 static void listPresets(void);
 void loadAutoPresetFromReg(char *AVIname);
public:
 Tconfig(const char *IpresetName=PRESET_DEFAULT);
 static std::vector<std::string> *presets;
 
 void init(void);
 void done(bool save);
 char pth[1024];
 static int cpu_flags;
 
 int trayIcon;
 int isDlg;
 int lastPage;
 int inPlayer;
 
 char presetName[256];
 static bool removePreset(const char *name);
 bool savePreset(const char *IpresetName),savePresetToFile(const char *flnm);
 void loadPreset(const char *IpresetName),loadPresetFromFile(const char *flnm);
 void loadActivePreset(void);
 static void Tconfig::normalizeName(char *dst,char *src);
 int autoPreset,autoPresetFileFirst;
 void autoPresetLoad(char *AVIname);
 int presetShouldBeSaved;
 
 int isPostproc;
 int ppqual;
 int autoq;int currentq;
 int ppIsCustom,ppcustom;
 int ppsetting;
 int levelFixLum,levelFixChrom;
 void getPostProcDescription(char *buf);
  
 int isPictProp; 
 int lumGain,lumOffset,gammaCorrection;
 static const int lumGainDef,lumOffsetDef,gammaCorrectionDef;
 int hue,saturation;
 static const int hueDef,saturationDef;
 void getPictPropDescription(char *buf);
 
 int xvid,div3,divx,dx50,mp43,mp42,mp41,h263;
 
 int flip;

 int isBlur;
 int blurStrength;
 void getBlurDescription(char *buf);
 
 int isSharpen,sharpenMethod;
 int xsharp_strength,xsharp_threshold;
 static const int xsharp_strengthDef,xsharp_thresholdDef;
 int unsharp_strength,unsharp_threshold;
 static const int unsharp_strengthDef,unsharp_thresholdDef;
 void getSharpenDescription(char *buf);
 
 int isNoise,noiseMethod,uniformNoise;
 int noiseStrength,noiseStrengthChroma;
 static const int noiseStrengthDef,noiseStrengthChromaDef;
 void getNoiseDescription(char *buf);
 
 int idct,idctChanged;
 static const int idctDef;
 
 int isResize;
 int resizeDx,resizeDy;
 int resizeAspect,resizeFirst;
 int aspectRatio;
 int resizeMethod;
 int resizeGblurLum,resizeGblurChrom;
 int resizeSharpenLum,resizeSharpenChrom;
 int isCropNzoom;
 int isZoom;
 int magnificationX,magnificationY,magnificationLocked;
 int cropTop,cropBottom,cropLeft,cropRight;
 int autocrop;
 void getCropDescription(char *buf);

 int isSubtitles;
 int fontChanged;
 char fontName[256];
 int fontCharset,fontSize,fontSpacing,fontWeight,fontColor,fontShadowStrength,fontShadowRadius;
 int fontAutosize;
 int subPosX,subPosY,subDelay,subSpeed;
 int subAutoFlnm;
 char subFlnm[256];
 subtitle *sub;
};

#endif

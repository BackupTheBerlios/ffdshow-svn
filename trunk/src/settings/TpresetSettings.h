#ifndef _TPRESETSETTINGS_H_
#define _TPRESETSETTINGS_H_

#define FFPRESET_EXT "ffpreset"
#define FFPRESET_DEFAULT   "default"

struct TpresetSettings
{
public:
 static void normalizePresetName(char *dst,const char *src)
  {
   char c;
   do
   {
    c=*(src++);
    if (c=='\\') c='/';
    *(dst++)=c;
   } while (c);
  }
 static bool isValidPresetName(const char *presetName);
public:
 TpresetSettings(const char *IpresetName=FFPRESET_DEFAULT);
 void loadDefault(void);
 void loadReg(void),saveReg(void);
 void loadFile(const char *flnm),saveFile(const char *flnm);
 int autoLoadedFromFile;

 char presetName[260];
 int isPostproc,orderPostproc;
 static const int orderPostprocDef;
 int ppqual;
 int autoq;int currentq;
 int ppIsCustom,ppcustom;
 //int ppsetting;
 int levelFixLum,levelFixChrom;
  
 int isPictProp,orderPictProp; 
 static const int orderPictPropDef;
 int lumGain,lumOffset,gammaCorrection;
 static const int lumGainDef,lumOffsetDef,gammaCorrectionDef;
 int hue,saturation;
 static const int hueDef,saturationDef;
 
 int flip;

 int isBlur,orderBlur;
 static const int orderBlurDef;
 int blurStrength,tempSmooth;
 
 int isSharpen,orderSharpen;
 static const int orderSharpenDef;
 int sharpenMethod;
 int xsharp_strength,xsharp_threshold;
 static const int xsharp_strengthDef,xsharp_thresholdDef;
 int unsharp_strength,unsharp_threshold;
 static const int unsharp_strengthDef,unsharp_thresholdDef;
 
 int isNoise,orderNoise;
 static const int orderNoiseDef;
 int noiseMethod,uniformNoise;
 int noiseStrength,noiseStrengthChroma;
 static const int noiseStrengthDef,noiseStrengthChromaDef;
 
 int idct;
 
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

 int isSubtitles,orderSubtitles;
 static const int orderSubtitlesDef;
 int fontChanged;
 char fontName[256];
 int fontCharset,fontSize,fontSpacing,fontWeight,fontColor,fontShadowStrength,fontShadowRadius;
 int subPosX,subPosY,subDelay,subSpeed;
 int subAutoFlnm;
 char subFlnm[260];

 int isOffset,orderOffset;
 static const int orderOffsetDef;
 int offsetY_X,offsetY_Y,offsetU_X,offsetU_Y,offsetV_X,offsetV_Y;

 static const int min_order,max_order;
};

#endif

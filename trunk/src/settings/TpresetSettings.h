#ifndef _TPRESETSETTINGS_H_
#define _TPRESETSETTINGS_H_

#define FFPRESET_EXT "ffpreset"
#define FFPRESET_DEFAULT "default"

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
 int fullPostproc;
 int ppqual;
 int autoq;
 int ppIsCustom,ppcustom;
 int deblockStrength;
 static const int deblockStrengthDef;
 int levelFixLum,levelFixChrom;
  
 int isPictProp,orderPictProp; 
 static const int orderPictPropDef;
 int fullPictProp;
 int lumGain,lumOffset,gammaCorrection;
 static const int lumGainDef,lumOffsetDef,gammaCorrectionDef;
 int hue,saturation;
 static const int hueDef,saturationDef;
 
 int flip;

 int isBlur,orderBlur;
 static const int orderBlurDef;
 int fullBlur;
 int blurStrength,tempSmooth;
 
 int isSharpen,orderSharpen;
 static const int orderSharpenDef;
 int fullSharpen;
 int sharpenMethod;
 int xsharp_strength,xsharp_threshold;
 static const int xsharp_strengthDef,xsharp_thresholdDef;
 int unsharp_strength,unsharp_threshold;
 static const int unsharp_strengthDef,unsharp_thresholdDef;
 
 int isNoise,orderNoise;
 static const int orderNoiseDef;
 int fullNoise;
 int noiseMethod,uniformNoise;
 int noiseStrength,noiseStrengthChroma;
 static const int noiseStrengthDef,noiseStrengthChromaDef;
 
 int idct;
 
 int isResize,orderResize;
 static const int orderResizeDef;
 int fullResize;
 int resizeDx,resizeDy;
 int resizeAspect;
 int aspectRatio;
 int resizeMethod;
 static const int resizeMethodNone;
 int resizeGblurLum,resizeGblurChrom;
 int resizeSharpenLum,resizeSharpenChrom;
 
 int isCropNzoom;
 int isZoom;
 int magnificationX,magnificationY,magnificationLocked;
 int cropTop,cropBottom,cropLeft,cropRight;
 int autocrop;

 int isSubtitles,orderSubtitles;
 static const int orderSubtitlesDef;
 int fullSubtitles;
 char fontName[256];
 int fontCharset,fontSize,fontSpacing,fontWeight,fontColor,fontShadowStrength,fontShadowRadius;
 int subPosX,subPosY;
 int subDelay,subSpeed;
 static const int subDelayDef,subSpeedDef;
 int subAutoFlnm;
 char subFlnm[260];

 int isOffset,orderOffset;
 static const int orderOffsetDef;
 int fullOffset;
 int offsetY_X,offsetY_Y,offsetU_X,offsetU_Y,offsetV_X,offsetV_Y;
 
 int orderShowMV;
 static const int orderShowMVdef;

 static const int min_order,max_order;
};

#endif

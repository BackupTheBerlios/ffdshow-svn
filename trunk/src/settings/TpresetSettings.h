#ifndef _TPRESETSETTINGS_H_
#define _TPRESETSETTINGS_H_

#define FFPRESET_EXT "ffpreset"

struct TpresetSettings
{
private:
 static void normalizePresetName(char *dst,char *src)
  {
   char c;
   do
   {
    c=*(src++);
    if (c=='\\') c='/';
    *(dst++)=c;
   } while (c);
  }
public:
 TpresetSettings(void);
 TpresetSettings(const char *IpresetName);
 void loadDefault(void);
 void loadReg(void),saveReg(void);
 void loadFile(const char *flnm),saveFile(const char *flnm);
 int presetShouldBeSaved,autoloadedfromreg;

 char presetName[1024];
 int isPostproc,orderPostproc;
 int ppqual;
 int autoq;int currentq;
 int ppIsCustom,ppcustom;
 //int ppsetting;
 int levelFixLum,levelFixChrom;
 void getPostProcDescription(char *buf);
  
 int isPictProp,orderPictProp; 
 int lumGain,lumOffset,gammaCorrection;
 static const int lumGainDef,lumOffsetDef,gammaCorrectionDef;
 int hue,saturation;
 static const int hueDef,saturationDef;
 void getPictPropDescription(char *buf);
 
 int flip;

 int isBlur,orderBlur;
 int blurStrength;
 void getBlurDescription(char *buf);
 
 int isSharpen,orderSharpen;
 int sharpenMethod;
 int xsharp_strength,xsharp_threshold;
 static const int xsharp_strengthDef,xsharp_thresholdDef;
 int unsharp_strength,unsharp_threshold;
 static const int unsharp_strengthDef,unsharp_thresholdDef;
 void getSharpenDescription(char *buf);
 
 int isNoise,orderNoise;
 int noiseMethod,uniformNoise;
 int noiseStrength,noiseStrengthChroma;
 static const int noiseStrengthDef,noiseStrengthChromaDef;
 void getNoiseDescription(char *buf);
 
 int idct;
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

 int isSubtitles,orderSubtitles;
 int fontChanged;
 char fontName[256];
 int fontCharset,fontSize,fontSpacing,fontWeight,fontColor,fontShadowStrength,fontShadowRadius;
 int fontAutosize;
 int subPosX,subPosY,subDelay,subSpeed;
 int subAutoFlnm;
 char subFlnm[256];

 static int min_order,max_order;
};

#endif
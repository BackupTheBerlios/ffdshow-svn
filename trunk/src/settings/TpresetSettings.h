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
 char presetName[260];
 int autoLoadedFromFile;

 int isPostproc,orderPostproc;
 static const int orderPostprocDef;
 int fullPostproc;

 struct TpostprocSettings
  {
   int qual;
   int autoq;
   int isCustom,custom;
   int deblockStrength;
   static const int deblockStrengthDef;
   int levelFixLum,levelFixChrom;
  } postproc;
  
 int isPictProp,orderPictProp; 
 static const int orderPictPropDef;
 int fullPictProp;
 struct TpictPropSettings
  {
   int lumGain,lumOffset,gammaCorrection;
   static const int lumGainDef,lumOffsetDef,gammaCorrectionDef;
   int hue,saturation;
   static const int hueDef,saturationDef;
  } pictProp;
 
 int isDeinterlace;
 
 int flip;

 int isBlur,orderBlur;
 static const int orderBlurDef;
 int fullBlur;
 struct TblurSettings
  {
   int strength;
   int tempSmooth;
  } blur;
 
 int isSharpen,orderSharpen;
 static const int orderSharpenDef;
 int fullSharpen;
 struct TsharpenSettings
  {
   int method;
   int xsharpStrength,xsharpThreshold;
   static const int xsharpStrengthDef,xsharpThresholdDef;
   int unsharpStrength,unsharpThreshold;
   static const int unsharpStrengthDef,unsharpThresholdDef;
  } sharpen;
 
 int isNoise,orderNoise;
 static const int orderNoiseDef;
 int fullNoise;
 struct TnoiseSettings
  {
   int method,uniformNoise;
   int strength,strengthChroma;
   static const int strengthDef,strengthChromaDef;
  } noise;
 
 int idct;
 
 int isResize,orderResize;
 static const int orderResizeDef;
 int fullResize;
 struct TresizeAspectSettings
  {
   int dx,dy;
   int isAspect;
   int aspectRatio;
   int method;
   static const int methodNone;
   int GblurLum,GblurChrom;
   int sharpenLum,sharpenChrom;
  } resizeAspect;
 
 int isCropNzoom;
 struct TcropSettings
  {
   int isZoom;
   int magnificationX,magnificationY,magnificationLocked;
   int cropTop,cropBottom,cropLeft,cropRight;
   int autocrop;
  } cropNzoom;

 int isSubtitles,orderSubtitles;
 static const int orderSubtitlesDef;
 int fullSubtitles;
 struct TfontSettings
  {
   char name[256];
   int charset,size,spacing,weight,color,shadowStrength,shadowRadius;
  } font;
 struct TsubtitlesSettings
  {
   char flnm[260];
   int autoFlnm;
   int posX,posY;
   int delay,speed;
   static const int delayDef,speedDef;
  } subtitles;

 int isOffset,orderOffset;
 static const int orderOffsetDef;
 int fullOffset;
 struct ToffsetSettings
  {
   int Y_X,Y_Y,U_X,U_Y,V_X,V_Y;
  } offset;
 
 int orderShowMV;
 static const int orderShowMVdef;

 static const int min_order,max_order;
};

#endif

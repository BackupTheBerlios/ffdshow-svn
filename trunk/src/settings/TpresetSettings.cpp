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

#include "stdafx.h"
#include "reg.h"
#include "TpresetSettings.h"
#include "ffdebug.h"

const int TpresetSettings::min_order=1;
const int TpresetSettings::orderPostprocDef=1;
const int TpresetSettings::orderPictPropDef=2;
const int TpresetSettings::orderOffsetDef=3;
const int TpresetSettings::orderBlurDef=4;
const int TpresetSettings::orderSharpenDef=5;
const int TpresetSettings::orderNoiseDef=6;
const int TpresetSettings::orderSubtitlesDef=7;
const int TpresetSettings::orderResizeDef=8;
const int TpresetSettings::orderShowMVdef=9;
const int TpresetSettings::max_order=9;

void TpresetSettings::reg_op(TregOp &t)
{
 t._REG_OP_N("flip",flip,0);
 t._REG_OP_N("idct",idct,0);

 t._REG_OP_N("isPostproc",isPostproc,0);
 t._REG_OP_N("orderPostproc",orderPostproc,orderPostprocDef);
 t._REG_OP_N("fullPostproc",fullPostproc,0);
 t._REG_OP_N("ppIsCustom",postproc.isCustom,0);
 t._REG_OP_N("ppqual",postproc.qual,0);
 t._REG_OP_N("autoq",postproc.autoq,0);
 t._REG_OP_N("ppcustom",postproc.custom,0);
 t._REG_OP_N("deblockStrength",postproc.deblockStrength,postproc.deblockStrengthDef);
 t._REG_OP_N("levelFixLum",postproc.levelFixLum,0);
 t._REG_OP_N("levelFixChrom",postproc.levelFixChrom,0);postproc.levelFixChrom=0;
 t._REG_OP_N("fullYrange",postproc.fullYrange,0);
 t._REG_OP_N("tempNoiseFilter",postproc.tempNoiseFilter,0);

 t._REG_OP_N("isDeinterlace",isDeinterlace,0);
 t._REG_OP_N("fullDeinterlace",fullDeinterlace,0);
 t._REG_OP_N("deinterlaceMethod",deinterlace.method,deinterlace.methodDef);

 t._REG_OP_N("isPictProp",isPictProp,0);
 t._REG_OP_N("orderPictProp",orderPictProp,orderPictPropDef);
 t._REG_OP_N("fullPictProp",fullPictProp,0);
 t._REG_OP_N("lumGain",pictProp.lumGain,pictProp.lumGainDef);
 t._REG_OP_N("lumOffset",pictProp.lumOffset,pictProp.lumOffsetDef);
 t._REG_OP_N("gammaCorrection",pictProp.gammaCorrection,pictProp.gammaCorrectionDef);
 t._REG_OP_N("hue",pictProp.hue,pictProp.hueDef);
 t._REG_OP_N("saturation",pictProp.saturation,pictProp.saturationDef);

 t._REG_OP_N("isBlur",isBlur,0);
 t._REG_OP_N("orderBlur",orderBlur,orderBlurDef);
 t._REG_OP_N("fullBlur",fullBlur,0);
 t._REG_OP_N("blurStrength",blur.strength,30);
 t._REG_OP_N("tempSmooth",blur.tempSmooth,0);

 t._REG_OP_N("xsharpen",isSharpen,0);
 t._REG_OP_N("orderSharpen",orderSharpen,orderSharpenDef);
 t._REG_OP_N("fullSharpen",fullSharpen,0);
 t._REG_OP_N("sharpenMethod",sharpen.method,0);
 t._REG_OP_N("xsharp_strenght",sharpen.xsharpStrength,sharpen.xsharpStrengthDef);
 t._REG_OP_N("xsharp_threshold",sharpen.xsharpThreshold,sharpen.xsharpThresholdDef);
 t._REG_OP_N("unsharp_strength",sharpen.unsharpStrength,sharpen.unsharpStrengthDef);
 t._REG_OP_N("unsharp_threshold",sharpen.unsharpThreshold,sharpen.unsharpThresholdDef);

 t._REG_OP_N("isNoise",isNoise,0);
 t._REG_OP_N("orderNoise",orderNoise,orderNoiseDef);
 t._REG_OP_N("fullNoise",fullNoise,0);
 t._REG_OP_N("noiseMethod",noise.method,0);
 t._REG_OP_N("noiseStrength",noise.strength,noise.strengthDef);
 t._REG_OP_N("noiseStrengthChroma",noise.strengthChroma,noise.strengthChromaDef);
 t._REG_OP_N("uniformNoise",noise.uniformNoise,0);

 t._REG_OP_N("isResize",isResize,0);
 t._REG_OP_N("orderResize",orderResize,orderResizeDef);
 t._REG_OP_N("fullResize",fullResize,0);
 t._REG_OP_N("resizeDx",resizeAspect.dx,640);
 t._REG_OP_N("resizeDy",resizeAspect.dy,480);       
 t._REG_OP_N("resizeAscpect",resizeAspect.isAspect,1);
 t._REG_OP_N("resizeMethod",resizeAspect.method,2);
 t._REG_OP_N("resizeBicubicParam",resizeAspect.bicubicParam,0);
 t._REG_OP_N("resizeXparam",resizeAspect.xParam,0);
 t._REG_OP_N("resizeGaussParam",resizeAspect.gaussParam,0);
 t._REG_OP_N("resizeLanczosParam",resizeAspect.lanczosParam,0);
 t._REG_OP_N("aspectRatio",resizeAspect.aspectRatio,1.33*65536);
 t._REG_OP_N("resizeGblurLum",resizeAspect.GblurLum,0);
 t._REG_OP_N("resizeGblurChrom",resizeAspect.GblurChrom,0);
 t._REG_OP_N("resizeSharpenLum",resizeAspect.sharpenLum,0);
 t._REG_OP_N("resizeSharpenChrom",resizeAspect.sharpenChrom,0);

 t._REG_OP_N("isCropNzoom",isCropNzoom,0);
 t._REG_OP_N("isZoom",cropNzoom.isZoom,1);
 t._REG_OP_N("magnificationX",cropNzoom.magnificationX,0);
 t._REG_OP_N("magnificationY",cropNzoom.magnificationY,0);
 t._REG_OP_N("magnificationLocked",cropNzoom.magnificationLocked,1);
 t._REG_OP_N("cropLeft"  ,cropNzoom.cropLeft,0);
 t._REG_OP_N("cropRight" ,cropNzoom.cropRight ,0);
 t._REG_OP_N("cropTop"   ,cropNzoom.cropTop   ,0);
 t._REG_OP_N("cropBottom",cropNzoom.cropBottom,0);
 t._REG_OP_N("autocrop",cropNzoom.autocrop,0);

 t._REG_OP_N("isSubtitles",isSubtitles,0);
 t._REG_OP_N("orderSubtitles",orderSubtitles,orderSubtitlesDef);
 t._REG_OP_N("fullSubtitles",fullSubtitles,0);
 t._REG_OP_N("subPosX",subtitles.posX,50);
 t._REG_OP_N("subPosY",subtitles.posY,90);
 t._REG_OP_N("subDelay",subtitles.delay,subtitles.delayDef);
 t._REG_OP_N("subSpeed",subtitles.speed,subtitles.speedDef);
 t._REG_OP_S("subFlnm",subtitles.flnm,"");
 t._REG_OP_N("subAutoFlnm",subtitles.autoFlnm,1);

 t._REG_OP_S("fontName",font.name,"Arial");
 t._REG_OP_N("fontCharset",font.charset,DEFAULT_CHARSET);
 t._REG_OP_N("fontSize",font.size,26);
 t._REG_OP_N("fontWeight",font.weight,FW_NORMAL);if (font.weight>900) font.weight=900;
 t._REG_OP_N("fontColor",font.color,RGB(255,255,255));
 t._REG_OP_N("fontShadowStrength",font.shadowStrength,90);
 t._REG_OP_N("fontShadowRadius"  ,font.shadowRadius  ,50);
 t._REG_OP_N("fontSpacing",font.spacing,0);

 t._REG_OP_S("OSDfontName",fontOSD.name,"Arial");
 t._REG_OP_N("OSDfontCharset",fontOSD.charset,DEFAULT_CHARSET);
 t._REG_OP_N("OSDfontSize",fontOSD.size,20);
 t._REG_OP_N("OSDfontWeight",fontOSD.weight,FW_NORMAL);
 t._REG_OP_N("OSDfontColor",fontOSD.color,RGB(255,0,0));
 t._REG_OP_N("OSDfontShadowStrength",fontOSD.shadowStrength,70);
 t._REG_OP_N("OSDfontShadowRadius"  ,fontOSD.shadowRadius  ,60);
 t._REG_OP_N("OSDfontSpacing",fontOSD.spacing,0);

 t._REG_OP_N("isOffset",isOffset,0);
 t._REG_OP_N("orderOffset",orderOffset,orderOffsetDef);
 t._REG_OP_N("fullOffset",fullOffset,0);
 t._REG_OP_N("offsetY_X",offset.Y_X,0);
 t._REG_OP_N("offsetY_Y",offset.Y_Y,0);
 t._REG_OP_N("offsetU_X",offset.U_X,0);
 t._REG_OP_N("offsetU_Y",offset.U_Y,0);
 t._REG_OP_N("offsetV_X",offset.V_X,0);
 t._REG_OP_N("offsetV_Y",offset.V_Y,0);

 t._REG_OP_N("orderShowMV",orderShowMV,orderShowMVdef);
}

TpresetSettings::TpresetSettings(const char *IpresetName) 
{
 memset(&resizeAspect,0,sizeof(TresizeAspectSettings));
 memset(&font,0,sizeof(TfontSettings));
 memset(&cropNzoom,0,sizeof(TcropSettings));
 strcpy(presetName,IpresetName);
 autoLoadedFromFile=0;
}
void TpresetSettings::loadDefault(void)
{
 strcpy(presetName,FFPRESET_DEFAULT);
 loadReg();
}
void TpresetSettings::loadReg(void)
{
 char presetRegStr[256];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",presetName);
 TregOpRegRead t(HKEY_CURRENT_USER,presetRegStr);
 reg_op(t);
}
void TpresetSettings::saveReg(void)
{
 char presetRegStr[256];
 sprintf(presetRegStr,FFDSHOW_REG_PARENT"\\"FFDSHOW_REG_CHILD"\\%s",presetName);
 TregOpRegWrite t(HKEY_CURRENT_USER,presetRegStr);
 reg_op(t);
}
void TpresetSettings::loadFile(const char *flnm)
{
 char sections[4096]="";
 GetPrivateProfileSectionNames(sections,4095,flnm);
 if (sections[0]=='\0') return;
 _splitpath(flnm,NULL,NULL,presetName,NULL);
 TregOpFileRead t(flnm,sections);
 reg_op(t);
}
void TpresetSettings::saveFile(const char *flnm)
{
 DeleteFile(flnm);
 TregOpFileWrite t(flnm,"presetSettings");
 reg_op(t);
}

bool TpresetSettings::isValidPresetName(const char *presetName)
{
 if (presetName[0]=='\0') return false;
 for (unsigned int i=0;i<strlen(presetName);i++)
  if (presetName[i]=='*' || presetName[i]=='?') return false;
 return true; 
}

_REG_OP_N("flip",flip,0);

_REG_OP_N("isPostproc",isPostproc,0);
_REG_OP_N("orderPostproc",orderPostproc,orderPostprocDef);
_REG_OP_N("ppIsCustom",ppIsCustom,0);
_REG_OP_N("ppqual",ppqual,0);currentq=ppqual;
_REG_OP_N("autoq",autoq,0);
_REG_OP_N("ppcustom",ppcustom,0);
_REG_OP_N("levelFixLum",levelFixLum,0);
_REG_OP_N("levelFixChrom",levelFixChrom,0);

_REG_OP_N("isPictProp",isPictProp,0);
_REG_OP_N("orderPictProp",orderPictProp,orderPictPropDef);
_REG_OP_N("lumGain",lumGain,lumGainDef);
_REG_OP_N("lumOffset",lumOffset,lumOffsetDef);
_REG_OP_N("gammaCorrection",gammaCorrection,gammaCorrectionDef);
_REG_OP_N("hue",hue,hueDef);
_REG_OP_N("saturation",saturation,saturationDef);

_REG_OP_N("isBlur",isBlur,0);
_REG_OP_N("orderBlur",orderBlur,orderBlurDef);
_REG_OP_N("blurStrength",blurStrength,30);

_REG_OP_N("xsharpen",isSharpen,0);
_REG_OP_N("orderSharpen",orderSharpen,orderSharpenDef);
_REG_OP_N("sharpenMethod",sharpenMethod,0);
_REG_OP_N("xsharp_strenght",xsharp_strength,xsharp_strengthDef);
_REG_OP_N("xsharp_threshold",xsharp_threshold,xsharp_thresholdDef);
_REG_OP_N("unsharp_strength",unsharp_strength,unsharp_strengthDef);
_REG_OP_N("unsharp_threshold",unsharp_threshold,unsharp_thresholdDef);

_REG_OP_N("isNoise",isNoise,0);
_REG_OP_N("orderNoise",orderNoise,orderNoiseDef);
_REG_OP_N("noiseMethod",noiseMethod,0);
_REG_OP_N("noiseStrength",noiseStrength,noiseStrengthDef);
_REG_OP_N("noiseStrengthChroma",noiseStrengthChroma,noiseStrengthChromaDef);
_REG_OP_N("uniformNoise",uniformNoise,0);

_REG_OP_N("idct",idct,idctDef);

_REG_OP_N("isResize",isResize,0);
_REG_OP_N("resizeDx",resizeDx,640);
_REG_OP_N("resizeDy",resizeDy,480);       
_REG_OP_N("resizeAscpect",resizeAspect,1);
_REG_OP_N("resizeFirst",resizeFirst,0);
_REG_OP_N("resizeMethod",resizeMethod,2);
_REG_OP_N("aspectRatio",aspectRatio,1.33*65536);
_REG_OP_N("resizeGblurLum",resizeGblurLum,0);
_REG_OP_N("resizeGblurChrom",resizeGblurChrom,0);
_REG_OP_N("resizeSharpenLum",resizeSharpenLum,0);
_REG_OP_N("resizeSharpenChrom",resizeSharpenChrom,0);
_REG_OP_N("isCropNzoom",isCropNzoom,0);
_REG_OP_N("isZoom",isZoom,1);
_REG_OP_N("magnificationX",magnificationX,0);
_REG_OP_N("magnificationY",magnificationY,0);
_REG_OP_N("magnificationLocked",magnificationLocked,1);
_REG_OP_N("cropLeft"  ,cropLeft,0);
_REG_OP_N("cropRight" ,cropRight ,0);
_REG_OP_N("cropTop"   ,cropTop   ,0);
_REG_OP_N("cropBottom",cropBottom,0);
_REG_OP_N("autocrop",autocrop,0);

_REG_OP_N("isSubtitles",isSubtitles,0);
_REG_OP_N("orderSubtitles",orderSubtitles,orderSubtitlesDef);
_REG_OP_S("fontName",fontName,"Arial");
_REG_OP_N("fontCharset",fontCharset,DEFAULT_CHARSET);
_REG_OP_N("fontSize",fontSize,26);
_REG_OP_N("fontWeight",fontWeight,FW_NORMAL);
_REG_OP_N("fontColor",fontColor,RGB(255,255,255));
_REG_OP_N("fontShadowStrength",fontShadowStrength,90);
_REG_OP_N("fontShadowRadius"  ,fontShadowRadius  ,50);
_REG_OP_N("fontAutosize",fontAutosize,0);
_REG_OP_N("fontSpacing",fontSpacing,0);
_REG_OP_N("subPosX",subPosX,50);
_REG_OP_N("subPosY",subPosY,90);
_REG_OP_N("subDelay",subDelay,0);
_REG_OP_N("subSpeed",subSpeed,1000);
_REG_OP_S("subFlnm",subFlnm,"");
_REG_OP_N("subAutoFlnm",subAutoFlnm,1);

_REG_OP_N("isOffset",isOffset,0);
_REG_OP_N("orderOffset",orderOffset,orderOffsetDef);
_REG_OP_N("offsetY_X",offsetY_X,0);
_REG_OP_N("offsetY_Y",offsetY_Y,0);
_REG_OP_N("offsetU_X",offsetU_X,0);
_REG_OP_N("offsetU_Y",offsetU_Y,0);
_REG_OP_N("offsetV_X",offsetV_X,0);
_REG_OP_N("offsetV_Y",offsetV_Y,0);

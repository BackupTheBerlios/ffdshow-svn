_REG_OP_N("flip",flip,0);

_REG_OP_N("isPostproc",isPostproc,0);
_REG_OP_N("orderPostproc",orderPostproc,orderPostprocDef);
_REG_OP_N("fullPostproc",fullPostproc,0);
_REG_OP_N("ppIsCustom",postproc.isCustom,0);
_REG_OP_N("ppqual",postproc.qual,0);
_REG_OP_N("autoq",postproc.autoq,0);
_REG_OP_N("ppcustom",postproc.custom,0);
_REG_OP_N("deblockStrength",postproc.deblockStrength,postproc.deblockStrengthDef);
_REG_OP_N("levelFixLum",postproc.levelFixLum,0);
_REG_OP_N("levelFixChrom",postproc.levelFixChrom,0);postproc.levelFixChrom=0;

_REG_OP_N("isDeinterlace",isDeinterlace,0);

_REG_OP_N("isPictProp",isPictProp,0);
_REG_OP_N("orderPictProp",orderPictProp,orderPictPropDef);
_REG_OP_N("fullPictProp",fullPictProp,0);
_REG_OP_N("lumGain",pictProp.lumGain,pictProp.lumGainDef);
_REG_OP_N("lumOffset",pictProp.lumOffset,pictProp.lumOffsetDef);
_REG_OP_N("gammaCorrection",pictProp.gammaCorrection,pictProp.gammaCorrectionDef);
_REG_OP_N("hue",pictProp.hue,pictProp.hueDef);
_REG_OP_N("saturation",pictProp.saturation,pictProp.saturationDef);

_REG_OP_N("isBlur",isBlur,0);
_REG_OP_N("orderBlur",orderBlur,orderBlurDef);
_REG_OP_N("fullBlur",fullBlur,0);
_REG_OP_N("blurStrength",blur.strength,30);
_REG_OP_N("tempSmooth",blur.tempSmooth,0);

_REG_OP_N("xsharpen",isSharpen,0);
_REG_OP_N("orderSharpen",orderSharpen,orderSharpenDef);
_REG_OP_N("fullSharpen",fullSharpen,0);
_REG_OP_N("sharpenMethod",sharpen.method,0);
_REG_OP_N("xsharp_strenght",sharpen.xsharpStrength,sharpen.xsharpStrengthDef);
_REG_OP_N("xsharp_threshold",sharpen.xsharpThreshold,sharpen.xsharpThresholdDef);
_REG_OP_N("unsharp_strength",sharpen.unsharpStrength,sharpen.unsharpStrengthDef);
_REG_OP_N("unsharp_threshold",sharpen.unsharpThreshold,sharpen.unsharpThresholdDef);

_REG_OP_N("isNoise",isNoise,0);
_REG_OP_N("orderNoise",orderNoise,orderNoiseDef);
_REG_OP_N("fullNoise",fullNoise,0);
_REG_OP_N("noiseMethod",noise.method,0);
_REG_OP_N("noiseStrength",noise.strength,noise.strengthDef);
_REG_OP_N("noiseStrengthChroma",noise.strengthChroma,noise.strengthChromaDef);
_REG_OP_N("uniformNoise",noise.uniformNoise,0);

_REG_OP_N("idct",idct,0);

_REG_OP_N("isResize",isResize,0);
_REG_OP_N("orderResize",orderResize,orderResizeDef);
_REG_OP_N("fullResize",fullResize,0);
_REG_OP_N("resizeDx",resizeAspect.dx,640);
_REG_OP_N("resizeDy",resizeAspect.dy,480);       
_REG_OP_N("resizeAscpect",resizeAspect.isAspect,1);
_REG_OP_N("resizeMethod",resizeAspect.method,2);
_REG_OP_N("aspectRatio",resizeAspect.aspectRatio,1.33*65536);
_REG_OP_N("resizeGblurLum",resizeAspect.GblurLum,0);
_REG_OP_N("resizeGblurChrom",resizeAspect.GblurChrom,0);
_REG_OP_N("resizeSharpenLum",resizeAspect.sharpenLum,0);
_REG_OP_N("resizeSharpenChrom",resizeAspect.sharpenChrom,0);

_REG_OP_N("isCropNzoom",isCropNzoom,0);
_REG_OP_N("isZoom",cropNzoom.isZoom,1);
_REG_OP_N("magnificationX",cropNzoom.magnificationX,0);
_REG_OP_N("magnificationY",cropNzoom.magnificationY,0);
_REG_OP_N("magnificationLocked",cropNzoom.magnificationLocked,1);
_REG_OP_N("cropLeft"  ,cropNzoom.cropLeft,0);
_REG_OP_N("cropRight" ,cropNzoom.cropRight ,0);
_REG_OP_N("cropTop"   ,cropNzoom.cropTop   ,0);
_REG_OP_N("cropBottom",cropNzoom.cropBottom,0);
_REG_OP_N("autocrop",cropNzoom.autocrop,0);

_REG_OP_N("isSubtitles",isSubtitles,0);
_REG_OP_N("orderSubtitles",orderSubtitles,orderSubtitlesDef);
_REG_OP_N("fullSubtitles",fullSubtitles,0);
_REG_OP_S("fontName",font.name,"Arial");
_REG_OP_N("fontCharset",font.charset,DEFAULT_CHARSET);
_REG_OP_N("fontSize",font.size,26);
_REG_OP_N("fontWeight",font.weight,FW_NORMAL);if (font.weight>900) font.weight=900;
_REG_OP_N("fontColor",font.color,RGB(255,255,255));
_REG_OP_N("fontShadowStrength",font.shadowStrength,90);
_REG_OP_N("fontShadowRadius"  ,font.shadowRadius  ,50);
_REG_OP_N("fontSpacing",font.spacing,0);
_REG_OP_N("subPosX",subtitles.posX,50);
_REG_OP_N("subPosY",subtitles.posY,90);
_REG_OP_N("subDelay",subtitles.delay,subtitles.delayDef);
_REG_OP_N("subSpeed",subtitles.speed,subtitles.speedDef);
_REG_OP_S("subFlnm",subtitles.flnm,"");
_REG_OP_N("subAutoFlnm",subtitles.autoFlnm,1);

_REG_OP_N("isOffset",isOffset,0);
_REG_OP_N("orderOffset",orderOffset,orderOffsetDef);
_REG_OP_N("fullOffset",fullOffset,0);
_REG_OP_N("offsetY_X",offset.Y_X,0);
_REG_OP_N("offsetY_Y",offset.Y_Y,0);
_REG_OP_N("offsetU_X",offset.U_X,0);
_REG_OP_N("offsetU_Y",offset.U_Y,0);
_REG_OP_N("offsetV_X",offset.V_X,0);
_REG_OP_N("offsetV_Y",offset.V_Y,0);

_REG_OP_N("orderShowMV",orderShowMV,orderShowMVdef);

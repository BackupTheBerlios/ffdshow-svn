#define PARAM_GET(id,settingsClass,valName,min,max,onChange)  \
 case id:val=settingsClass##valName;break;
#define PARAM_SET(id,settingsClass,valName,min,max,_onNotify) \
 case id:                                                     \
  {                                                           \
   if (min==-1 && max==-1) break;                             \
   int *dst=(int*)&settingsClass##valName;                    \
   int oldVal=*dst;                                           \
   *dst=val;                                                  \
   if (min || max)                                            \
    {                                                         \
     if (*dst<min) *dst=min;                                  \
     if (*dst>max) *dst=max;                                  \
    }                                                         \
   void (TffDecoder::*onNotify)(void)=_onNotify;              \
   if (onNotify && oldVal!=*dst) (this->*onNotify)();         \
  }                                                           \
  break;
#pragma warning (once:4127)
//state variables
_PARAM_OP(IDFF_inPlayer           ,this->,inPlayer          ,0,0,NULL)
_PARAM_OP(IDFF_cfgDlgHnwd         ,this->,cfgDlgHnwd        ,0,0,NULL)
//dialog settings
_PARAM_OP(IDFF_lastPage           ,dialogSettings.,lastPage     ,0,0,NULL)
_PARAM_OP(IDFF_dlgRestorePos      ,dialogSettings.,dlgRestorePos,0,0,NULL)
_PARAM_OP(IDFF_dlgPosX            ,dialogSettings.,dlgPosX      ,-4096,4096,NULL)
_PARAM_OP(IDFF_dlgPosY            ,dialogSettings.,dlgPosY      ,-4096,4096,NULL)
_PARAM_OP(IDFF_lvWidth0           ,dialogSettings.,lvWidth0     ,1,2048,NULL)
_PARAM_OP(IDFF_showHints,         ,dialogSettings.,showHints    ,0,0,NULL)
//global settings
_PARAM_OP(IDFF_autoPreset         ,globalSettings.,autoPreset         ,0,0,NULL)
_PARAM_OP(IDFF_autoPresetFileFirst,globalSettings.,autoPresetFileFirst,0,0,NULL)
_PARAM_OP(IDFF_trayIcon           ,globalSettings.,trayIcon           ,0,0,&TffDecoder::onTrayIconChanged)
_PARAM_OP(IDFF_xvid               ,globalSettings.,xvid               ,0,0,NULL)
_PARAM_OP(IDFF_div3               ,globalSettings.,div3               ,0,0,NULL)
_PARAM_OP(IDFF_divx               ,globalSettings.,divx               ,0,0,NULL)
_PARAM_OP(IDFF_dx50               ,globalSettings.,dx50               ,0,0,NULL)
_PARAM_OP(IDFF_mp43               ,globalSettings.,mp43               ,0,0,NULL)
_PARAM_OP(IDFF_mp42               ,globalSettings.,mp42               ,0,0,NULL)
_PARAM_OP(IDFF_mp41               ,globalSettings.,mp41               ,0,0,NULL)
_PARAM_OP(IDFF_h263               ,globalSettings.,h263               ,0,0,NULL)
_PARAM_OP(IDFF_blz0               ,globalSettings.,blz0               ,0,0,NULL)
_PARAM_OP(IDFF_wmv1               ,globalSettings.,wmv1               ,0,0,NULL)
_PARAM_OP(IDFF_rawv               ,globalSettings.,rawv               ,0,0,NULL)
_PARAM_OP(IDFF_isShowMV           ,globalSettings.,isShowMV           ,0,0,NULL)
_PARAM_OP(IDFF_outYV12            ,globalSettings.,outYV12            ,0,0,NULL)
_PARAM_OP(IDFF_outYUY2            ,globalSettings.,outYUY2            ,0,0,NULL)
_PARAM_OP(IDFF_outYVYU            ,globalSettings.,outYVYU            ,0,0,NULL)
_PARAM_OP(IDFF_outUYVY            ,globalSettings.,outUYVY            ,0,0,NULL)
_PARAM_OP(IDFF_outRGB32           ,globalSettings.,outRGB32           ,0,0,NULL)
_PARAM_OP(IDFF_outRGB24           ,globalSettings.,outRGB24           ,0,0,NULL)
_PARAM_OP(IDFF_outRGB555          ,globalSettings.,outRGB555          ,0,0,NULL)
_PARAM_OP(IDFF_outRGB565          ,globalSettings.,outRGB565          ,0,0,NULL)

//preset settings
_PARAM_OP(IDFF_autoLoadedFromFile ,presetSettings->,autoLoadedFromFile ,0,0,NULL)

_PARAM_OP(IDFF_isPostproc         ,presetSettings->,isPostproc         ,0,0,NULL)
_PARAM_OP(IDFF_orderPostproc      ,presetSettings->,orderPostproc      ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullPostproc       ,presetSettings->,fullPostproc       ,0,0,NULL)
_PARAM_OP(IDFF_ppqual             ,presetSettings->,ppqual             ,0,6,NULL)
_PARAM_OP(IDFF_autoq              ,presetSettings->,autoq              ,0,0,NULL)
_PARAM_OP(IDFF_ppIsCustom         ,presetSettings->,ppIsCustom         ,0,0,NULL)
_PARAM_OP(IDFF_ppcustom           ,presetSettings->,ppcustom           ,0,0,NULL)
_PARAM_OP(IDFF_currentq           ,this->          ,currentq           ,0,6,NULL)
_PARAM_OP(IDFF_deblockStrength    ,presetSettings->,deblockStrength    ,0,512,NULL);
_PARAM_OP(IDFF_levelFixLum        ,presetSettings->,levelFixLum        ,0,0,NULL)
_PARAM_OP(IDFF_levelFixChrom      ,presetSettings->,levelFixChrom      ,0,0,NULL)

_PARAM_OP(IDFF_isPictProp         ,presetSettings->,isPictProp         ,0,0,NULL)
_PARAM_OP(IDFF_orderPictProp      ,presetSettings->,orderPictProp      ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullPictProp       ,presetSettings->,fullPictProp       ,0,0,NULL)
_PARAM_OP(IDFF_lumGain            ,presetSettings->,lumGain            ,0,256,NULL)
_PARAM_OP(IDFF_lumOffset          ,presetSettings->,lumOffset          ,-256,256,NULL)
_PARAM_OP(IDFF_gammaCorrection    ,presetSettings->,gammaCorrection    ,1,400,NULL)
_PARAM_OP(IDFF_hue                ,presetSettings->,hue                ,-180,180,NULL)
_PARAM_OP(IDFF_saturation         ,presetSettings->,saturation         ,0,128,NULL)
_PARAM_OP(IDFF_lumGainDef         ,TpresetSettings::,lumGainDef        ,-1,-1,NULL)
_PARAM_OP(IDFF_lumOffsetDef       ,TpresetSettings::,lumOffsetDef      ,-1,-1,NULL)
_PARAM_OP(IDFF_gammaCorrectionDef ,TpresetSettings::,gammaCorrectionDef,-1,-1,NULL)
_PARAM_OP(IDFF_hueDef             ,TpresetSettings::,hueDef            ,-1,-1,NULL)
_PARAM_OP(IDFF_saturationDef      ,TpresetSettings::,saturationDef     ,-1,-1,NULL)
                                                        
_PARAM_OP(IDFF_flip               ,presetSettings->,flip               ,0,0,NULL)

_PARAM_OP(IDFF_isBlur             ,presetSettings->,isBlur             ,0,0,NULL)
_PARAM_OP(IDFF_orderBlur          ,presetSettings->,orderBlur          ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullBlur           ,presetSettings->,fullBlur           ,0,0,NULL)
_PARAM_OP(IDFF_blurStrength       ,presetSettings->,blurStrength       ,0,255,NULL)
_PARAM_OP(IDFF_tempSmooth         ,presetSettings->,tempSmooth         ,0,10,NULL)

_PARAM_OP(IDFF_isSharpen          ,presetSettings->,isSharpen          ,0,0,NULL)
_PARAM_OP(IDFF_orderSharpen       ,presetSettings->,orderSharpen       ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullSharpen        ,presetSettings->,fullSharpen        ,0,0,NULL)
_PARAM_OP(IDFF_sharpenMethod      ,presetSettings->,sharpenMethod      ,0,1,NULL)
_PARAM_OP(IDFF_xsharp_strength    ,presetSettings->,xsharp_strength    ,1,127,NULL)
_PARAM_OP(IDFF_xsharp_threshold   ,presetSettings->,xsharp_threshold   ,0,255,NULL)
_PARAM_OP(IDFF_unsharp_strength   ,presetSettings->,unsharp_strength   ,1,127,NULL)
_PARAM_OP(IDFF_unsharp_threshold  ,presetSettings->,unsharp_threshold  ,0,255,NULL)
                                                
_PARAM_OP(IDFF_isNoise            ,presetSettings->,isNoise            ,0,0,NULL)
_PARAM_OP(IDFF_orderNoise         ,presetSettings->,orderNoise         ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullNoise          ,presetSettings->,fullNoise          ,0,0,NULL)
_PARAM_OP(IDFF_noiseMethod        ,presetSettings->,noiseMethod        ,0,1,NULL)
_PARAM_OP(IDFF_uniformNoise       ,presetSettings->,uniformNoise       ,0,0,NULL)
_PARAM_OP(IDFF_noiseStrength      ,presetSettings->,noiseStrength      ,0,255,NULL)
_PARAM_OP(IDFF_noiseStrengthChroma,presetSettings->,noiseStrengthChroma,0,127,NULL)
                                                
_PARAM_OP(IDFF_idct               ,presetSettings->,idct               ,0,4,NULL)
                                                
_PARAM_OP(IDFF_isResize           ,presetSettings->,isResize           ,0,0,NULL)
_PARAM_OP(IDFF_orderResize        ,presetSettings->,orderResize        ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullResize         ,presetSettings->,fullResize         ,0,0,NULL)
_PARAM_OP(IDFF_resizeDx           ,presetSettings->,resizeDx           ,16,2048,NULL)
_PARAM_OP(IDFF_resizeDy           ,presetSettings->,resizeDy           ,16,2048,NULL)
_PARAM_OP(IDFF_resizeAspect       ,presetSettings->,resizeAspect       ,0,0,NULL)
_PARAM_OP(IDFF_resizeMethod       ,presetSettings->,resizeMethod       ,0,TpresetSettings::resizeMethodNone,NULL)
_PARAM_OP(IDFF_aspectRatio        ,presetSettings->,aspectRatio        ,0,0,NULL)
_PARAM_OP(IDFF_resizeGblurLum     ,presetSettings->,resizeGblurLum     ,0,200,NULL)
_PARAM_OP(IDFF_resizeGblurChrom   ,presetSettings->,resizeGblurChrom   ,0,200,NULL)
_PARAM_OP(IDFF_resizeSharpenLum   ,presetSettings->,resizeSharpenLum   ,0,200,NULL)
_PARAM_OP(IDFF_resizeSharpenChrom ,presetSettings->,resizeSharpenChrom ,0,200,NULL)

_PARAM_OP(IDFF_isCropNzoom        ,presetSettings->,isCropNzoom        ,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_isZoom             ,presetSettings->,isZoom             ,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_magnificationX     ,presetSettings->,magnificationX     ,0,100,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_magnificationY     ,presetSettings->,magnificationY     ,0,100,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_magnificationLocked,presetSettings->,magnificationLocked,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropLeft           ,presetSettings->,cropLeft           ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropRight          ,presetSettings->,cropRight          ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropTop            ,presetSettings->,cropTop            ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropBottom         ,presetSettings->,cropBottom         ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_autocrop           ,presetSettings->,autocrop           ,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropChanged,       ,this->          ,cropChanged        ,0,0,NULL)

_PARAM_OP(IDFF_isSubtitles        ,presetSettings->,isSubtitles        ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_orderSubtitles     ,presetSettings->,orderSubtitles     ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullSubtitles      ,presetSettings->,fullSubtitles      ,0,0,NULL)
_PARAM_OP(IDFF_fontCharset        ,presetSettings->,fontCharset        ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontSize           ,presetSettings->,fontSize           ,2,255,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontWeight         ,presetSettings->,fontWeight         ,0,900,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontColor          ,presetSettings->,fontColor          ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontShadowStrength ,presetSettings->,fontShadowStrength ,0,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontShadowRadius   ,presetSettings->,fontShadowRadius   ,1,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontSpacing        ,presetSettings->,fontSpacing        ,-10,10,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subPosX            ,presetSettings->,subPosX            ,0,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subPosY            ,presetSettings->,subPosY            ,0,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subDelay           ,presetSettings->,subDelay           ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subDelayDef        ,TpresetSettings::,subDelayDef       ,-1,-1,NULL)
_PARAM_OP(IDFF_subSpeed           ,presetSettings->,subSpeed           ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subSpeedDef        ,TpresetSettings::,subSpeedDef       ,-1,-1,NULL)
_PARAM_OP(IDFF_subAutoFlnm        ,presetSettings->,subAutoFlnm        ,0,0,NULL)
_PARAM_OP(IDFF_fontChanged,       ,this->          ,fontChanged        ,0,0,NULL)

_PARAM_OP(IDFF_isOffset           ,presetSettings->,isOffset           ,0,0,NULL);
_PARAM_OP(IDFF_orderOffset        ,presetSettings->,orderOffset        ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullOffset         ,presetSettings->,fullOffset         ,0,0,NULL)
_PARAM_OP(IDFF_offsetY_X          ,presetSettings->,offsetY_X          ,-32,32,NULL);
_PARAM_OP(IDFF_offsetY_Y          ,presetSettings->,offsetY_Y          ,-32,32,NULL);
_PARAM_OP(IDFF_offsetU_X          ,presetSettings->,offsetU_X          ,-32,32,NULL);
_PARAM_OP(IDFF_offsetU_Y          ,presetSettings->,offsetU_Y          ,-32,32,NULL);
_PARAM_OP(IDFF_offsetV_X          ,presetSettings->,offsetV_X          ,-32,32,NULL);
_PARAM_OP(IDFF_offsetV_Y          ,presetSettings->,offsetV_Y          ,-32,32,NULL);

_PARAM_OP(IDFF_orderShowMV        ,presetSettings->,orderShowMV        ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)

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

_PARAM_OP(IDFF_isPostproc         ,presetSettings->,isPostproc              ,0,0,NULL)
_PARAM_OP(IDFF_orderPostproc      ,presetSettings->,orderPostproc           ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullPostproc       ,presetSettings->,fullPostproc            ,0,0,NULL)
_PARAM_OP(IDFF_ppqual             ,presetSettings->,postproc.qual           ,0,6,NULL)
_PARAM_OP(IDFF_autoq              ,presetSettings->,postproc.autoq          ,0,0,NULL)
_PARAM_OP(IDFF_ppIsCustom         ,presetSettings->,postproc.isCustom       ,0,0,NULL)
_PARAM_OP(IDFF_ppcustom           ,presetSettings->,postproc.custom         ,0,0,NULL)
_PARAM_OP(IDFF_currentq           ,this->          ,currentq                ,0,6,NULL)
_PARAM_OP(IDFF_deblockStrength    ,presetSettings->,postproc.deblockStrength,0,512,NULL);
_PARAM_OP(IDFF_levelFixLum        ,presetSettings->,postproc.levelFixLum    ,0,0,NULL)
_PARAM_OP(IDFF_levelFixChrom      ,presetSettings->,postproc.levelFixChrom  ,0,0,NULL)

_PARAM_OP(IDFF_isDeinterlace      ,presetSettings->,isDeinterlace      ,0,0,NULL)

_PARAM_OP(IDFF_isPictProp         ,presetSettings->,isPictProp                            ,0,0,NULL)
_PARAM_OP(IDFF_orderPictProp      ,presetSettings->,orderPictProp                         ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullPictProp       ,presetSettings->,fullPictProp                          ,0,0,NULL)
_PARAM_OP(IDFF_lumGain            ,presetSettings->,pictProp.lumGain                      ,0,256,NULL)
_PARAM_OP(IDFF_lumOffset          ,presetSettings->,pictProp.lumOffset                    ,-256,256,NULL)
_PARAM_OP(IDFF_gammaCorrection    ,presetSettings->,pictProp.gammaCorrection              ,1,400,NULL)
_PARAM_OP(IDFF_hue                ,presetSettings->,pictProp.hue                          ,-180,180,NULL)
_PARAM_OP(IDFF_saturation         ,presetSettings->,pictProp.saturation                   ,0,128,NULL)
_PARAM_OP(IDFF_lumGainDef         ,TpresetSettings::,TpictPropSettings::lumGainDef        ,-1,-1,NULL)
_PARAM_OP(IDFF_lumOffsetDef       ,TpresetSettings::,TpictPropSettings::lumOffsetDef      ,-1,-1,NULL)
_PARAM_OP(IDFF_gammaCorrectionDef ,TpresetSettings::,TpictPropSettings::gammaCorrectionDef,-1,-1,NULL)
_PARAM_OP(IDFF_hueDef             ,TpresetSettings::,TpictPropSettings::hueDef            ,-1,-1,NULL)
_PARAM_OP(IDFF_saturationDef      ,TpresetSettings::,TpictPropSettings::saturationDef     ,-1,-1,NULL)
                                                        
_PARAM_OP(IDFF_flip               ,presetSettings->,flip               ,0,0,NULL)

_PARAM_OP(IDFF_isBlur             ,presetSettings->,isBlur         ,0,0,NULL)
_PARAM_OP(IDFF_orderBlur          ,presetSettings->,orderBlur      ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullBlur           ,presetSettings->,fullBlur       ,0,0,NULL)
_PARAM_OP(IDFF_blurStrength       ,presetSettings->,blur.strength  ,0,255,NULL)
_PARAM_OP(IDFF_tempSmooth         ,presetSettings->,blur.tempSmooth,0,10,NULL)

_PARAM_OP(IDFF_isSharpen          ,presetSettings->,isSharpen               ,0,0,NULL)
_PARAM_OP(IDFF_orderSharpen       ,presetSettings->,orderSharpen            ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullSharpen        ,presetSettings->,fullSharpen             ,0,0,NULL)
_PARAM_OP(IDFF_sharpenMethod      ,presetSettings->,sharpen.method          ,0,1,NULL)
_PARAM_OP(IDFF_xsharp_strength    ,presetSettings->,sharpen.xsharpStrength  ,1,127,NULL)
_PARAM_OP(IDFF_xsharp_threshold   ,presetSettings->,sharpen.xsharpThreshold ,0,255,NULL)
_PARAM_OP(IDFF_unsharp_strength   ,presetSettings->,sharpen.unsharpStrength ,1,127,NULL)
_PARAM_OP(IDFF_unsharp_threshold  ,presetSettings->,sharpen.unsharpThreshold,0,255,NULL)
                                                
_PARAM_OP(IDFF_isNoise            ,presetSettings->,isNoise             ,0,0,NULL)
_PARAM_OP(IDFF_orderNoise         ,presetSettings->,orderNoise          ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullNoise          ,presetSettings->,fullNoise           ,0,0,NULL)
_PARAM_OP(IDFF_noiseMethod        ,presetSettings->,noise.method        ,0,1,NULL)
_PARAM_OP(IDFF_uniformNoise       ,presetSettings->,noise.uniformNoise  ,0,0,NULL)
_PARAM_OP(IDFF_noiseStrength      ,presetSettings->,noise.strength      ,0,255,NULL)
_PARAM_OP(IDFF_noiseStrengthChroma,presetSettings->,noise.strengthChroma,0,127,NULL)
                                                
_PARAM_OP(IDFF_idct               ,presetSettings->,idct               ,0,4,NULL)
                                                
_PARAM_OP(IDFF_isResize           ,presetSettings->,isResize                 ,0,0,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_orderResize        ,presetSettings->,orderResize              ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullResize         ,presetSettings->,fullResize               ,0,0,NULL)
_PARAM_OP(IDFF_resizeDx           ,presetSettings->,resizeAspect.dx          ,16,2048,NULL)
_PARAM_OP(IDFF_resizeDy           ,presetSettings->,resizeAspect.dy          ,16,2048,NULL)
_PARAM_OP(IDFF_isAspect           ,presetSettings->,resizeAspect.isAspect    ,0,0,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_aspectRatio        ,presetSettings->,resizeAspect.aspectRatio ,0,0,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_resizeMethod       ,presetSettings->,resizeAspect.method      ,0,TpresetSettings::TresizeAspectSettings::methodNone,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_resizeGblurLum     ,presetSettings->,resizeAspect.GblurLum    ,0,200,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_resizeGblurChrom   ,presetSettings->,resizeAspect.GblurChrom  ,0,200,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_resizeSharpenLum   ,presetSettings->,resizeAspect.sharpenLum  ,0,200,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_resizeSharpenChrom ,presetSettings->,resizeAspect.sharpenChrom,0,200,&TffDecoder::onResizeChanged)
_PARAM_OP(IDFF_resizeChanged,     ,this->          ,resizeChanged            ,0,0,NULL)

_PARAM_OP(IDFF_isCropNzoom        ,presetSettings->,isCropNzoom                  ,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_isZoom             ,presetSettings->,cropNzoom.isZoom             ,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_magnificationX     ,presetSettings->,cropNzoom.magnificationX     ,0,100,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_magnificationY     ,presetSettings->,cropNzoom.magnificationY     ,0,100,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_magnificationLocked,presetSettings->,cropNzoom.magnificationLocked,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropLeft           ,presetSettings->,cropNzoom.cropLeft           ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropRight          ,presetSettings->,cropNzoom.cropRight          ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropTop            ,presetSettings->,cropNzoom.cropTop            ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropBottom         ,presetSettings->,cropNzoom.cropBottom         ,0,2048,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_autocrop           ,presetSettings->,cropNzoom.autocrop           ,0,0,&TffDecoder::onCropChanged)
_PARAM_OP(IDFF_cropChanged,       ,this->          ,cropChanged                  ,0,0,NULL)

_PARAM_OP(IDFF_isSubtitles        ,presetSettings->,isSubtitles                  ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_orderSubtitles     ,presetSettings->,orderSubtitles               ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullSubtitles      ,presetSettings->,fullSubtitles                ,0,0,NULL)
_PARAM_OP(IDFF_fontCharset        ,presetSettings->,font.charset                 ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontSize           ,presetSettings->,font.size                    ,2,255,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontWeight         ,presetSettings->,font.weight                  ,0,900,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontColor          ,presetSettings->,font.color                   ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontShadowStrength ,presetSettings->,font.shadowStrength          ,0,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontShadowRadius   ,presetSettings->,font.shadowRadius            ,1,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_fontSpacing        ,presetSettings->,font.spacing                 ,-10,10,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subAutoFlnm        ,presetSettings->,subtitles.autoFlnm           ,0,0,NULL)
_PARAM_OP(IDFF_subPosX            ,presetSettings->,subtitles.posX               ,0,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subPosY            ,presetSettings->,subtitles.posY               ,0,100,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subDelay           ,presetSettings->,subtitles.delay              ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subSpeed           ,presetSettings->,subtitles.speed              ,0,0,&TffDecoder::onSubsChanged)
_PARAM_OP(IDFF_subDelayDef        ,TpresetSettings::,TsubtitlesSettings::delayDef,-1,-1,NULL)
_PARAM_OP(IDFF_subSpeedDef        ,TpresetSettings::,TsubtitlesSettings::speedDef,-1,-1,NULL)
_PARAM_OP(IDFF_fontChanged,       ,this->          ,fontChanged                  ,0,0,NULL)

_PARAM_OP(IDFF_isOffset           ,presetSettings->,isOffset   ,0,0,NULL);
_PARAM_OP(IDFF_orderOffset        ,presetSettings->,orderOffset,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
_PARAM_OP(IDFF_fullOffset         ,presetSettings->,fullOffset ,0,0,NULL)
_PARAM_OP(IDFF_offsetY_X          ,presetSettings->,offset.Y_X ,-32,32,NULL);
_PARAM_OP(IDFF_offsetY_Y          ,presetSettings->,offset.Y_Y ,-32,32,NULL);
_PARAM_OP(IDFF_offsetU_X          ,presetSettings->,offset.U_X ,-32,32,NULL);
_PARAM_OP(IDFF_offsetU_Y          ,presetSettings->,offset.U_Y ,-32,32,NULL);
_PARAM_OP(IDFF_offsetV_X          ,presetSettings->,offset.V_X ,-32,32,NULL);
_PARAM_OP(IDFF_offsetV_Y          ,presetSettings->,offset.V_Y ,-32,32,NULL);

_PARAM_OP(IDFF_orderShowMV        ,presetSettings->,orderShowMV        ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)

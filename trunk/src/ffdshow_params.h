#define PARAM_GET(id,settingsClass,valName,min,max,onChange)  \
 case id:val=settingsClass##valName;break;
#define PARAM_SET(id,settingsClass,valName,min,max,_onNotify) \
 case id:                                                     \
  {                                                           \
   if (min==-1 && max==-1) break;                             \
   int &dst=int(settingsClass##valName);                      \
   int oldVal=dst;                                            \
   dst=val;                                                   \
   if (min || max)                                            \
    {                                                         \
     if (dst<min) dst=min;                                    \
     if (dst>max) dst=max;                                    \
    }                                                         \
   void (TffDecoder::*onNotify)(void)=_onNotify;              \
   if (onNotify && oldVal!=dst) (this->*onNotify)();          \
  }                                                           \
  break;

//state variables
PARAM(IDFF_inPlayer           ,this->,inPlayer          ,0,0,NULL)
PARAM(IDFF_isDlg              ,this->,isDlg             ,0,0,NULL)
//dialog settings
PARAM(IDFF_lastPage           ,dialogSettings.,lastPage,0,0,NULL)
//global settings
PARAM(IDFF_autoPreset         ,globalSettings.,autoPreset         ,0,0,NULL)
PARAM(IDFF_trayIcon           ,globalSettings.,trayIcon           ,0,0,&TffDecoder::trayIconChanged)
PARAM(IDFF_autoPresetFileFirst,globalSettings.,autoPresetFileFirst,0,0,NULL)
PARAM(IDFF_xvid               ,globalSettings.,xvid               ,0,0,NULL)
PARAM(IDFF_div3               ,globalSettings.,div3               ,0,0,NULL)
PARAM(IDFF_divx               ,globalSettings.,divx               ,0,0,NULL)
PARAM(IDFF_dx50               ,globalSettings.,dx50               ,0,0,NULL)
PARAM(IDFF_mp43               ,globalSettings.,mp43               ,0,0,NULL)
PARAM(IDFF_mp42               ,globalSettings.,mp42               ,0,0,NULL)
PARAM(IDFF_mp41               ,globalSettings.,mp41               ,0,0,NULL)
PARAM(IDFF_h263               ,globalSettings.,h263               ,0,0,NULL)

PARAM(IDFF_autoLoadedFromFile ,presetSettings->,autoLoadedFromFile ,0,0,NULL)
PARAM(IDFF_autoLoadedFromReg  ,presetSettings->,autoLoadedFromReg  ,0,0,NULL)

PARAM(IDFF_isPostproc         ,presetSettings->,isPostproc         ,0,0,NULL)
PARAM(IDFF_orderPostproc      ,presetSettings->,orderPostproc      ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
PARAM(IDFF_ppqual             ,presetSettings->,ppqual             ,0,6,NULL)
PARAM(IDFF_autoq              ,presetSettings->,autoq              ,0,0,NULL)
PARAM(IDFF_ppIsCustom         ,presetSettings->,ppIsCustom         ,0,0,NULL)
PARAM(IDFF_ppcustom           ,presetSettings->,ppcustom           ,0,0,NULL)
PARAM(IDFF_currentq           ,presetSettings->,currentq           ,0,6,NULL)
PARAM(IDFF_levelFixLum        ,presetSettings->,levelFixLum        ,0,0,NULL)
PARAM(IDFF_levelFixChrom      ,presetSettings->,levelFixChrom      ,0,0,NULL)

PARAM(IDFF_isPictProp         ,presetSettings->,isPictProp         ,0,0,NULL)
PARAM(IDFF_orderPictProp      ,presetSettings->,orderPictProp      ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
PARAM(IDFF_lumGain            ,presetSettings->,lumGain            ,0,256,NULL)
PARAM(IDFF_lumOffset          ,presetSettings->,lumOffset          ,-256,256,NULL)
PARAM(IDFF_gammaCorrection    ,presetSettings->,gammaCorrection    ,1,400,NULL)
PARAM(IDFF_hue                ,presetSettings->,hue                ,-180,180,NULL)
PARAM(IDFF_saturation         ,presetSettings->,saturation         ,0,128,NULL)
PARAM(IDFF_lumGainDef         ,TpresetSettings::,lumGainDef,-1,-1,NULL)
PARAM(IDFF_lumOffsetDef       ,TpresetSettings::,lumOffsetDef,-1,-1,NULL)
PARAM(IDFF_gammaCorrectionDef ,TpresetSettings::,gammaCorrectionDef,-1,-1,NULL)
PARAM(IDFF_hueDef             ,TpresetSettings::,hueDef,-1,-1,NULL)
PARAM(IDFF_saturationDef      ,TpresetSettings::,saturationDef,-1,-1,NULL)
                                                        
PARAM(IDFF_flip               ,presetSettings->,flip               ,0,0,NULL)

PARAM(IDFF_isBlur             ,presetSettings->,isBlur             ,0,0,NULL)
PARAM(IDFF_orderBlur          ,presetSettings->,orderBlur          ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
PARAM(IDFF_blurStrength       ,presetSettings->,blurStrength       ,1,255,NULL)

PARAM(IDFF_isSharpen          ,presetSettings->,isSharpen          ,0,0,NULL)
PARAM(IDFF_orderSharpen       ,presetSettings->,orderSharpen       ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
PARAM(IDFF_sharpenMethod      ,presetSettings->,sharpenMethod      ,0,1,NULL)
PARAM(IDFF_xsharp_strength    ,presetSettings->,xsharp_strength    ,1,127,NULL)
PARAM(IDFF_xsharp_threshold   ,presetSettings->,xsharp_threshold   ,0,255,NULL)
PARAM(IDFF_unsharp_strength   ,presetSettings->,unsharp_strength   ,1,127,NULL)
PARAM(IDFF_unsharp_threshold  ,presetSettings->,unsharp_threshold  ,0,255,NULL)
                                                
PARAM(IDFF_isNoise            ,presetSettings->,isNoise            ,0,0,NULL)
PARAM(IDFF_orderNoise         ,presetSettings->,orderNoise         ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
PARAM(IDFF_noiseMethod        ,presetSettings->,noiseMethod        ,0,1,NULL)
PARAM(IDFF_uniformNoise       ,presetSettings->,uniformNoise       ,0,0,NULL)
PARAM(IDFF_noiseStrength      ,presetSettings->,noiseStrength      ,0,255,NULL)
PARAM(IDFF_noiseStrengthChroma,presetSettings->,noiseStrengthChroma,0,127,NULL)
                                                
PARAM(IDFF_idct               ,presetSettings->,idct               ,0,4,NULL)
                                                
PARAM(IDFF_isResize           ,presetSettings->,isResize           ,0,0,NULL)
PARAM(IDFF_resizeDx           ,presetSettings->,resizeDx           ,8,2048,NULL)
PARAM(IDFF_resizeDy           ,presetSettings->,resizeDy           ,8,2048,NULL)
PARAM(IDFF_resizeAspect       ,presetSettings->,resizeAspect       ,0,0,&TffDecoder::resizeChanged)
PARAM(IDFF_resizeFirst        ,presetSettings->,resizeFirst        ,0,0,&TffDecoder::resizeChanged)
PARAM(IDFF_resizeMethod       ,presetSettings->,resizeMethod       ,0,5,&TffDecoder::resizeChanged)
PARAM(IDFF_aspectRatio        ,presetSettings->,aspectRatio        ,0,0,&TffDecoder::resizeChanged)
PARAM(IDFF_resizeGblurLum     ,presetSettings->,resizeGblurLum     ,0,200,&TffDecoder::resizeChanged)
PARAM(IDFF_resizeGblurChrom   ,presetSettings->,resizeGblurChrom   ,0,200,&TffDecoder::resizeChanged)
PARAM(IDFF_resizeSharpenLum   ,presetSettings->,resizeSharpenLum   ,0,200,&TffDecoder::resizeChanged)
PARAM(IDFF_resizeSharpenChrom ,presetSettings->,resizeSharpenChrom ,0,200,&TffDecoder::resizeChanged)
PARAM(IDFF_isCropNzoom        ,presetSettings->,isCropNzoom        ,0,0,&TffDecoder::resizeChanged)
PARAM(IDFF_isZoom             ,presetSettings->,isZoom             ,0,0,&TffDecoder::resizeChanged)
PARAM(IDFF_magnificationX     ,presetSettings->,magnificationX     ,0,100,&TffDecoder::resizeChanged)
PARAM(IDFF_magnificationY     ,presetSettings->,magnificationY     ,0,100,&TffDecoder::resizeChanged)
PARAM(IDFF_magnificationLocked,presetSettings->,magnificationLocked,0,0,&TffDecoder::resizeChanged)
PARAM(IDFF_cropLeft           ,presetSettings->,cropLeft           ,0,2048,&TffDecoder::resizeChanged)
PARAM(IDFF_cropRight          ,presetSettings->,cropRight          ,0,2048,&TffDecoder::resizeChanged)
PARAM(IDFF_cropTop            ,presetSettings->,cropTop            ,0,2048,&TffDecoder::resizeChanged)
PARAM(IDFF_cropBottom         ,presetSettings->,cropBottom         ,0,2048,&TffDecoder::resizeChanged)
PARAM(IDFF_autocrop           ,presetSettings->,autocrop           ,0,0,&TffDecoder::resizeChanged)

PARAM(IDFF_isSubtitles        ,presetSettings->,isSubtitles        ,0,0,&TffDecoder::subsChanged)
PARAM(IDFF_orderSubtitles     ,presetSettings->,orderSubtitles     ,TpresetSettings::min_order,TpresetSettings::max_order,NULL)
PARAM(IDFF_fontCharset        ,presetSettings->,fontCharset        ,0,0,&TffDecoder::subsChanged)
PARAM(IDFF_fontSize           ,presetSettings->,fontSize           ,2,255,&TffDecoder::subsChanged)
PARAM(IDFF_fontWeight         ,presetSettings->,fontWeight         ,0,1000,&TffDecoder::subsChanged)
PARAM(IDFF_fontColor          ,presetSettings->,fontColor          ,0,0,&TffDecoder::subsChanged)
PARAM(IDFF_fontShadowStrength ,presetSettings->,fontShadowStrength ,0,100,&TffDecoder::subsChanged)
PARAM(IDFF_fontShadowRadius   ,presetSettings->,fontShadowRadius   ,1,100,&TffDecoder::subsChanged)
PARAM(IDFF_fontAutosize       ,presetSettings->,fontAutosize       ,0,0,&TffDecoder::subsChanged)
PARAM(IDFF_fontSpacing        ,presetSettings->,fontSpacing        ,-10,10,&TffDecoder::subsChanged)
PARAM(IDFF_subPosX            ,presetSettings->,subPosX            ,0,100,&TffDecoder::subsChanged)
PARAM(IDFF_subPosY            ,presetSettings->,subPosY            ,0,100,&TffDecoder::subsChanged)
PARAM(IDFF_subDelay           ,presetSettings->,subDelay           ,0,0,&TffDecoder::subsChanged)
PARAM(IDFF_subSpeed           ,presetSettings->,subSpeed           ,0,0,&TffDecoder::subsChanged)
PARAM(IDFF_subAutoFlnm        ,presetSettings->,subAutoFlnm        ,0,0,NULL)

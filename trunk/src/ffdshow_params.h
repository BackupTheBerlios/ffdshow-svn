//state variables
params[IDFF_inPlayer           ]=Tparam(&inPlayer               ,0,0);
params[IDFF_isDlg              ]=Tparam(&isDlg                  ,0,0);
//dialog settings
params[IDFF_lastPage           ]=Tparam(&dialogSettings.lastPage,0,0);
//global settings
params[IDFF_autoPreset         ]=Tparam(&globalSettings.autoPreset         ,0,0);
params[IDFF_trayIcon           ]=Tparam(&globalSettings.trayIcon           ,0,0,&TffDecoder::trayIconChanged);
params[IDFF_autoPresetFileFirst]=Tparam(&globalSettings.autoPresetFileFirst,0,0);
params[IDFF_xvid               ]=Tparam(&globalSettings.xvid               ,0,0);
params[IDFF_div3               ]=Tparam(&globalSettings.div3               ,0,0);
params[IDFF_divx               ]=Tparam(&globalSettings.divx               ,0,0);
params[IDFF_dx50               ]=Tparam(&globalSettings.dx50               ,0,0);
params[IDFF_mp43               ]=Tparam(&globalSettings.mp43               ,0,0);
params[IDFF_mp42               ]=Tparam(&globalSettings.mp42               ,0,0);
params[IDFF_mp41               ]=Tparam(&globalSettings.mp41               ,0,0);
params[IDFF_h263               ]=Tparam(&globalSettings.h263               ,0,0);

params[IDFF_presetShouldBeSaved]=Tparam(&presetSettings.presetShouldBeSaved,0,0);
params[IDFF_autoloadedfromreg  ]=Tparam(&presetSettings.autoloadedfromreg  ,0,0);

params[IDFF_isPostproc         ]=Tparam(&presetSettings.isPostproc         ,0,0);
params[IDFF_orderPostproc      ]=Tparam(&presetSettings.orderPostproc      ,TpresetSettings::min_order,TpresetSettings::max_order);
params[IDFF_ppqual             ]=Tparam(&presetSettings.ppqual             ,0,6);
params[IDFF_autoq              ]=Tparam(&presetSettings.autoq              ,0,0);
params[IDFF_ppIsCustom         ]=Tparam(&presetSettings.ppIsCustom         ,0,0);
params[IDFF_ppcustom           ]=Tparam(&presetSettings.ppcustom           ,0,0);
params[IDFF_currentq           ]=Tparam(&presetSettings.currentq           ,0,6);
params[IDFF_levelFixLum        ]=Tparam(&presetSettings.levelFixLum        ,0,0);
params[IDFF_levelFixChrom      ]=Tparam(&presetSettings.levelFixChrom      ,0,0);

params[IDFF_isPictProp         ]=Tparam(&presetSettings.isPictProp         ,0,0);
params[IDFF_orderPictProp      ]=Tparam(&presetSettings.orderPictProp      ,TpresetSettings::min_order,TpresetSettings::max_order);
params[IDFF_lumGain            ]=Tparam(&presetSettings.lumGain            ,0,256);
params[IDFF_lumOffset          ]=Tparam(&presetSettings.lumOffset          ,-256,256);
params[IDFF_gammaCorrection    ]=Tparam(&presetSettings.gammaCorrection    ,1,400);
params[IDFF_hue                ]=Tparam(&presetSettings.hue                ,-180,180);
params[IDFF_saturation         ]=Tparam(&presetSettings.saturation         ,0,128);
params[IDFF_lumGainDef         ]=Tparam(&TpresetSettings::lumGainDef);
params[IDFF_lumOffsetDef       ]=Tparam(&TpresetSettings::lumOffsetDef);
params[IDFF_gammaCorrectionDef ]=Tparam(&TpresetSettings::gammaCorrectionDef);
params[IDFF_hueDef             ]=Tparam(&TpresetSettings::hueDef);
params[IDFF_saturationDef      ]=Tparam(&TpresetSettings::saturationDef);
                                                        
params[IDFF_flip               ]=Tparam(&presetSettings.flip               ,0,0);

params[IDFF_isBlur             ]=Tparam(&presetSettings.isBlur             ,0,0);
params[IDFF_orderBlur          ]=Tparam(&presetSettings.orderBlur          ,TpresetSettings::min_order,TpresetSettings::max_order);
params[IDFF_blurStrength       ]=Tparam(&presetSettings.blurStrength       ,1,255);

params[IDFF_isSharpen          ]=Tparam(&presetSettings.isSharpen          ,0,0);
params[IDFF_orderSharpen       ]=Tparam(&presetSettings.orderSharpen       ,TpresetSettings::min_order,TpresetSettings::max_order);
params[IDFF_sharpenMethod      ]=Tparam(&presetSettings.sharpenMethod      ,0,1);
params[IDFF_xsharp_strength    ]=Tparam(&presetSettings.xsharp_strength    ,1,127);
params[IDFF_xsharp_threshold   ]=Tparam(&presetSettings.xsharp_threshold   ,0,255);
params[IDFF_unsharp_strength   ]=Tparam(&presetSettings.unsharp_strength   ,1,127);
params[IDFF_unsharp_threshold  ]=Tparam(&presetSettings.unsharp_threshold  ,0,255);
                                                        
params[IDFF_isNoise            ]=Tparam(&presetSettings.isNoise            ,0,0);
params[IDFF_orderNoise         ]=Tparam(&presetSettings.orderNoise         ,TpresetSettings::min_order,TpresetSettings::max_order);
params[IDFF_noiseMethod        ]=Tparam(&presetSettings.noiseMethod        ,0,1);
params[IDFF_uniformNoise       ]=Tparam(&presetSettings.uniformNoise       ,0,0);
params[IDFF_noiseStrength      ]=Tparam(&presetSettings.noiseStrength      ,0,255);
params[IDFF_noiseStrengthChroma]=Tparam(&presetSettings.noiseStrengthChroma,0,127);
                                                        
params[IDFF_idct               ]=Tparam(&presetSettings.idct               ,0,4);
                                                        
params[IDFF_isResize           ]=Tparam(&presetSettings.isResize           ,0,0);
params[IDFF_resizeDx           ]=Tparam(&presetSettings.resizeDx           ,8,2048);
params[IDFF_resizeDy           ]=Tparam(&presetSettings.resizeDy           ,8,2048);
params[IDFF_resizeAspect       ]=Tparam(&presetSettings.resizeAspect       ,0,0,&TffDecoder::resizeChanged);
params[IDFF_resizeFirst        ]=Tparam(&presetSettings.resizeFirst        ,0,0,&TffDecoder::resizeChanged);
params[IDFF_resizeMethod       ]=Tparam(&presetSettings.resizeMethod       ,0,5,&TffDecoder::resizeChanged);
params[IDFF_aspectRatio        ]=Tparam(&presetSettings.aspectRatio        ,0,0,&TffDecoder::resizeChanged);
params[IDFF_resizeGblurLum     ]=Tparam(&presetSettings.resizeGblurLum     ,0,200,&TffDecoder::resizeChanged);
params[IDFF_resizeGblurChrom   ]=Tparam(&presetSettings.resizeGblurChrom   ,0,200,&TffDecoder::resizeChanged);
params[IDFF_resizeSharpenLum   ]=Tparam(&presetSettings.resizeSharpenLum   ,0,200,&TffDecoder::resizeChanged);
params[IDFF_resizeSharpenChrom ]=Tparam(&presetSettings.resizeSharpenChrom ,0,200,&TffDecoder::resizeChanged);
params[IDFF_isCropNzoom        ]=Tparam(&presetSettings.isCropNzoom        ,0,0,&TffDecoder::resizeChanged);
params[IDFF_isZoom             ]=Tparam(&presetSettings.isZoom             ,0,0,&TffDecoder::resizeChanged);
params[IDFF_magnificationX     ]=Tparam(&presetSettings.magnificationX     ,0,100,&TffDecoder::resizeChanged);
params[IDFF_magnificationY     ]=Tparam(&presetSettings.magnificationY     ,0,100,&TffDecoder::resizeChanged);
params[IDFF_magnificationLocked]=Tparam(&presetSettings.magnificationLocked,0,0,&TffDecoder::resizeChanged);
params[IDFF_cropLeft           ]=Tparam(&presetSettings.cropLeft           ,0,2048,&TffDecoder::resizeChanged);
params[IDFF_cropRight          ]=Tparam(&presetSettings.cropRight          ,0,2048,&TffDecoder::resizeChanged);
params[IDFF_cropTop            ]=Tparam(&presetSettings.cropTop            ,0,2048,&TffDecoder::resizeChanged);
params[IDFF_cropBottom         ]=Tparam(&presetSettings.cropBottom         ,0,2048,&TffDecoder::resizeChanged);
params[IDFF_autocrop           ]=Tparam(&presetSettings.autocrop           ,0,0,&TffDecoder::resizeChanged);

params[IDFF_isSubtitles        ]=Tparam(&presetSettings.isSubtitles        ,0,0,&TffDecoder::subsChanged);
params[IDFF_orderSubtitles     ]=Tparam(&presetSettings.orderSubtitles     ,TpresetSettings::min_order,TpresetSettings::max_order);
params[IDFF_fontCharset        ]=Tparam(&presetSettings.fontCharset        ,0,0,&TffDecoder::subsChanged);
params[IDFF_fontSize           ]=Tparam(&presetSettings.fontSize           ,2,255,&TffDecoder::subsChanged);
params[IDFF_fontWeight         ]=Tparam(&presetSettings.fontWeight         ,0,1000,&TffDecoder::subsChanged);
params[IDFF_fontColor          ]=Tparam(&presetSettings.fontColor          ,0,0,&TffDecoder::subsChanged);
params[IDFF_fontShadowStrength ]=Tparam(&presetSettings.fontShadowStrength ,0,100,&TffDecoder::subsChanged);
params[IDFF_fontShadowRadius   ]=Tparam(&presetSettings.fontShadowRadius   ,1,100,&TffDecoder::subsChanged);
params[IDFF_fontAutosize       ]=Tparam(&presetSettings.fontAutosize       ,0,0,&TffDecoder::subsChanged);
params[IDFF_fontSpacing        ]=Tparam(&presetSettings.fontSpacing        ,-10,10,&TffDecoder::subsChanged);
params[IDFF_subPosX            ]=Tparam(&presetSettings.subPosX            ,0,100,&TffDecoder::subsChanged);
params[IDFF_subPosY            ]=Tparam(&presetSettings.subPosY            ,0,100,&TffDecoder::subsChanged);
params[IDFF_subDelay           ]=Tparam(&presetSettings.subDelay           ,0,0,&TffDecoder::subsChanged);
params[IDFF_subSpeed           ]=Tparam(&presetSettings.subSpeed           ,0,0,&TffDecoder::subsChanged);
params[IDFF_subAutoFlnm        ]=Tparam(&presetSettings.subAutoFlnm        ,0,0);

1. About ffdshow 

ffdshow is a DirectShow decoding filter for decompressing DIVX movies.
It uses libavcodec from ffmpeg project for video decompression, 
postprocessing code from mplayer to enhance visual quality of low bitrate
movies, and is based on original DirectShow filter from XviD, which is
GPL'ed educational implementation of MPEG4 encoder.

2. Features

- fast video decompression using optimized MMX, SSE and 3DNow! code
- support for different codecs: XviD and all DIVX versions
- additional support for MSMPEG4v2, MSMPEG4v3 and H263
- image postprocessing for higher playback quality
- automatic quality control: automatically reduces postprocessing level when CPU load is high
- hue, saturation and luminance correction (MMX optimized)
- completely free software: ffdshow is distributed under GPL 
- two sharpening filters: xsharpen and unsharp mask
- tray icon
- noising with two selectable algorithms
- resizing and aspect ratio changing

3. Usage

ffdshow will be used by movie players to decode videos encoded with XviD codec.
It also supports all DIVX versions (DIVX 5 PRO features too)
You can select which codec should be handled by ffdshow.

4. Configuration

To be written.

4.1 Presets

Amount of different setting in ffdshow is quite large and it is not 
confortable to have to change setting to fit best for every movie. 
Therefore preset was introduced. For now presets system is manual. User 
has to save and load presets. But when I'll find out how to get name of 
currently played file I will implement some sort of automatic preset 
selection. At the bottom of configuration dialog is combo box containing 
available preset. 'default' preset is available always and is used 
as template for new presets.
Saving presets:
  type new preset name and press Save button. Current settings will be 
  saved to named preset.
Loading presets:
  select existing preset and press Load button. If preset with that name 
  doesn't exists, nothing will happen.
Removing presets:
  select preset you want to remove and press Remove button. Default preset 
  can't be removed.
Active preset setting are automatically saved when closing configuration 
dialog.
Supported codec settings are not stored in preset.


5. Web links

ffdshow: http://cutka.szm.sk
XviD:    http://www.xvid.org
ffmpeg:  http://ffmpeg.sf.net
mplayer: http://www.mplayerhq.hu
DVD2AVI: http://arbor.ee.ntu.edu.tw/~jackei/dvd2avi/
Xsharpen, unsharp mask, hue and saturation code
         http://sauron.mordor.net/dgraft/index.html

and 
doom9:   http://www.doom9.org 

6. Copying

All used sources (except of cpu utilization detection routine) and ffdsow itself
are distributed under GPL. See copying.txt


Milan Cutka (cutka@szm.sk)
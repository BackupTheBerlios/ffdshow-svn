#ifndef _CFONTOSDPAGE_H_
#define _CFONTOSDPAGE_H_

#include "Cfont.h"

class TOSDfontPage :public TfontPage
{
public:
 TOSDfontPage(TffdshowPage *Iparent,HWND IhwndParent,IffDecoder *Ideci);
};

#endif 

#ifndef _IMGFILTER_H_
#define _IMGFILTER_H_

struct TpresetSettings;
class TimgFilter
{
protected:
 int dxY,strideY,dyY;
 int dxUV,strideUV,dyUV;
public:
 virtual void init(int Idx,int Istride,int Idy)
  {
   done();
   dxY=Idx;dyY=Idy;strideY=Istride;
   dxUV=Idx/2;dyUV=Idy/2;strideUV=Istride/2;
  };
 virtual void done(void) {};
 virtual void process(const unsigned char *srcY,const unsigned char *srcU,const unsigned char *srcV,
                      unsigned char *dstY,unsigned char *dstU,unsigned char *dstV,
                      const TpresetSettings *cfg)=0;
};

#endif

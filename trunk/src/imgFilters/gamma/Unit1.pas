unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, ComCtrls,math, StdCtrls;

type
  TForm1 = class(TForm)
    imgExp: TImage;
    tbr: TTrackBar;
    lblGamma: TLabel;
    lblA: TLabel;
    lblB: TLabel;
    lblC: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure tbrChange(Sender: TObject);
  private
    exp:array[0..255] of extended;
    fit:array[0..255] of integer;
    procedure calc(g0: Real);
    procedure draw;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

procedure TForm1.calc(g0:Real);
var i:Integer;
    s0,s1,t0,s2,s3,s4,s5,s6,t1,t2,t3:extended;
    xx,yy:Extended;
    a11,a12,a13,a21,a22,a23:extended;
    c1,c2,c3:extended;
    g:extended;
    c1i,c2i,c3i:integer;
    xxi,k:integer;
begin
 lblGamma.Caption:=FloatToStr(g0);
// g:=1/g;
 if g0<1 then g:=1/g0 else g:=g0;
 for i:=0 to 255 do begin
   exp[i]:=Power(i/255.0,g);
  end;

 s0:=0;s1:=0;t0:=0;s2:=0;s3:=0;s4:=0;s5:=0;s6:=0;t1:=0;t2:=0;t3:=0;
 for i:=0 to 255 do begin
   s0:=s0+i; //i
   xx:=i/255.0;
   s1:=s1+xx; //x
   yy:=exp[i];
   t0:=t0+yy; //y
   s2:=s2+xx*xx;
   s3:=s3+xx*xx*xx;
   s4:=s4+xx*xx*xx*xx;
   s5:=s5+xx*xx*xx*xx*xx;
   s6:=s6+xx*xx*xx*xx*xx*xx;
   t1:=t1+yy*xx;
   t2:=t2+yy*xx*xx;
   t3:=t3+yy*xx*xx*xx;
  end;
 a11:=s1-s3;a12:=s2-s3;a13:=s3-t0;
 a21:=s2-s4;a22:=s3-s4;a23:=s4-t1;
 c2:=(a11*a23-a13*a21)/(a12*a21-a11*a22);
 c1:=-(a22*c2+a23)/a21;
 c3:=1-(c1+c2);

 c1i:=trunc(c1*256);
 c2i:=trunc(c2*256);
 c3i:=256-(c1i+c2i);

 c1i:=c1i div 8;
 c2i:=c2i div 8;
 c3i:=c3i div 8;

 if g0>1 then
  for i:=0 to 255 do begin
    exp[i]:=255*Power(i/255.0,1/g);
    xx:=1-i/255.0;
    xxi:=255-i;
//    fit[i]:=255*(1-(c3*xx*xx*xx+c2*xx*xx+c1*xx));
    //fit[i]:=255-((c3i*(xxi div 16)*(xxi/16)*(xxi/16))/4096+(c2i*(xxi/16)*(xxi/16))/256+(c1i*(xxi/16))/16);
   end
 else
  for i:=0 to 255 do begin
//    exp[i]:=255*Power(i/255.0,g);
    fit[i]:=0;
    k:=i;
    inc(fit[i],(c1i*k) div 32);
    k:=(k div 16)*i;
    inc(fit[i],(c2i*k) div 512);
    k:=(k div 64)*i;
    inc(fit[i],(c3i*k) div 2048);
   end;
 lblA.Caption:='c3='+FloatToStr(c3i);
 lblB.Caption:='c2='+FloatToStr(c2i);
 lblC.Caption:='c1='+FloatToStr(c1i);
 draw;
end;

procedure TForm1.draw;
var i:integer;
begin
 imgExp.Canvas.FillRect(imgExp.Canvas.ClipRect);
 for i:=0 to 255 do begin
   imgExp.Canvas.Pixels[i,255-Trunc(exp[i])]:=clBlack;
   imgExp.Canvas.Pixels[i,255-trunc(fit[i])]:=clBlue;
  end;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
 calc(1);
end;

procedure TForm1.tbrChange(Sender: TObject);
begin
 calc(tbr.Position/10.0);
end;

end.

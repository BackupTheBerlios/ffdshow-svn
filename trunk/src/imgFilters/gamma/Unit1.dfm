object Form1: TForm1
  Left = 282
  Top = 462
  Width = 531
  Height = 290
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object imgExp: TImage
    Left = 78
    Top = 4
    Width = 256
    Height = 256
  end
  object lblGamma: TLabel
    Left = 10
    Top = 4
    Width = 46
    Height = 13
    Caption = 'lblGamma'
  end
  object lblA: TLabel
    Left = 358
    Top = 10
    Width = 17
    Height = 13
    Caption = 'lblA'
  end
  object lblB: TLabel
    Left = 358
    Top = 29
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object lblC: TLabel
    Left = 358
    Top = 47
    Width = 32
    Height = 13
    Caption = 'Label1'
  end
  object tbr: TTrackBar
    Left = 8
    Top = 20
    Width = 45
    Height = 239
    Max = 33
    Min = 3
    Orientation = trVertical
    Frequency = 1
    Position = 10
    SelEnd = 0
    SelStart = 0
    TabOrder = 0
    TickMarks = tmBottomRight
    TickStyle = tsAuto
    OnChange = tbrChange
  end
end

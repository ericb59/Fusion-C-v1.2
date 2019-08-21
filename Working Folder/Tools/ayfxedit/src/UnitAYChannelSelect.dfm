object FormAyChn: TFormAyChn
  Left = 213
  Top = 193
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Select AY channel for import'
  ClientHeight = 78
  ClientWidth = 216
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 16
  object RadioButton1: TRadioButton
    Left = 8
    Top = 8
    Width = 49
    Height = 17
    Caption = 'Auto'
    Checked = True
    TabOrder = 2
    TabStop = True
    OnClick = RadioButton1Click
  end
  object RadioButton2: TRadioButton
    Left = 8
    Top = 24
    Width = 65
    Height = 17
    Caption = 'Chn. A'
    TabOrder = 3
    OnClick = RadioButton2Click
  end
  object RadioButton3: TRadioButton
    Left = 8
    Top = 40
    Width = 65
    Height = 17
    Caption = 'Chn. B'
    TabOrder = 4
    OnClick = RadioButton3Click
  end
  object RadioButton4: TRadioButton
    Left = 8
    Top = 56
    Width = 65
    Height = 17
    Caption = 'Chn. C'
    TabOrder = 5
    OnClick = RadioButton4Click
  end
  object Button1: TButton
    Left = 128
    Top = 12
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 128
    Top = 44
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = Button2Click
  end
end

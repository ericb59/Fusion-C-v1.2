object FormSnChn: TFormSnChn
  Left = 213
  Top = 304
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Select SN channel for import'
  ClientHeight = 96
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
    Width = 73
    Height = 17
    Caption = 'Tone #0'
    TabOrder = 3
    OnClick = RadioButton1Click
  end
  object RadioButton3: TRadioButton
    Left = 8
    Top = 40
    Width = 73
    Height = 17
    Caption = 'Tone #1'
    TabOrder = 4
    OnClick = RadioButton1Click
  end
  object RadioButton4: TRadioButton
    Left = 8
    Top = 56
    Width = 73
    Height = 17
    Caption = 'Tone #2'
    TabOrder = 5
    OnClick = RadioButton1Click
  end
  object RadioButton5: TRadioButton
    Left = 8
    Top = 72
    Width = 65
    Height = 17
    Caption = 'Noise'
    TabOrder = 6
    OnClick = RadioButton1Click
  end
  object CheckBox1: TCheckBox
    Left = 128
    Top = 72
    Width = 81
    Height = 17
    Caption = 'Mix noise'
    Checked = True
    State = cbChecked
    TabOrder = 7
    OnClick = RadioButton1Click
  end
  object Button1: TButton
    Left = 128
    Top = 6
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 128
    Top = 38
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = Button2Click
  end
end

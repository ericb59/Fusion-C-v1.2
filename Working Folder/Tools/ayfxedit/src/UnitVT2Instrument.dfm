object FormVortexExp: TFormVortexExp
  Left = 316
  Top = 491
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'VTII instrument export'
  ClientHeight = 60
  ClientWidth = 256
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonOK: TButton
    Left = 191
    Top = 5
    Width = 61
    Height = 20
    Caption = 'OK'
    TabOrder = 0
    OnClick = ButtonOKClick
  end
  object ButtonCancel: TButton
    Left = 191
    Top = 36
    Width = 61
    Height = 20
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = ButtonCancelClick
  end
  object GroupBoxBaseNote: TGroupBox
    Left = 0
    Top = 0
    Width = 185
    Height = 57
    Caption = 'Base note'
    TabOrder = 2
    object EditNote: TEdit
      Left = 70
      Top = 22
      Width = 41
      Height = 21
      ReadOnly = True
      TabOrder = 0
      Text = 'EditNote'
    end
    object ButtonDownSemi: TButton
      Left = 43
      Top = 22
      Width = 21
      Height = 20
      Caption = '-1'
      TabOrder = 1
      OnClick = ButtonDownSemiClick
    end
    object ButtonDownOctave: TButton
      Left = 16
      Top = 22
      Width = 25
      Height = 20
      Caption = '-12'
      TabOrder = 2
      OnClick = ButtonDownOctaveClick
    end
    object ButtonUpSemi: TButton
      Left = 115
      Top = 22
      Width = 21
      Height = 20
      Caption = '+1'
      TabOrder = 3
      OnClick = ButtonUpSemiClick
    end
    object ButtonUpOctave: TButton
      Left = 139
      Top = 22
      Width = 25
      Height = 20
      Caption = '+12'
      TabOrder = 4
      OnClick = ButtonUpOctaveClick
    end
  end
end

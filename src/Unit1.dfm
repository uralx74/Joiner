object Form1: TForm1
  Left = 195
  Top = 123
  Width = 614
  Height = 540
  Caption = 'Joiner'
  Color = clBtnFace
  Constraints.MinHeight = 540
  Constraints.MinWidth = 614
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnActivate = FormActivate
  OnCreate = FormCreate
  DesignSize = (
    598
    502)
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonStart: TButton
    Left = 168
    Top = 464
    Width = 153
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = #1042#1099#1087#1086#1083#1085#1080#1090#1100
    TabOrder = 0
    OnClick = ButtonStartClick
  end
  object OpenConfigButton: TButton
    Left = 8
    Top = 464
    Width = 153
    Height = 25
    Hint = #1054#1090#1082#1088#1099#1090#1100' '#1092#1072#1081#1083' '#1082#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1080'...'
    Anchors = [akLeft, akBottom]
    Caption = #1054#1090#1082#1088#1099#1090#1100' '#1082#1086#1085#1092#1080#1075#1091#1088#1072#1094#1080#1102'...'
    TabOrder = 1
    OnClick = OpenConfigButtonClick
  end
  object LogRichEdit: TRichEdit
    Left = 0
    Top = 0
    Width = 598
    Height = 457
    Align = alTop
    Anchors = [akLeft, akTop, akRight, akBottom]
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    HideScrollBars = False
    Lines.Strings = (
      'RichEdit1')
    ParentFont = False
    PlainText = True
    PopupMenu = PopupMenu1
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 2
    OnContextPopup = LogRichEditContextPopup
  end
  object ExitButton: TButton
    Left = 440
    Top = 464
    Width = 153
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = #1042#1099#1093#1086#1076
    TabOrder = 3
    OnClick = ExitButtonClick
  end
  object OpenDialog1: TOpenDialog
    DefaultExt = 'xml'
    Options = [ofPathMustExist, ofEnableSizing]
    Left = 160
    Top = 440
  end
  object PopupMenu1: TPopupMenu
    Left = 128
    Top = 440
    object N1: TMenuItem
      Caption = #1050#1086#1087#1080#1088#1086#1074#1072#1090#1100
      ShortCut = 16451
      OnClick = N1Click
    end
  end
  object Timer1: TTimer
    OnTimer = Timer1Timer
    Left = 352
    Top = 464
  end
end

/*!
  \file   Keys.cpp
  \brief  Keys 

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License abide by Mona Project<BR>
  \b Create 2004/04/15
  \author  shadow

  $Revision$
  $Date$
*/

#include <monapi/Keys.h>

using namespace MonAPI;

const int Keys::mapUS102[128] = {
    0        , Escape  , D1             , D2              , D3       , D4         , D5       , D6          ,
    D7       , D8      , D9             , D0              , OemMinus , Oemplus    , Back     , Tab         ,
    Q        , W       , E              , R               , T        , Y          , U        , I           ,
    O        , P       , OemOpenBrackets, OemCloseBrackets, Enter    , LControlKey, A        , S           ,
    D        , F       , G              , H               , J        , K          , L        , OemSemicolon,
    OemQuotes, Oemtilde, LShiftKey      , OemPipe         , Z        , X          , C        , V           ,
    B        , N       , M              , Oemcomma        , OemPeriod, OemQuestion, RShiftKey, Multiply    ,
    LMenu    , Space   , CapsLock       , F1              , F2       , F3         , F4       , F5          ,
    F6       , F7      , F8             , F9              , F10      , NumLock    , Scroll   , NumPad7     ,
    NumPad8  , NumPad9 , Subtract       , NumPad4         , NumPad5  , NumPad6    , Add      , NumPad1     ,
    NumPad2  , NumPad3 , NumPad0        , Decimal         , 0        , 0          , 0        , F11         ,
    F12      , 0       , 0              , 0               , 0        , 0          , 0        , 0
};

const int Keys::mapUS102E0[128] = {
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , Separator, RControlKey, 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , Divide     , 0, PrintScreen,
    RMenu, 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, Home       ,
    Up   , PageUp  , 0     , Left  , 0        , Right      , 0, End        ,
    Down , PageDown, Insert, Delete, 0        , 0          , 0, 0          ,
    0    , 0       , 0     , LWin  , RWin     , Apps       , 0, 0
};

const int Keys::mapJP109[128] = {
    0           , Escape    , D1       , D2              , D3       , D4         , D5       , D6      ,
    D7          , D8        , D9       , D0              , OemMinus , OemQuotes  , Back     , Tab     ,
    Q           , W         , E        , R               , T        , Y          , U        , I       ,
    O           , P         , Oemtilde , OemOpenBrackets , Enter    , LControlKey, A        , S       ,
    D           , F         , G        , H               , J        , K          , L        , Oemplus ,
    OemSemicolon, KanjiMode , LShiftKey, OemCloseBrackets, Z        , X          , C        , V       ,
    B           , N         , M        , Oemcomma        , OemPeriod, OemQuestion, RShiftKey, Multiply,
    LMenu       , Space     , CapsLock , F1              , F2       , F3         , F4       , F5      ,
    F6          , F7        , F8       , F9              , F10      , NumLock    , Scroll   , NumPad7 ,
    NumPad8     , NumPad9   , Subtract , NumPad4         , NumPad5  , NumPad6    , Add      , NumPad1 ,
    NumPad2     , NumPad3   , NumPad0  , Decimal         , 0        , 0          , 0        , F11     ,
    F12         , 0         , 0        , 0               , 0        , 0          , 0        , 0       ,
    0           , 0         , 0        , 0               , 0        , 0          , 0        , 0       ,
    0           , 0         , 0        , 0               , 0        , 0          , 0        , 0       ,
    KanaMode    , 0         , 0        , OemBackslash    , 0        , 0          , 0        , 0       ,
    0           , IMEConvert, 0        , IMENonconvert   , 0        , OemPipe    , 0        , 0
};

const int Keys::mapJP109E0[128] = {
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , Separator, RControlKey, 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , Divide     , 0, PrintScreen,
    RMenu, 0       , 0     , 0     , 0        , 0          , 0, 0          ,
    0    , 0       , 0     , 0     , 0        , 0          , 0, Home       ,
    Up   , PageUp  , 0     , Left  , 0        , Right      , 0, End        ,
    Down , PageDown, Insert, Delete, 0        , 0          , 0, 0          ,
    0    , 0       , 0     , LWin  , RWin     , Apps       , 0, 0
};

char Keys::ToChar(KeyInfo keyInfo){

    char result;

    switch(keyInfo.keycode) {

    case(Keys::A):
        result = 'A';
        break;
    case(Keys::B):
        result = 'B';
        break;
    case(Keys::C):
        result = 'C';
        break;
    case(Keys::D):
        result = 'D';
        break;
    case(Keys::E):
        result = 'E';
        break;
    case(Keys::F):
        result = 'F';
        break;
    case(Keys::G):
        result = 'G';
        break;
    case(Keys::H):
        result = 'H';
        break;
    case(Keys::I):
        result = 'I';
        break;
    case(Keys::J):
        result = 'J';
        break;
    case(Keys::K):
        result = 'K';
        break;
    case(Keys::L):
        result = 'L';
        break;
    case(Keys::M):
        result = 'M';
        break;
    case(Keys::N):
        result = 'N';
        break;
    case(Keys::O):
        result = 'O';
        break;
    case(Keys::P):
        result = 'P';
        break;
    case(Keys::Q):
        result = 'Q';
        break;
    case(Keys::R):
        result = 'R';
        break;
    case(Keys::S):
        result = 'S';
        break;
    case(Keys::T):
        result = 'T';
        break;
    case(Keys::U):
        result = 'U';
        break;
    case(Keys::V):
        result = 'V';
        break;
    case(Keys::W):
        result = 'W';
        break;
    case(Keys::X):
        result = 'X';
        break;
    case(Keys::Y):
        result = 'Y';
        break;
    case(Keys::Z):
        result = 'Z';
        break;
    case(Keys::D0):
        result = '0';
        break;
    case(Keys::D1):
        result = '1';
        break;
    case(Keys::D2):
        result = '2';
        break;
    case(Keys::D3):
        result = '3';
        break;
    case(Keys::D4):
        result = '4';
        break;
    case(Keys::D5):
        result = '5';
        break;
    case(Keys::D6):
        result = '6';
        break;
    case(Keys::D7):
        result = '7';
        break;
    case(Keys::D8):
        result = '8';
        break;
    case(Keys::D9):
        result = '9';
        break;
    case(Keys::NumPad0):
        result = '0';
        break;
    case(Keys::NumPad1):
        result = '1';
        break;
    case(Keys::NumPad2):
        result = '2';
        break;
    case(Keys::NumPad3):
        result = '3';
        break;
    case(Keys::NumPad4):
        result = '4';
        break;
    case(Keys::NumPad5):
        result = '5';
        break;
    case(Keys::NumPad6):
        result = '6';
        break;
    case(Keys::NumPad7):
        result = '7';
        break;
    case(Keys::NumPad8):
        result = '8';
        break;
    case(Keys::NumPad9):
        result = '9';
        break;
    case(Keys::Decimal):
        result = '.';
        break;
    case(Keys::Add):
        result = '+';
        break;
    case(Keys::Subtract):
        result = '-';
        break;
    case(Keys::Multiply):
        result = '*';
        break;
    case(Keys::Divide):
        result = '/';
        break;
    case(Keys::Space):
        result = ' ';
        break;
    case(Keys::OemPeriod):
        result = '.';
        break;
    case(Keys::Oemcomma):
        result = ',';
        break;
    case(Keys::Oemplus):
        result = ';';
        break;
    case(Keys::OemMinus):
        result = '-';
        break;
    case(Keys::OemSemicolon):
        result = ':';
        break;
    case(Keys::Oemtilde):
        result = '@';
        break;
    case(Keys::OemOpenBrackets):
        result = '[';
        break;
    case(Keys::OemCloseBrackets):
        result = ']';
        break;
    case(Keys::OemBackslash):
        result = '\\';
        break;
    case(Keys::OemPipe):
        result = '\\';
        break;
    case(Keys::OemQuestion):
        result = '/';
        break;
    case(Keys::OemQuotes):
        result = '^';
        break;
    default:
        result = ' ';
        break;
    }
    return result;
}

bool Keys::IsToChar(KeyInfo keyInfo){

  int keycode = keyInfo.keycode;
  
  if( (keycode == Space)
    || ((keycode >= D0) && (keycode <= D9)) 
    || ((keycode >= A) && (keycode <= Z))
    || ((keycode >= NumPad0) && (keycode <= Divide))
    || ((keycode >= OemSemicolon) && (keycode <= Oemtilde))
    || ((keycode >= OemOpenBrackets) && (keycode <= OemQuotes))
    || (keycode == OemBackslash) ) return true;

  return false;
}

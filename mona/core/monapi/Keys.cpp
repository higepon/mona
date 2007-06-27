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

const char Keys::keyTopMapUS102[256] = {
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  0 ,  0 ,  0 ,  0 ,  0 ,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , ';', '+', ',', '-', '.', '/',
    '~',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '[','\\', ']','\'',  0 ,
     0 ,  0 ,'\\',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
};

const char Keys::keyTopMapUS102S[256] = {
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ')', '!', '@', '#', '$', '%', '^', '&', '*', '(',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  0 ,  0 ,  0 ,  0 ,  0 ,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , ':', '=', '<', '_', '>', '?',
    '`',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '{', '|', '}', '"',  0 ,
     0 ,  0 , '_',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
};

const char Keys::keyTopMapJP109[256] = {
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 , 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  0 ,  0 ,  0 ,  0 ,  0 ,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , ':', ';', ',', '-', '.', '/',
    '@',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '[','\\', ']', '^',  0 ,
     0 ,  0 ,'\\',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
};

const char Keys::keyTopMapJP109S[256] = {
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
    ' ',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 , '!', '"', '#', '$', '%', '&','\'', '(', ')',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 , 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  0 ,  0 ,  0 ,  0 ,  0 ,
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '+',  0 , '-', '.', '/',
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '*', '+', '<', '=', '>', '?',
    '`',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
     0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 , '{', '|', '}', '~',  0 ,
     0 ,  0 , '|',  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
};

char Keys::ToChar(KeyInfo keyInfo){

    char result = '\0';

    if(keyInfo.modifiers & KEY_MODIFIER_SHIFT){
        result = keyTopMapJP109S[keyInfo.keycode];
    } else {
        result = keyTopMapJP109[keyInfo.keycode];
    }

    return result;
}

bool Keys::IsToChar(KeyInfo keyInfo){

    char code = '\0';

    if(keyInfo.modifiers & KEY_MODIFIER_SHIFT){
        code = keyTopMapJP109S[keyInfo.keycode];
    } else {
        code = keyTopMapJP109[keyInfo.keycode];
    }

    if(code != '\0') return true;

    return false;
}

char Keys::ToChar(KeyInfo keyInfo, char *map, char *maps){

    char result = '\0';

    if(keyInfo.modifiers & KEY_MODIFIER_SHIFT){
        result = maps[keyInfo.keycode];
    } else {
        result = map[keyInfo.keycode];
    }

    return result;
}

bool Keys::IsToChar(KeyInfo keyInfo, char *map, char *maps){

    char code = '\0';

    if(keyInfo.modifiers & KEY_MODIFIER_SHIFT){
        code = maps[keyInfo.keycode];
    } else {
        code = map[keyInfo.keycode];
    }

    if(code != '\0') return true;

    return false;
}

const int* Keys::GetKeyMap(Keys::KeyboardType kbdt)
{
    switch (kbdt)
    {
        case US102:
            return mapUS102;
        case JP109:
            return mapJP109;
    }
    return NULL;
}

const int* Keys::GetKeyMapE0(Keys::KeyboardType kbdt)
{
    switch (kbdt)
    {
        case US102:
            return mapUS102E0;
        case JP109:
            return mapJP109E0;
    }
    return NULL;
}

const char* Keys::GetKeyTopMap(Keys::KeyboardType kbdt)
{
    switch (kbdt)
    {
        case US102:
            return keyTopMapUS102;
        case JP109:
            return keyTopMapJP109;
    }
    return NULL;
}

const char* Keys::GetKeyTopMapS(Keys::KeyboardType kbdt)
{
    switch (kbdt)
    {
        case US102:
            return keyTopMapUS102S;
        case JP109:
            return keyTopMapJP109S;
    }
    return NULL;
}

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
    default:
        result = ' ';
        break;
    }
    return result;
}




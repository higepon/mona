/*!
  \file   BfMain.h
  \brief  BrainF**k

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/06/08
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _BF_MAIN_
#define _BF_MAIN_

#include <monapi.h>

#ifndef _BF_OPERATOR_
#define _BF_OPERATOR_

#define BF_GREATER 'M' /* > */
#define BF_LESS    'O' /* < */
#define BF_PLUS    'N' /* + */
#define BF_MINUS   'A' /* - */
#define BF_COMMA   'm' /* , */
#define BF_PERIOD  'o' /* . */
#define BF_LEFT    'n' /* [ */
#define BF_RIGHT   'a' /* ] */

#endif

class BfMain{

  private:
    char mainbuf[1024];
    char *mainptr;
    char *strptr;

    int BfGreater(void);
    int BfLess(void);
    int BfPlus(void);
    int BfMinus(void);
    int BfComma(void);
    int BfPeriod(void);
    int BfLeft(void);
    int BfRight(void);

  public:
    BfMain(void);
    ~BfMain(void);
    void Init();
    int Parse(char *str);

};

#endif

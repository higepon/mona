/*!
    \file  msgpanel.h
    \brief 簡易メッセージ 結果表示パネルクラス ヘッダ

    Copyright (c) 2005 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/10 update:$Date$
*/


#include <baygui.h>


#define MAX_MSG_LINE 15

/** 一般部品であるControlクラスを継承 */
class MsgPanel : public Control {

private:
    /** IPアドレス(またはホスト名) */
    char ipaddr[128];
    bool replayflag;
    char pacbuf[128];
    int  paclen;
    HList<char*> *msglist;

public:
    MsgPanel();
    virtual void setIpAddr(char *ipaddr);
    virtual void setPacbuf(char *pacbuf);
    virtual void setPaclen(int paclen);
    virtual void setReplayflag(bool rpf);
    virtual void onPaint(Graphics *g);
};

/*!
    \file  pingpanel.h
    \brief Pingもどき 結果表示パネルクラス ヘッダ

    Copyright (c) 2005 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2005/05/09 update:$Date$
*/


#include <baygui.h>

/** 一般部品であるControlクラスを継承 */
class PingPanel : public Control {

private:
    /** IPアドレス(またはホスト名) */
    char ipaddr[128];
    bool replayflag;
    char pacbuf[128];
    int  paclen;

public:
    PingPanel();
    virtual void setIpAddr(char *ipaddr);
    virtual void setPacbuf(char *pacbuf);
    virtual void setPaclen(int paclen);
    virtual void setReplayflag(bool rpf);
    virtual void paint(Graphics *g);
};

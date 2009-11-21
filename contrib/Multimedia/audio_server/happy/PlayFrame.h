/*!
    \file   bplayer.cpp
    \brief  beep music player

    Copyright (c) 2007 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/26 update:$Date$
*/

#ifndef _PLAY_FRAME_
#define _PLAY_FRAME_

#include <baygui.h>

namespace baygui
{
class PlayFrame : public Window
{
protected:
    Image* icon;

protected:
    virtual bool getCloseButtonClicked(int px, int py)
    {
        int w = getWidth();
        return (w - 17 <= px && px <= w - 12 && 6 <= py && py <= 11);
    }

    virtual bool getTitlebarClicked(int px, int py)
    {
        int w = getWidth();
        return (0 <= px && px < w - 17 && 0 <= py && py < getInsets()->top)
            || (w - 12 < px && px < w && 0 <= py && py < getInsets()->top);
    }

public:
    PlayFrame(const char* iconPath);
    virtual ~PlayFrame();
    virtual void repaint();
};
}

#endif // _PLAY_FRAME_

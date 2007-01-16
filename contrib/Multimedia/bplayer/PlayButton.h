#ifndef _PLAY_BUTTON_
#define _PLAY_BUTTON_

#include <baygui.h>

class PlayButton : public Component
{
protected:
    bool isPushed_;
    Image* playImage_;
    Image* stopImage_;

public:
    PlayButton(const std::string& playImagePath, const std::string& stopImagePath, int width, int height) :
        isPushed_(false),
        playImage_(NULL),
        stopImage_(NULL)
    {
        playImage_ = new Image(playImagePath.c_str());
        stopImage_ = new Image(stopImagePath.c_str());
        if (NULL == playImage_ || NULL == stopImage_)
        {
            char buf[64];
            sprintf("%s: image load error\n", __func__);
            syscall_print(buf);
            exit(-1);
        }
        setBounds(0, 0, width, height);
    }

    virtual ~PlayButton()
    {
        delete playImage_;
        delete stopImage_;
    }

    void paint(Graphics* g)
    {
        int w = getWidth();
        int h = getHeight();
        g->setColor(32, 33, 32);
        g->fillRect(0, 0, w, h);
        g->drawImage(isPushed_ ? stopImage_ : playImage_, 0, 0);
    }

    void stop()
    {
        isPushed_ = false;
        repaint();
    }

    void processEvent(Event* event)
    {
        if (event->getType() == MouseEvent::MOUSE_PRESSED)
        {
            isPushed_ = !isPushed_;
            repaint();
        }
        getParent()->processEvent(event);
    }
};

#endif // _PLAY_BUTTON_

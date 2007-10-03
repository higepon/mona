#ifndef _IMAGE_SINK_BUTTON_
#define _IMAGE_SINK_BUTTON_

#include <baygui.h>

class ImageSinkButton : public Component
{
protected:
    Image* normal_;
    Image* pushed_;
    Image* focused_;
    Image* image_;
    bool   isPushed_;

public:
    ImageSinkButton(Image* normal, Image* pushed, Image* focused) :
        normal_(normal),
        pushed_(pushed),
        focused_(focused),
        image_(normal),
        isPushed_(false)
    {
    }

    virtual ~ImageSinkButton()
    {
    }

    void paint(Graphics* g)
    {
        g->drawImage(image_, 0, 0);
    }

    // MOUSE_ENTERED/MOUSE_EXITは baygui に実装されていない
    void processEvent(Event* event)
    {
        switch(event->getType())
        {
        case MouseEvent::MOUSE_ENTERED:
        {
            image_ = focused_;
            repaint();
            break;
        }
        case MouseEvent::MOUSE_EXIT:
        {
            image_ = isPushed_ ? pushed_ : normal_;
            repaint();
            break;
        }
        case MouseEvent::MOUSE_PRESSED:
            if (isPushed_)
            {
                image_ = normal_;
                isPushed_ = false;
            }
            else
            {
                image_ = pushed_;
                isPushed_ = true;
            }
            repaint();
            break;
        }
        getParent()->processEvent(event);
    }

    void revert()
    {
        if (isPushed_)
        {
            image_ = normal_;
            isPushed_ = false;
        }
        else
        {
            image_ = pushed_;
            isPushed_ = true;
        }
    }
};

#endif // _PLAY_BUTTON_

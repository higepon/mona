#ifndef _IMAGE_BUTTON_
#define _IMAGE_BUTTON_

#include <baygui.h>

class ImageButton : public Component
{
protected:
    Image* image_;
    Image* normal_;
    Image* onpush_;

public:
    ImageButton(Image* normal, Image* onpush) :
        image_(normal),
        normal_(normal),
        onpush_(onpush)
    {
    }

    virtual ~ImageButton()
    {
    }

    void paint(Graphics* g)
    {
        g->drawImage(image_, 0, 0);
    }

    void processEvent(Event* event)
    {
        switch(event->getType())
        {
        case MouseEvent::MOUSE_PRESSED:
        {
            image_ = onpush_;
            repaint();
            break;
        }
        case MouseEvent::MOUSE_RELEASED:
        {
            image_ = normal_;
            repaint();
            break;
        }
        }
        getParent()->processEvent(event);
    }
};

#endif // _IMAGE_BUTTON_

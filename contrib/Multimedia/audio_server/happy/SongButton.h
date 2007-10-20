#ifndef _SONG_BUTTON_
#define _SONG_BUTTON_

#include <baygui.h>

class SongButton : public Component
{
protected:
    bool isPushed_;
    String label_;
public:
    SongButton(const String& label) : isPushed_(false), label_(label)
    {
        setBounds(0, 0, 40, 20);
    }

    virtual ~SongButton()
    {
    }

    void paint(Graphics* g)
    {
        int w = getWidth();
        int h = getHeight();

        g->setColor(getBackground());
        g->fillRect(0, 0, w, h);

        int fh = getFontMetrics()->getHeight(getLabel());
        int y = (h - fh) / 2;
        g->setColor(getForeground());
        g->drawString(getLabel(), 10, y);
    }

    void processEvent(Event* event)
    {
        if (event->getType() == MouseEvent::MOUSE_PRESSED)
        {
            isPushed_ = true;
            repaint();
            getParent()->processEvent(event);
        }
        else if (event->getType() == MouseEvent::MOUSE_RELEASED)
        {
            isPushed_ = false;
            repaint();
            getParent()->processEvent(event);
        }

    }

    const char* getLabel()
    {
        return label_.getBytes();
    }
};

#endif // _SONG_BUTTON_

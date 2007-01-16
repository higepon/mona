#ifndef _PROGRESS_ICON_
#define _PROGRESS_ICON_

#include <baygui.h>
#include <monapi/StringHelper.h>

class ProgressIcon : public Component
{
protected:
    typedef std::vector<Image*> Images;
    Images images_;
    uint32_t index_;

public:
    ProgressIcon(MonAPI::Strings s, int width, int height) : index_(0)
    {
        for (MonAPI::Strings::iterator it = s.begin(); it != s.end(); it++)
        {
            Image* image = new Image((*it).c_str());
            images_.push_back(image);

        }
        setBounds(0, 0, width, height);
    }

    virtual ~ProgressIcon()
    {
        for (Images::iterator it = images_.begin(); it != images_.end(); it++)
        {
            delete (*it);
        }
    }

    void progress()
    {
        index_++;
        if (index_ >= images_.size()) index_ = 0;
        repaint();
    }

    void reset()
    {
        index_ = 0;
        repaint();
    }

    void paint(Graphics* g)
    {
        g->drawImage(images_[index_], 0, 0);
    }

    void processEvent(Event* event)
    {
    }
};

#endif // _PROGRESS_ICON_

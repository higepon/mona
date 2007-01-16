/*!
    \file   bplayer.cpp
    \brief  beep music player

    Copyright (c) 2006 Higepon
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2006/11/21 update:$Date$
*/

#include <monapi/audio/Beep.h>
#include <monapi/StringHelper.h>
#include "PlayButton.h"
#include "ProgressIcon.h"
#include "PlayFrame.h"
#include "ImageButton.h"
#include "ImageSinkButton.h"

using namespace std;
using namespace MonAPI;

#define APPLICATION_DATA_DIR "/APPS/BAYGUI/BPLAYER.APP"

static void wrapperPlay(void* p);
static void wrapperStopped(void* p);
static void wrapperProgress(void* p);

class BPlayer : public PlayFrame
{
private:
    typedef vector<string> PlayList;

    bool        nowPlaying_;
    uint32_t       playIndex_;
    PlayList    playList_;
    audio::Beep beeper_;

    // Images
    Image* playImage_;
    Image* playImageFocused_;
    Image* stopImage_;
    Image* backImage_;
    Image* backImageFocused_;
    Image* forwardImage_;
    Image* forwardImageFocused_;
    Label* statusLabel_;

    // Components
    ImageButton*     backButton_;
    ImageButton*     forwardButton_;
    ProgressIcon*    progressIcon_;
    ImageSinkButton* playButton_;

    // Threads
    MonAPI::Thread* playThread_;
    MonAPI::Thread* progressThread_;

    // Const
    const int   PADDING;
    const int   START_X;
    const int   START_Y;
    const int   ICON_WIDTH;
    const int   ICON_HEIGHT;
    const int   PROGRESS_SIZE;
    const uint32_t BG_COLOR;
    const uint32_t FG_COLOR;

public:
    BPlayer() : PlayFrame(APPLICATION_DATA_DIR"/USER.JPG"),
        nowPlaying_(false),
        playIndex_(0),
        playThread_(NULL),
        progressThread_(NULL),
        PADDING(4),
        START_X(PADDING * 4),
        START_Y(PADDING * 2),
        ICON_WIDTH(25),
        ICON_HEIGHT(14),
        PROGRESS_SIZE(12),
        BG_COLOR(0xff212021),
        FG_COLOR(0xff393c39)
    {
        loadImages();
        initComponents();
        if (!readPlayList())
        {
            statusLabel_->setText("音楽ファイルがみつかりません。");
            return;
        }
    }

    ~BPlayer()
    {
        destroyComponents();
        destroyImages();
        if (playThread_ != NULL) delete playThread_;
        if (progressThread_ != NULL) delete progressThread_;
    }

    void paint(Graphics *g)
    {
        int w = getWidth();
        int h = getHeight();
        g->setColor(BG_COLOR);
        g->fillRect(0, 0, w, h);
        g->setColor(FG_COLOR);
        g->drawRect(PADDING, PADDING, w - PADDING * 5 , h - PADDING * 7);
    }

    bool readPlayList()
    {
        monapi_cmemoryinfo* mi = monapi_file_read_directory(APPLICATION_DATA_DIR);
        if (NULL == mi) return false;
        int size = *(int*)mi->Data;
        if (size == 0) return false;

        monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
        for (int i = 0; i < size; i++, p++)
        {
            string file = p->name;

            if (StringHelper::endsWith(file, ".MML"))
            {
                playList_.push_back(file);
            }
        }
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
        return playList_.size() != 0;
    }

    void play()
    {
        syscall_get_io();
        for (uint32_t i = playIndex_; i < playList_.size(); i++)
        {
            string path = APPLICATION_DATA_DIR"/" + playList_[i];
            monapi_cmemoryinfo* mi = monapi_file_read_all(path.c_str());
            if (NULL == mi || mi->Size == 0) return;
            string mml = string((char*)mi->Data, mi->Size);
            statusLabel_->setText(beeper_.title(mml.c_str()).c_str());
            beeper_.play(mml.c_str());
            beeper_.rest(1000);
        }
        playIndex_ = 0;
        playButton_->revert();
    }

    void progress()
    {
        for (;;)
        {
            progressIcon_->progress();
            sleep(100);
        }
    }

    bool forward()
    {
        playIndex_++;
        if (playIndex_ >= playList_.size())
        {
            playIndex_ = playList_.size() - 1;
            return false;
        }
        else
        {
            return true;
        }
    }

    bool back()
    {
        playIndex_--;
        if (playIndex_ < 0)
        {
            playIndex_ = 0;
            return false;
        }
        else
        {
            return true;
        }
    }

    void stopPlay()
    {
        if (!nowPlaying_ || NULL == playThread_) return;
        progressThread_->stop();
        playThread_->stop();
        beeper_.rest(1);
        delete progressThread_;
        delete playThread_;
        playThread_ = NULL;
        progressThread_ = NULL;
        nowPlaying_ = false;
        progressIcon_->reset();
    }

    void stopped()
    {
        beeper_.rest(1);
        progressThread_->stop();
        delete progressThread_;
        progressThread_ = NULL;
        nowPlaying_ = false;
        progressIcon_->reset();
    }

    void startPlayThread()
    {
        playThread_ = new MonAPI::Thread(wrapperPlay, (void*)this, wrapperStopped);
        playThread_->start();
        nowPlaying_ = true;

        progressThread_ = new MonAPI::Thread(wrapperProgress, (void*)this, NULL);
        progressThread_->start();
    }

    void onMousePress(MouseEvent* event)
    {
        Component* eventSource = event->getSource();
        if (eventSource == playButton_)
        {
            if (nowPlaying_)
            {
                stopPlay();
            }
            else
            {
                startPlayThread();
            }
        }
        else if (eventSource == forwardButton_)
        {
            if (!nowPlaying_) return;
            if (forward())
            {
                stopPlay();
                startPlayThread();
            }
        }
        else if (eventSource == backButton_)
        {
            if (!nowPlaying_) return;
            if (back())
            {
                stopPlay();
                startPlayThread();
            }
        }
    }


    void processEvent(Event* event)
    {
        switch(event->getType())
        {
        case MouseEvent::MOUSE_PRESSED:
            onMousePress((MouseEvent*)event);
            break;
        default:
            break;
        }
    }

    void stop()
    {
        if (playThread_ != NULL) playThread_->stop();
        if (progressThread_ != NULL) progressThread_->stop();
        PlayFrame::stop();
    }

protected:
    void initComponents()
    {
        setBackground(BG_COLOR);
        setBounds(120, 120, ICON_WIDTH * 3 + PROGRESS_SIZE + PADDING * 3 + 40, ICON_HEIGHT + PROGRESS_SIZE + 50);

        statusLabel_ = new Label("ready");
        statusLabel_->setBounds(START_X, START_Y, ICON_WIDTH * 4, ICON_HEIGHT);
        statusLabel_->setBackground(BG_COLOR);
        statusLabel_->setForeground(baygui::Color::gray);
        add(statusLabel_);

        playButton_ = new ImageSinkButton(playImage_, stopImage_, playImageFocused_);
        playButton_->setBounds(ICON_WIDTH + PADDING + START_X, ICON_HEIGHT + START_Y + PADDING, ICON_WIDTH, ICON_HEIGHT);
        add(playButton_);

        backButton_ = new ImageButton(backImage_, backImageFocused_);
        backButton_->setBounds(START_X, ICON_HEIGHT + START_Y + PADDING, ICON_WIDTH, ICON_HEIGHT);
        add(backButton_);

        forwardButton_ = new ImageButton(forwardImage_, forwardImageFocused_);
        forwardButton_->setBounds(ICON_WIDTH * 2 + PADDING * 2 + START_X, ICON_HEIGHT + START_Y + PADDING, ICON_WIDTH, ICON_HEIGHT);
        add(forwardButton_);

        Strings icons;
        icons.push_back(APPLICATION_DATA_DIR"/BAR0.JPG");
        icons.push_back(APPLICATION_DATA_DIR"/BAR1.JPG");
        icons.push_back(APPLICATION_DATA_DIR"/BAR2.JPG");
        icons.push_back(APPLICATION_DATA_DIR"/BAR3.JPG");
        icons.push_back(APPLICATION_DATA_DIR"/BAR4.JPG");
        progressIcon_ = new ProgressIcon(icons, PROGRESS_SIZE, PROGRESS_SIZE);
        progressIcon_->setBounds(ICON_WIDTH * 3 + PADDING * 3 + START_X, ICON_HEIGHT + START_Y + PADDING, PROGRESS_SIZE, PROGRESS_SIZE);
        add(progressIcon_);
    }

    void destroyComponents()
    {
        delete statusLabel_;
        delete playButton_;
        delete backButton_;
        delete forwardButton_;
        delete progressIcon_;
    }

    void loadImages()
    {
        playImage_           = new Image(APPLICATION_DATA_DIR"/PLAY.JPG");
        playImageFocused_    = new Image(APPLICATION_DATA_DIR"/PLAY_F.JPG");
        stopImage_           = new Image(APPLICATION_DATA_DIR"/STOP_F.JPG");
        backImage_           = new Image(APPLICATION_DATA_DIR"/BACK.JPG");
        backImageFocused_    = new Image(APPLICATION_DATA_DIR"/BACK_F.JPG");
        forwardImage_        = new Image(APPLICATION_DATA_DIR"/FWD.JPG");
        forwardImageFocused_ = new Image(APPLICATION_DATA_DIR"/FWD_F.JPG");
    }

    void destroyImages()
    {
        delete playImage_;
        delete playImageFocused_;
        delete stopImage_;
        delete backImage_;
        delete backImageFocused_;
        delete forwardImage_;
        delete forwardImageFocused_;
    }
};

static void wrapperProgress(void* p)
{
    BPlayer* player = (BPlayer*)p;
    player->progress();
}

static void wrapperPlay(void* p)
{
    BPlayer* player = (BPlayer*)p;
    player->play();
}

static void wrapperStopped(void* p)
{
    BPlayer* player = (BPlayer*)p;
    player->stopped();
}

int main(int argc, char* argv[])
{
    syscall_get_io();
    BPlayer* player = new BPlayer();
    player->run();
    delete player;
    return 0;
}

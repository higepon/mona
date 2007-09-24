#ifndef _GUI_PLAYER_
#define _GUI_PLAYER_

#include <baygui.h>
#include <audio.h>
#include <string>
#include <vector>
#include <limits.h>
#include <monapi/messages.h>
#include <servers/audio.h>
#include <monapi/Stream.h>
#include <monapi/StringHelper.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <dirent.h>
#include <ivorbisfile.h>
#include <monapi.h>

typedef std::vector<std::string> strings;

class GUIPlayer : public Frame
{
private:
    Audio* audio;
    Button* forwardButton;
    Button* backwardButton;
    Label* statusLabel;
    int command;
    uint32_t tid;

    enum
    {
        COMMAND_NONE,
        COMMAND_FORWARD,
        COMMAND_BACKWARD,
        COMMAND_VOLUME_DOWN,
        COMMAND_VOLUME_UP,
        COMMAND_EXIT,
        MSG_BUFFER_SIZE = 64,
    };

public:
    GUIPlayer();
    virtual ~GUIPlayer();
    void playLoop();

protected:
    void initComponents();
    void destroyComponents();
    void processEvent(Event* e);
    void listOggfiles(const char* dirPath, strings& oggFiles);
};

#endif // _GUI_PLAYER_

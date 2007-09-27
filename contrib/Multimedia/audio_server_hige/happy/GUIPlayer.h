#ifndef _GUI_PLAYER_
#define _GUI_PLAYER_

#include <baygui.h>
#include <audio.h>
#include <string>
#include <vector>
#include <map>
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
#include "PlayFrame.h"

class GUIPlayer : public PlayFrame
{
private:
    typedef std::vector<std::string> strings;
    typedef std::map<std::string, Label*> LabelsMap;

    Audio* audio;
    Button* forwardButton;
    Button* backwardButton;
    Label* statusLabel;
    Label** songLabels;
    int command;
    uint32_t tid;
    LabelsMap labelsMap;

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

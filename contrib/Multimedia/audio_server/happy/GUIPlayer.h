#ifndef _GUI_PLAYER_
#define _GUI_PLAYER_

#include <baygui.h>
#include <audio.h>
#include <string>
#include <vector>
#include <algorithm>
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
#include "ImageSinkButton.h"
#include "SongButton.h"

class GUIPlayer : public PlayFrame
{
private:
    typedef struct
    {
        std::string path;
        std::string artist;
        std::string title;
        SongButton* button;
    } Song;

    typedef std::vector<std::string> strings;
    typedef std::map<std::string, Button*> buttonsMap;
    typedef std::vector<Song*> Songs;

    Audio* audio;
    Image* forwardImage;
    Image* backwardImage;
    ImageSinkButton* forwardButton;
    ImageSinkButton* backwardButton;
    int command;
    int commandSelectIndex;
    uint32_t tid;
    Songs songs;
    static const std::string MUSIC_DIR;
    enum
    {
        COMMAND_NONE,
        COMMAND_FORWARD,
        COMMAND_BACKWARD,
        COMMAND_VOLUME_DOWN,
        COMMAND_VOLUME_UP,
        COMMAND_SELECT,
        COMMAND_EXIT,
        MSG_BUFFER_SIZE = 64,
        BACKGROUND_COLOR = baygui::Color::white, //0xff000000 | (0xff << 16) | (0xd3 << 8) | 0x21,
        BUTTON_BACKGROUND_COLOR1 = baygui::Color::lightGray,
        BUTTON_BACKGROUND_COLOR2 = baygui::Color::blue,
        BUTTON_FOREGROUND_COLOR1 = baygui::Color::black,
        BUTTON_FOREGROUND_COLOR2 = baygui::Color::white,
    };

public:
    GUIPlayer();
    virtual ~GUIPlayer();
    void playLoop();

protected:
    std::string upperCase(const std::string& s);
    void showError(const char *fmt, ...);
    void paint(Graphics *g);
    void readSongs();
    void initComponents();
    void destroyComponents();
    void processEvent(Event* e);
    void listOggfiles(const std::string& path, strings& oggFiles);
};

#endif // _GUI_PLAYER_

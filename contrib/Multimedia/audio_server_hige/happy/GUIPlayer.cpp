#include "GUIPlayer.h"

using namespace MonAPI;
using namespace std;

// wrapperPlayLoop for thread
static void __fastcall wrapperPlayLoop(void* p)
{
    GUIPlayer* player = (GUIPlayer*)p;
    player->playLoop();
}

// GUIPlayer
GUIPlayer::GUIPlayer() : PlayFrame(NULL), command(COMMAND_NONE)
{
    initComponents();
    tid = mthread_create_with_arg(wrapperPlayLoop, this);
}

GUIPlayer::~GUIPlayer()
{
    destroyComponents();
    if (NULL != audio)
    {
        audio->stop();
        delete audio;
    }
    mthread_kill(tid);
}

void GUIPlayer::initComponents()
{
    setBounds(20, 20, 250, 375);
//    setTitle("happy!!");

    forwardButton  = new Button("forward");
    backwardButton = new Button("back");
    statusLabel    = new Label("");
    backwardButton->setBounds(0, 0, 50, 20);
    forwardButton->setBounds(50, 0, 50, 20);
    statusLabel->setBounds(0, 20, 100, 20);
    add(forwardButton);
    add(backwardButton);
    add(statusLabel);
}

void GUIPlayer::processEvent(Event* e)
{
    if (e->getType() == Event::MOUSE_PRESSED && e->getSource() == forwardButton)
    {
        command = COMMAND_FORWARD;
    }
    else if (e->getType() == Event::MOUSE_PRESSED && e->getSource() == backwardButton)
    {
        command = COMMAND_BACKWARD;
    }
}

void GUIPlayer::destroyComponents()
{
    delete forwardButton;
    delete backwardButton;
    delete statusLabel;
}

void GUIPlayer::playLoop()
{
    static char msg[MSG_BUFFER_SIZE];

    struct audio_data_format defaultFormat = {0, 2, 16, 4400};
    audio = new Audio(&defaultFormat, AUDIO_OUTPUT);
    if (-1 == audio->start())
    {
        statusLabel->setText("Can not connect to AUDIO server!\n");
    }

    strings oggFiles;
    listOggfiles("/MUSIC", oggFiles);
    if (oggFiles.size() == 0)
    {
        statusLabel->setText("No ogg files found\n");
        delete audio;
    }

    int playingIndex = 0;
    for (;;)
    {
        string oggFile = oggFiles[playingIndex];
        FILE* fp = fopen(oggFile.data(), "rb");
        if (NULL == fp)
        {
            snprintf(msg, MSG_BUFFER_SIZE, "File open error: %s.\n", oggFile.data());
            statusLabel->setText(msg);
            continue;
        }
        OggVorbis_File vf;
        if (ov_open(fp, &vf, NULL, 0) < 0)
        {
            snprintf(msg, MSG_BUFFER_SIZE, "Skipped, %s does not appear to be an Ogg bitstream.\n", oggFile.data());
            statusLabel->setText(msg);
            continue;
        }
        char** infoTexts = ov_comment(&vf, -1)->user_comments;
        vorbis_info* vi=ov_info(&vf, -1);
        string title = "";
        while(*infoTexts)
        {
            if (title != "") title += " - ";
            title += *infoTexts;
            ++infoTexts;
        }

        // default title is file name
        if (title == "") title = oggFile;
        statusLabel->setText(title.data());

        struct audio_data_format format = {0, vi->channels, 16, vi->rate};
        audio->setFormat(&format);
        int current_section;
        char pcmout[4096];
        for (;;)
        {
            switch(command)
            {
            case COMMAND_FORWARD:
                command = COMMAND_NONE;
                goto replay;
            case COMMAND_BACKWARD:
                command = COMMAND_NONE;
                playingIndex -= 2;
                if (playingIndex < 0) playingIndex = oggFiles.size() -1;
                goto replay;
            case COMMAND_VOLUME_UP:
            case COMMAND_VOLUME_DOWN:
                break;
            default:
                break;
            }
            long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
            if (ret == 0)
            {
                break;
            }
            else if (ret < 0)
            {
                statusLabel->setText("Warning data broken?\n");
            }
            else
            {
                audio->write(pcmout, ret);
            }
        }
    replay:
        fclose(fp);
        playingIndex++;
        if (playingIndex >= oggFiles.size()) playingIndex = 0;
    }
    audio->stop();
    delete audio;
    audio = NULL;
}

void GUIPlayer::listOggfiles(const char* dirPath, strings& oggFiles)
{
    DIR* dir;
    if ((dir = opendir(dirPath)) == NULL)
    {
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        string name = entry->d_name;
        if (StringHelper::endsWith(name, ".OGG"))
        {
            name = "/" + name;
            name = dirPath + name;
            oggFiles.push_back(name);
        }
    }
    closedir(dir);
}

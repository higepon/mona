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
const string GUIPlayer::MUSIC_DIR = "/MUSIC";

GUIPlayer::GUIPlayer() : PlayFrame(NULL), command(COMMAND_NONE)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    initComponents();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    tid = mthread_create_with_arg(wrapperPlayLoop, this);
}

GUIPlayer::~GUIPlayer()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    destroyComponents();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    if (NULL != audio)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        audio->stop();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        delete audio;
    }
    for (Songs::iterator it = songs.begin(); it != songs.end(); ++it)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        delete (*it)->button;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        delete(*it);
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    mthread_kill(tid);
}

void GUIPlayer::initComponents()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    forwardImage  = new Image((MUSIC_DIR + "/FWD.JPG").data());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    backwardImage = new Image((MUSIC_DIR + "/BACK.JPG").data());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    setBounds(200, 20, 250, 375);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    forwardButton  = new ImageSinkButton(forwardImage, forwardImage, forwardImage);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    backwardButton = new ImageSinkButton(backwardImage, backwardImage, backwardImage);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    backwardButton->setBounds(0, 0, 16, 16);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    forwardButton->setBounds(20, 0, 16, 16);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    add(forwardButton);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    add(backwardButton);

}

void GUIPlayer::destroyComponents()
{
    delete forwardButton;
    delete backwardButton;
    delete forwardImage;
    delete backwardImage;
}

void GUIPlayer::processEvent(Event* e)
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e);fflush(stdout);
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getType());fflush(stdout);
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
    if (e->getType() == Event::MOUSE_PRESSED && e->getSource() == forwardButton)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        command = COMMAND_FORWARD;
    }
    else if (e->getType() == Event::MOUSE_PRESSED && e->getSource() == backwardButton)
    {
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
        command = COMMAND_BACKWARD;
    }
    else if (e->getType() == Event::MOUSE_PRESSED)
    {
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
        for (int i = 0; i < songs.size(); i++)
        {
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
            Song* song = songs[i];
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
            if (e->getSource() == song->button)
            {
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
                command = COMMAND_SELECT;
                commandSelectIndex = i;
                break;
            }
        }
    }
    logprintf("%s %s:%d %x\n", __func__, __FILE__, __LINE__, e->getSource());fflush(stdout);
}

void GUIPlayer::readSongs()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    strings oggFiles;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    listOggfiles(MUSIC_DIR, oggFiles);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    for (int i = 0; i < oggFiles.size(); i++)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        string path = oggFiles[i];
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        FILE* fp = fopen(path.data(), "rb");
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        if (NULL == fp)
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            continue;
        }
        OggVorbis_File vf;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        if (ov_open(fp, &vf, NULL, 0) < 0)
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            fclose(fp);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            continue;
        }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        char** infoTexts = ov_comment(&vf, -1)->user_comments;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        vorbis_info* vi=ov_info(&vf, -1);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        Song* song = new Song;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->path = path;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        while(*infoTexts)
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            if (char* p = strstr(*infoTexts, "title="))
            {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                p += strlen("title=");
                song->title = p;
            }
            else if (char* p = strstr(*infoTexts, "artist="))
            {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
                p += strlen("artist=");
                song->artist = p;
            }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            ++infoTexts;
        }
        if (song->title == "")
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            song->title = StringHelper::basename(path.data());
        }
        if (song->artist == "")
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            song->artist = "Unknown Artist";
        }

    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button = new SongButton((song->title + " - " + song->artist).data());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button->setBounds(0, 20 * i + 20, 235, 20);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        add(song->button);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        songs.push_back(song);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        fclose(fp);
    }
    repaint();
}

void GUIPlayer::playLoop()
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    struct audio_data_format defaultFormat = {0, 2, 16, 4400};
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    audio = new Audio(&defaultFormat, AUDIO_OUTPUT);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    if (-1 == audio->start())
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        showError("Can not connect to AUDIO server!\n");
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    readSongs();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    if (songs.size() == 0)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        showError("No songs found\n");
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        exit(-1);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
    }
    for (int playingIndex = 0;;)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        Song* song = songs[playingIndex];
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button->setBackground(baygui::Color::blue);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button->setForeground(baygui::Color::white);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button->repaint();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        FILE* fp = fopen(song->path.data(), "rb");
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        if (NULL == fp)
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            showError("File open error: %s.\n", song->path.data());
            continue;
        }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        OggVorbis_File vf;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        if (ov_open(fp, &vf, NULL, 0) < 0)
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            showError("Skipped, %s does not appear to be an Ogg bitstream.\n", song->path.data());
            fclose(fp);
            continue;
        }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        vorbis_info* vi=ov_info(&vf, -1);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        struct audio_data_format format = {0, vi->channels, 16, vi->rate};
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        audio->setFormat(&format);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
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
                if (playingIndex < 0) playingIndex = songs.size() -1;
                goto replay;
            case COMMAND_VOLUME_UP:
            case COMMAND_VOLUME_DOWN:
                break;
            case COMMAND_SELECT:
                command = COMMAND_NONE;
                playingIndex = commandSelectIndex - 1;
                goto replay;
                break;
            default:
                break;
            }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
            if (ret == 0)
            {
                break;
            }
            else if (ret < 0)
            {
                showError("Warning data broken?\n");
            }
            else
            {
                audio->write(pcmout, ret);
            }
        }
    replay:
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button->setBackground(baygui::Color::lightGray);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        song->button->setForeground(baygui::Color::black);
        song->button->repaint();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        fclose(fp);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);
        playingIndex++;
        if (playingIndex >= songs.size()) playingIndex = 0;
    }
    audio->stop();
    delete audio;
    audio = NULL;
}

void GUIPlayer::paint(Graphics *g)
{
    int w = getWidth();
    int h = getHeight();
    g->setColor(baygui::Color::white);
    g->fillRect(0, 0, w, h);
}

void GUIPlayer::listOggfiles(const string& dirPath, strings& oggFiles)
{
    DIR* dir;
    if ((dir = opendir(dirPath.data())) == NULL)
    {
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        string name = entry->d_name;
        if (StringHelper::endsWith(upperCase(name), ".OGG"))
        {
            name = "/" + name;
            name = dirPath + name;
            oggFiles.push_back(name);
        }
    }

    // hack
    // we hope startup.ogg to be the first.
    reverse(oggFiles.begin(), oggFiles.end());
    closedir(dir);
}

string GUIPlayer::upperCase(const string& s)
{
    string result = s;
    transform(result.begin(), result.end(), result.begin(), toupper);
    return result;
}

void GUIPlayer::showError(const char *fmt, ...)
{
    char buf[MSG_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, MSG_BUFFER_SIZE, fmt, args);
    va_end(args);
#if 1
    _printf(buf);
#else
    statusLabel->setText(buf);
#endif
}

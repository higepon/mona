#include <limits.h>
#include <monapi/messages.h>
#include <servers/audio.h>
#include <monapi/Stream.h>
#include <monapi/StringHelper.h>
#include <stdio.h>
#include <audio.h>
#include <assert.h>
#include <math.h>
#include <dirent.h>
#include <ivorbisfile.h>
#include <monapi.h>
#include <string>
#include <vector>

using namespace MonAPI;
using namespace std;

typedef vector<string> strings;

enum
{
    COMMAND_NONE,
    COMMAND_FORWARD,
    COMMAND_BACKWARD,
    COMMAND_VOLUME_DOWN,
    COMMAND_VOLUME_UP,
    COMMAND_EXIT,
};

static int command = COMMAND_NONE;

static void listOggfiles(const char* dirPath, strings& oggFiles)
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

static void inputLoop()
{
    monapi_register_to_server(ID_KEYBOARD_SERVER, 1);
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;
        switch (msg.header)
        {
        case MSG_KEY_VIRTUAL_CODE:
            if ((msg.arg2 & KEY_MODIFIER_DOWN) == 0) break;
            switch(msg.arg1)
            {
            case(Keys::Q):
                command = COMMAND_EXIT;
                goto end;
                break;
            case(Keys::Left):
                command = COMMAND_BACKWARD;
                break;
            case(Keys::Right):
                command = COMMAND_FORWARD;
                break;
            case(Keys::Down):
                command = COMMAND_VOLUME_DOWN;
                break;
            case(Keys::Up):
                command = COMMAND_VOLUME_UP;
                break;
            default:
                break;
            }
            break;
        default:
            fprintf(stderr, "unknown message\n");
            break;
        }
    }
end:
    monapi_register_to_server(ID_KEYBOARD_SERVER, 0);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s MUSIC.OGG\n", argv[0]);
        fprintf(stderr, "usage: %s -d OGG_DIR\n", argv[0]);
        return -1;
    }
    uint32_t tid = syscall_mthread_create(inputLoop);
    struct audio_data_format defaultFormat = {0, 2, 16, 4400};
    Audio* audio = new Audio(&defaultFormat, AUDIO_OUTPUT);
    if (-1 == audio->start())
    {
        _printf("Can not connect to AUDIO server!\n");
    }

    strings oggFiles;
    if (strcmp(argv[1], "-d") == 0 && argc > 2)
    {
        listOggfiles(argv[2], oggFiles);
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            oggFiles.push_back(argv[i]);
        }
    }

    for (int i = 0; i < oggFiles.size(); i++)
    {
        FILE* fp = fopen(oggFiles[i].data(), "rb");
        if (NULL == fp)
        {
            fprintf(stderr, "File open error: %s.\n", oggFiles[i].data());
            delete audio;
            return -1;
        }
        OggVorbis_File vf;
        if (ov_open(fp, &vf, NULL, 0) < 0)
        {
            fprintf(stderr, "Skipped, %s does not appear to be an Ogg bitstream.\n", argv[i]);
            continue;
        }
        char** infoTexts = ov_comment(&vf, -1)->user_comments;
        vorbis_info* vi=ov_info(&vf, -1);
        while(*infoTexts)
        {
            fprintf(stderr, "%s\n", *infoTexts);
            ++infoTexts;
        }
//         fprintf(stderr, "\nBitstream is %d channel, %dHz\n", vi->channels, vi->rate);
//         fprintf(stderr, "\nDecoded length: %d samples\n",(long)ov_pcm_total(&vf, -1));
//         fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);

        struct audio_data_format format = {0, vi->channels, 16, vi->rate};
        audio->setFonrmat(&format);
        int current_section;
        char pcmout[4096];
        fprintf(stdout, "key q(quit) <=(prev) =>(next)\n");
        for (;;)
        {
            switch(command)
            {
            case COMMAND_FORWARD:
                command = COMMAND_NONE;
                goto replay;
            case COMMAND_BACKWARD:
                command = COMMAND_NONE;
                i -= 2;
                goto replay;
            case COMMAND_EXIT:
                i = oggFiles.size();
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
                fprintf(stderr, "Warning data broken? %s\n", argv[i]);
            }
            else
            {
                audio->write(pcmout, ret);
            }
        }
    replay:
        fclose(fp);
    }
    audio->stop();
    delete audio;
// cleanup causes crash!
//    ov_clear(&vf);
    return 0;
}

#include <servers/audio.h>
#include <monapi/Stream.h>
#include <stdio.h>
#include <audio.h>
#include <assert.h>
#include <math.h>
#include <ivorbisfile.h>
#include <monapi.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s MUSIC.OGG\n", argv[0]);
        return -1;
    }

    printf("argc=%d\n", argc);
    for (int i = 1; i < argc; i++)
    {
        _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        FILE* fp = fopen(argv[i], "rb");
        if (NULL == fp)
        {
            fprintf(stderr, "File open error: %s.\n", argv[i]);
            return -1;
        }

        OggVorbis_File vf;
        _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if (ov_open(fp, &vf, NULL, 0) < 0)
        {
            fprintf(stderr, "Input does not appear to be an Ogg bitstream.\n");
            return -1;
        }
        _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug

        // Ogg Vorbis Info
        char** infoTexts = ov_comment(&vf, -1)->user_comments;
        vorbis_info* vi=ov_info(&vf, -1);
        while(*infoTexts)
        {
            fprintf(stderr, "%s\n", *infoTexts);
            ++infoTexts;
        }
        fprintf(stderr, "\nBitstream is %d channel, %dHz\n", vi->channels, vi->rate);
        fprintf(stderr, "\nDecoded length: %d samples\n",(long)ov_pcm_total(&vf, -1));
        fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);

        struct audio_data_format format = {0, vi->channels, 16, vi->rate};
        Audio* audio = new Audio(&format, AUDIO_OUTPUT);

        printf("Audio object was created.\n");

        audio->start();

        int current_section;
        char pcmout[4096];
        for (;;)
        {
            long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
            if (ret == 0)
            {
                break;
            }
            else if (ret < 0)
            {
                /* error in the stream.  Not a problem, just reporting it in
                   case we (the app) cares.  In this case, we don't. */
            }
            else
            {
                audio->write(pcmout, ret);
            }
        }
        audio->stop();
        fclose(fp);
        delete audio;
        printf("AUdio object was deleted.\n");
        /* cleanup */
        ov_clear(&vf);
 
    }
    return 0;
}

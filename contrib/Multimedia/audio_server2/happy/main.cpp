#include <servers/audio.h>
#include <monapi/Stream.h>
#include <stdio.h>
#include <audio.h>
#include <assert.h>
#include <math.h>
#include <ivorbisfile.h>
#include <monapi.h>

#if 0
int main()
{
    Audio* audio;
    char buf[4096];
    struct audio_data_format format = {0, 2, 16, 44100};
    audio = new Audio(&format, AUDIO_OUTPUT);
    audio->start();

    for (int i = 0; i < 100; i++)
    {
        logprintf("write i=%d\n", i);
        audio->write(buf, 2048);
    }

    audio->stop();

    delete audio;
    printf("AUdio object was deleted.\n");
    return 0;
}
#else

static uint8_t pcmout[4096];
int main()
{
    Audio* audio;
    char buf[4096];
    int counter = 0;
    struct audio_data_format format = {0, 2, 16, 44100};

    OggVorbis_File vf;
    int eof = 0;
    int current_section;


    FILE* fp = fopen("/APPS/A.OGG", "rb");
    if (NULL == fp) {
        fprintf(stderr, "file open error\n");
        return -1;
    }

    if (ov_open(fp, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        return -1;
    }


    char **ptr=ov_comment(&vf,-1)->user_comments;
    vorbis_info *vi=ov_info(&vf,-1);
    while(*ptr){
        fprintf(stderr, "%s\n",*ptr);
        ++ptr;
    }
    fprintf(stderr, "\nBitstream is %d channel, %dHz\n", vi->channels, vi->rate);
    fprintf(stderr, "\nDecoded length: %d samples\n",
            (long)ov_pcm_total(&vf, -1));
    fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);


    audio = new Audio(&format, AUDIO_OUTPUT);
    printf("Audio object was created.\n");

    audio->start();

   while(!eof){
       long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
        if (ret == 0) {
            /* EOF */
            eof=1;
        } else if (ret < 0) {
            /* error in the stream.  Not a problem, just reporting it in
               case we (the app) cares.  In this case, we don't. */
            //      printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        } else {
            /* we don't bother dealing with sample rate changes, etc, but
               you'll have to*/
            audio->write(pcmout, ret);
        }
   }

    audio->stop();

    delete audio;
    printf("AUdio object was deleted.\n");
    return 0;
}
#endif

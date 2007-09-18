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
    FILE* fp = fopen("/APPS/A.OGG", "rb");
    if (NULL == fp) {
        fprintf(stderr, "file open error\n");
        return -1;
    }

    OggVorbis_File vf;
    if (ov_open(fp, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        return -1;
    }

    // Ogg Vorbis Info
    char** ptr=ov_comment(&vf, -1)->user_comments;
    vorbis_info* vi=ov_info(&vf, -1);
    while(*ptr)
    {
        fprintf(stderr, "%s\n",*ptr);
        ++ptr;
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

    delete audio;
    printf("AUdio object was deleted.\n");
    return 0;
}

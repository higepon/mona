#include <stdio.h>
#include <stdint.h>
#include <ivorbisfile.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/stat.h>
static uint8_t pcmout[4096];

#if 0
int main(int ac,char **av) {
    OggVorbis_File vf;
    int eof=0;
    int current_section;

    FILE* fp = fopen("../A.OGG", "rb");
    if (NULL == fp) {
        fprintf(stderr, "file open error\n");
        return -1;
    }

    if (ov_open(fp, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        return -1;
    }

//     char **ptr=ov_comment(&vf,-1)->user_comments;
//     vorbis_info *vi=ov_info(&vf,-1);
//     while(*ptr){
//         fprintf(stderr, "%s\n",*ptr);
//         ++ptr;
//     }
//     fprintf(stderr, "\nBitstream is %d channel, %ldHz\n", vi->channels, vi->rate);
//     fprintf(stderr, "\nDecoded length: %ld samples\n",
//             (long)ov_pcm_total(&vf, -1));
//     fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);


    //    while(!eof){
    long ret = ov_read(&vf, pcmout, 16, &current_section);
      if (ret == 0) {
        /* EOF */
        eof=1;
      } else if (ret < 0) {
        /* error in the stream.  Not a problem, just reporting it in
           case we (the app) cares.  In this case, we don't. */
          printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
      } else {
        /* we don't bother dealing with sample rate changes, etc, but
           you'll have to*/
        int i;
        char buf[32];
        for (i = 0; i < 16; i++) {
          sprintf(buf, "%x ", pcmout[i]);
          printf(buf);
        }
            printf("\n");
      }
      //    }
    return 0;
}


#else

int main(int ac,char **av) {
  int out = open("/dev/dsp", O_WRONLY);
  int format = AFMT_S16_LE;
  ioctl(out, SNDCTL_DSP_SETFMT, &format);
  format = 1;
  ioctl(out, SNDCTL_DSP_STEREO, &format);
  format = 44100;
  ioctl(out, SNDCTL_DSP_SPEED, &format);
  int i;

    OggVorbis_File vf;
    int eof=0;
    int current_section;

    FILE* fp = fopen("../A.OGG", "rb");
    if (NULL == fp) {
        fprintf(stderr, "file open error\n");
        return -1;
    }

    if (ov_open(fp, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        return -1;
    }


   while(!eof){
     long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
      if (ret == 0) {
        /* EOF */
        eof=1;
      } else if (ret < 0) {
        /* error in the stream.  Not a problem, just reporting it in
           case we (the app) cares.  In this case, we don't. */
      } else {
        /* we don't bother dealing with sample rate changes, etc, but
           you'll have to*/
        write(out, pcmout, ret);
      }
    }
    return 0;
}
#endif

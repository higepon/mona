#include <stdio.h>
#include <stdint.h>
#include <ivorbisfile.h>
static uint8_t pcmout[4096];

quad_t __divdi3(quad_t a, quad_t b);

int main(int ac,char **av) {
    OggVorbis_File vf;
    int eof=0;
    int current_section;

    printf("-1/2 = %d\n", __divdi3(-1, 2));

    FILE* fp = fopen("./A.OGG", "rb");
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
      long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
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
        printf("ret=%d\n", ret);
        int i;
        for (i = 10; i < 20; i++) {
            int j = 0;
            for (j = 0; j < 16; j++) {
                printf("%d ", pcmout[i * 16 + j]);
            }
            printf("\n");
        }
      }
      //    }
    return 0;
}

#include <userlib.h>
#include <jpegls.h>

#include "text.hex"
int main() {

    Screen screen;
    printf("user mode screen (x, y) = (%d, %d) %dbpp\n", screen.getXResolution(), screen.getYResolution(), screen.getBpp());

    /* check bpp */
    if (screen.getBpp() < 24) {
        printf("sorry, this demo needs 24 or 32bpp Video mode\n");
        return -1;
    }

    printf("wait please......\n");

    /* nikq code */
    CJPEGLS *jpeg = new CJPEGLS;

    int w,h;
    jpeg->Open(test_jpeg,sizeof(test_jpeg)-1);
    jpeg->GetInfo(&w,&h);
    byte *picture = new byte[w * h * 3];
    jpeg->Decode(picture);

    int x,y;
    int vesaWidth = screen.getXResolution();
    int vesaBpp = screen.getBpp();
    byte* vesaVram = screen.getVRAM();

    for(y=0;y<h;y++){
        for(x=0;x<w;x++){
            int k =(x+(y*vesaWidth))*vesaBpp/8;
            int k2=(x+(y*w))*3;
            /*                picture[k2]=0xFF;
                              picture[k2+1]=0xFF;
                              picture[k2+2]=0xFF;*/
            vesaVram[k]   = picture[k2];
            vesaVram[k+1] = picture[k2+1];
            vesaVram[k+2] = picture[k2+2];
        }
    }

    return 0;
}

#include <userlib.h>
#include <jpegls.h>

//#include "text.hex"
int MonaMain(List<char*>* pekoe) {

    if (pekoe->size() != 1)
    {
        printf("usage: JPEGDEMO.ELF XXXX.JPG\n");
        return -1;
    }

    Screen screen;
    printf("user mode screen (x, y) = (%d, %d) %dbpp\n", screen.getWidth(), screen.getHeight(), screen.getBpp());

    /* check bpp */
    if (screen.getBpp() < 24) {
        printf("sorry, this demo needs 24 or 32bpp Video mode\n");
        return -1;
    }

    printf("wait please......\n");

    /* nikq code */
    CJPEGLS *jpeg = new CJPEGLS;

    FileInputStream fis(pekoe->get(0));

    if (fis.open() != 0)
    {
        printf("file %s not found", pekoe->get(0));
        return -1;
    }

    int size = fis.getFileSize();

    byte* buf = (byte*)malloc(size);
    if (buf == NULL)
    {
        printf("memory allocate error\n");
        return -1;
    }

    if (fis.read(buf, size) != 0)
    {
        printf("file read error\n");
        return -1;
    }

    fis.close();

    if (jpeg->Open(buf, size) != 0)
    {
        printf("not supported format image\n");
        free(buf);
        return -1;
    }

    int w,h;
    jpeg->GetInfo(&w,&h);
    byte *picture = new byte[w * h * 3];
    jpeg->Decode(picture);

    int x,y;
    int vesaWidth = screen.getWidth();
    int vesaBpp = screen.getBpp();
    byte* vesaVram = screen.getVRAM();

    for(y=0;y<h;y++){
        for(x=0;x<w;x++){
            int k =(x+(y*vesaWidth))*vesaBpp/8;
            int k2=(x+(y*w))*3;
            /*                picture[k2]=0xFF;
                              picture[k2+1]=0xFF;
                              picture[k2+2]=0xFF;*/
            vesaVram[k]   = picture[k2+2];
            vesaVram[k+1] = picture[k2+1];
            vesaVram[k+2] = picture[k2+0];
        }
    }

    free(buf);
    return 0;
}

#include <monapi.h>
#include <jpegls.h>

/* nikq and Higepon */

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    /* check arguments */
    if (pekoe->size() != 1)
    {
        printf("usage: JPEGDEMO.ELF XXXX.JPG\n");
        return -1;
    }

    Screen screen;
    printf("screen information (x, y) = (%d, %d) %dbpp\n", screen.getWidth(), screen.getHeight(), screen.getBpp());

    /* check bpp */
    if (screen.getBpp() < 24)
    {
        printf("sorry, this demo needs 24 or 32bpp Video mode\n");
        return -1;
    }

    /* start */
    printf("wait please......\n");

    CJPEGLS *jpeg = new CJPEGLS;

    /* file open */
    FileInputStream in(pekoe->get(0));
    if (in.open() != 0)
    {
        printf("file %s not found", pekoe->get(0));
        return -1;
    }

    /* allocate file buffer */
    int size = in.getFileSize();
    byte* filebuf = new byte[size];

    if (filebuf == NULL)
    {
        printf("memory allocate error\n");
        return -1;
    }

    /* read file */
    if (in.read(filebuf, size) != 0)
    {
        printf("file read error\n");
        delete filebuf;
        return -1;
    }
    in.close();

    /* jpeg operation */
    if (jpeg->Open(filebuf, size) != 0)
    {
        printf("not supported image\n");
        delete filebuf;
        return -1;
    }

    int w, h;
    jpeg->GetInfo(&w, &h);
    byte *picture = new byte[w * h * 3];
    jpeg->Decode(picture);

    int x, y;
    int vesaWidth  = screen.getWidth();
    int vesaBpp    = screen.getBpp();
    byte* vesaVram = screen.getVRAM();

    for(y = 0; y < h; y++)
    {
        for(x = 0; x < w; x++)
        {
            int k  = (x + (y * vesaWidth)) * vesaBpp / 8;
            int k2 = (x + (y * w)) * 3;
            vesaVram[k]   = picture[k2 + 2];
            vesaVram[k+1] = picture[k2 + 1];
            vesaVram[k+2] = picture[k2 + 0];
        }
    }

    delete picture;
    delete filebuf;
    return 0;
}

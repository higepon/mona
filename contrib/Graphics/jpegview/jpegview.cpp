#include <monapi.h>
#include <monapi/messages.h>
#include <jpegls.h>

/* nikq and Higepon */

using namespace MonAPI;

int main(int argc, char* argv[])
{
    /* check arguments */
    if (argc != 2)
    {
        printf("usage: JPEGVIEW /XXXX.JPG\n");
        return -1;
    }

    /* check bpp */
    Screen screen;
    if (screen.getBpp() < 16)
    {
        printf("screen information (x, y) = (%d, %d) %dbpp\n", screen.getWidth(), screen.getHeight(), screen.getBpp());
        printf("sorry, this demo needs 16bpp or higher Video mode\n");
        return -1;
    }

    /* start */
    printf("wait please......\n");

    CJPEGLS jpeg;

    monapi_cmemoryinfo* mi = NULL;
    mi = monapi_file_read_all(argv[1]);

    if (mi == NULL)
    {
        printf("file %s not found", argv[1]);
        return -1;
    }

    /* jpeg operation */
    if (jpeg.Open(mi->Data, mi->Size) != 0)
    {
        printf("not supported image\n");
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
        return -1;
    }

    int w, h;
    jpeg.GetInfo(&w, &h);
    uint8_t *picture = new uint8_t[w * h * 3];
    jpeg.Decode(picture);

    int x, y;
    int vesaWidth  = screen.getWidth();
    int vesaHeight = screen.getHeight();
    int vesaBpp    = screen.getBpp() / 8;
    int ww         = w < vesaWidth ? w : vesaWidth;
    int hh         = h < vesaHeight ? h : vesaHeight;
    uint8_t* vesaVram = screen.getVRAM();

    for(y = 0; y < hh; y++)
    {
        for(x = 0; x < ww; x++)
        {
            int k  = (x + (y * vesaWidth)) * vesaBpp;
            int k2 = (x + (y * w)) * 3;
            if (vesaBpp == 2)
            {
                *(uint16_t*)&vesaVram[k] = Color::bpp24to565(picture[k2], picture[k2 + 1], picture[k2 + 2]);
            }
            else
            {
                vesaVram[k]   = picture[k2 + 2];
                vesaVram[k+1] = picture[k2 + 1];
                vesaVram[k+2] = picture[k2 + 0];
            }
        }
    }

    delete [] picture;
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    return 0;
}

#include <userlib.h>

int MonaMain(List<char*>* pekoe)
{
    Screen screen;
    int result;
    byte buf[32];

    if (pekoe->isEmpty())
    {
        printf("usage: HELLO.ELF pathToFile\n");
        return -1;
    }

    FileInputStream fis(pekoe->get(0));

    if (0 != (result = fis.open()))
    {
        printf("can not open %s\n", pekoe->get(0));
        return -1;
    }

    printf("file size = %d\n", fis.getFileSize());

    if (fis.read(buf, 32))
    {
        printf("can not read %s\n", pekoe->get(0));
        fis.close();
        return -1;
    }

    printf("contents\n");
    sleep(5000);

    for (int i = 0; i < 32; i++)
    {
        printf("[%x]", buf[i]);
    }

    fis.close();

    return 0;
}

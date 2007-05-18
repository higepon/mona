#include <monapi.h>
#include <monapi/CString.h>
#include <monalibc.h>

#include <monapi.h>

#if 1

int main(int argc, char* argv[])
{
//    char buf[128];
    printf("Hello, World\n");
//    _printf(buf);
    return 0;
}

#endif

#if 0
extern void dlltest_print();

int main(int argc, char* argv[])
{
    printf("Hello, World \n");
    dlltest_print();
}

#endif
#if 0

static int magic1 = 0xdead;
static int magic2 = 0xbeef;
static int magic3 = 0x1234;
static int magic4 = 0x5678;

extern void dlltest_print();

int main(int argc, char* argv[])
{
    printf("Hello, World \n");
    char buf[26];
    sprintf(buf, "hoge");
    dlltest_print();
//    sprintf(NULL, "hoge");

//     int (*f)(void) = (int (*)(void))0xa0004200;
//     int ret = (*f)();
//     printf("ret=%d\n", ret);
    printf("magic1=%x\n", &magic1);
    printf("magic2=%x\n", &magic2);
    printf("magic3=%x\n", &magic3);
    printf("magic4=%x\n", &magic4);

    return 0;
    for (uint32_t i = 0xA0000000; i < 0xB0000000; i+=2)
    {
        uint16_t* value = (uint16_t*)i;
//        if (i % 100 == 0) printf("%x\n", value);
        if (magic1 == *value || magic2 == *value || magic3 == *value || magic4 == *value)
        {
            printf("found address=%x value=%x\n", value, *value);
        }
    }


    printf("not found\n");
    return 0;
}

#endif

#if 0

int main(int argc, char* argv[])
{

    if (weak_func)
    {
        weak_func();
    }
    else
    {
        printf("weak_func not found\n");
    }
}

#endif

#if 0
int main(int argc, char* argv[]) {

    printf("Hello, World!\n");
    return 0;
}

#endif

#if 0
#include <monalibc/setjmp.h>
jmp_buf g_errorjmp;

int main(int argc, char* argv[]) {

    if(setjmp(g_errorjmp) == 0) {
        printf("JUMP\n");
        longjmp(g_errorjmp, 1);
    }
    printf("END\n");
    return 0;
}
#endif

#if 0
// hz = 1193180 / value;
void beep(uint32_t hz)
{
    uint16_t value = (uint16_t)(1193180 / hz);
    outp8(0x43, 0xb6);
    outp8(0x42, value & 0xff);
    outp8(0x42, value >> 8);
    outp8(0x61, (inp8(0x61) | 0x03) & 0x0f);
}

typedef struct
{
    uint32_t hz;

} Unit;

enum
{
    A = 440,
    B = 467,
    C = 523,
    D = 587,
    E = 659,

};

int main(int argc, char* argv[])
{


    syscall_get_io();

    for (uint16_t i = 440; i < 1200; i++)
    {
        beep(i);
        sleep(10);
    }

    // off
    outp8(0x61, inp8(0x61) & 0x0d);
    printf("end ");

    beep(C);
    sleep(500);
    beep(D);
    sleep(500);
    beep(E);
    sleep(500);
    beep(C);
    sleep(500);
    beep(E);
    sleep(500);
    beep(C);
    sleep(500);
    beep(E);
    sleep(500);
    beep(C);

    return 0;
}
#endif
#if 0
    printf("Hello,World!\n");
#endif

#if 0
    if (argc < 2))
    {
        return -1;
    }
    char* command = argv[1];
    if (strcmp("ECHO", command) == 0)
    {
        char* text = pekoe->get(1);
        monapi_stdout_read((uint8_t*)text, strlen(text) + 1);
    }
    else if (strcmp("UPPER", command) == 0)
    {
        uint8_t buffer[256];
        uint32_t size = monapi_stdin_read(buffer, 256);
        for (uint32_t i = 0; i < size; i++)
        {
            printf("%c", toupper(buffer[i]));
        }
    }
    else
    {
        printf("wrong usage");
    }
#endif

#if 0
    uint32_t fileID = monapi_file_open("/AUTOEXEC.MSH", MONAPI_FALSE);
    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 486);
    printf("AUTOEXEC.MSH size = 485, readResult size = %d\n", mi->Size);
    monapi_file_close(fileID);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
#endif

#if 0
    const char* message = "Hello World\n";
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    if (!monapi_cmemoryinfo_create(buffer, strlen(message) + 1, 0))
    {
        monapi_cmemoryinfo_delete(buffer);
        return -1;
    }
    memcpy(buffer->Data, message, buffer->Size);
    uint32_t id = monapi_file_open("/FD/HELLO.TXT", true);
    monapi_file_write(id, buffer, buffer->Size);
    monapi_file_close(id);
    return 0;
}

#endif

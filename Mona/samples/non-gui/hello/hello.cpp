#include <monapi.h>

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    printf("Hello World!\n");
//     double seed = 0.31415926535897931;
//     for (int i = 0; i < 10000000; i++)
//     {
//         seed *= 17.0;
//         seed -= (int)seed;
//     }

//     char buf[64];
//     sprintf(buf, "%1.8f", seed);
//     printf("answer=%s", buf);

    return 0;
}

// int MonaMain(List<char*>* pekoe)
// {
//     byte buf[32];
//     int result;
//    FileInputStream fis("HOGE.TXT");

//    if (0 != (result = fis.open()))
//    {
//        printf("can not open %s\n", pekoe->get(0));
//        return -1;
//    }

//    printf("file size = %d\n", fis.getFileSize());

//    if (fis.read(buf, 32))
//    {
//        printf("can not read %s\n", pekoe->get(0));
//        fis.close();
//        return -1;
//    }

//    printf("contents\n");
//    sleep(5000);

//    for (int i = 0; i < 32; i++)
//    {
//        printf("[%x]", buf[i]);
//    }

//    fis.close();



//     printf("Hello World\n");

//     for (int i = 0; i < 10; i++)
//     {
//         printf("tick=%x\n", System::getTick());
//     }

//     sleep(100);

//     for (int i = 0; i < 10; i++)
//     {
//         printf("tick=%x\n", System::getTick());
//     }

//     return 0;
// }

// Mutex§Úª»§√§∆§ﬂ§ø§ÅE// static Mutex* mutex;

// void sub()
// {
//     printf("sub start\n");
//     sleep(5000);

//     printf("sub:tryLock=%x\n", mutex->tryLock());

//     sleep(10000);

//     printf("sub:lock start\n");
//     printf("sub:Lock=%x\n", mutex->lock());
//     printf("sub:lock done\n");

//     for (;;);
// }

// int MonaMain(List<char*>* pekoe)
// {
//     mutex = new Mutex();

//     int id = syscall_mthread_create((dword)sub);
//     printf("join result = %x", syscall_mthread_join(id));

//     printf("main:lock=%x\n", mutex->lock());

//     sleep(20000);

//     mutex->unlock();
//     printf("main:unlock\n");

//     return 0;
// }

// ∂¶Õ≠•·•‚•Í§ §‰§ƒ
// int MonaMain(List<char*>* pekoe)
// {
    /* 5000byte§Œ∂¶Õ≠•·•‚•ÅE•∞•˙Ωº•–•ÅE§Ú∫˚‹Æ º¬∫›§Œ•µ•§•∫§œ8192byte§À§ §ÅE*/
//     dword id1 = MemoryMap::create(5000);

//     if (id1 == 0)
//     {
//         printf("map create error = %x", MemoryMap::getLastError());
//         exit(1);
//     }

//     printf("shared size = %d", MemoryMap::getSize(id1));

//     /* ∫˚‹Æ§∑§ø∂¶Õ≠•·•‚•Í§Úº´ ¨§Œ∂ı¥÷§À≈Ω§ÅE’§±§ÅE*/
//     byte* p = MemoryMap::map(id1);
//     if (p == NULL)
//     {
//         printf("map error\n");
//         exit(1);
//     }

//     printf("map=[%x]", p);

//     /* ∂¶Õ≠•®•ÅE¢§ÀΩÒ§≠π˛§ﬂ */
//     strcpy((char*)p, "data share top hello!!\n");

//     /* §ø§·§∑§ÀServer§Àid1§Ú¡˜§√§∆§ﬂ§Ë§¶ */
//     dword targetID = Message::lookupMainThread("KEYBDMNG.SVR");
//     if (targetID == 0xFFFFFFFF)
//     {
//         printf("hello:Server not found\n");
//         exit(1);
//     }

//     MessageInfo info;
//     Message::create(&info, MSG_MEMORY_MAP_ID, id1, 0, 0, NULL);

//     /* send */
//     if (Message::send(targetID, &info)) {
//         printf("hello send error\n");
//     }

    /* ∂¶Õ≠•·•‚•Í§Úº´ ¨§Œ∂ı¥÷§´§È§œ§∫§π */
    //MemoryMap::unmap(id1);

    /* §ƒ§§§«§À•’•°•§•ÅE«§‚∫˚¿ÅE´ */
// //     FileOutputStream fos("HELLO.LOG", true);

// //     printf("open=%x\n", fos.open());

// //     char str[] = "Hello! \n Mona can create file and write\n";
// //     printf("write%x\n", fos.write((byte*)str, sizeof(str)));
// //     printf("write%x\n", fos.write((byte*)str, sizeof(str)));
// //     fos.close();
// //     for (;;);
// }


// •’•°•§•ÅEŒ√Êø»§Ú§œ§Ø§‰§ƒ
// int MonaMain(List<char*>* pekoe)
// {
//     int result;
//     byte buf[32];

//     if (pekoe->isEmpty())
//     {
//         printf("usage: HELLO.ELF pathToFile\n");
//         return -1;
//     }

//     FileInputStream fis(pekoe->get(0));

//     if (0 != (result = fis.open()))
//     {
//         printf("can not open %s\n", pekoe->get(0));
//         return -1;
//     }

//     printf("file size = %d\n", fis.getFileSize());

//     if (fis.read(buf, 32))
//     {
//         printf("can not read %s\n", pekoe->get(0));
//         fis.close();
//         return -1;
//     }

//     printf("contents\n");
//     sleep(5000);

//     for (int i = 0; i < 32; i++)
//     {
//         printf("[%x]", buf[i]);
//     }

//     fis.close();

//     return 0;
// }

// •ﬁ•ÅE¡•π•ÅE√•…§ §‰§ƒ
// void sub1() {for (;;) printf("sub1");}
// void sub2() {for (;;) printf("sub2");}
// void sub3() {for (;;) printf("sub3");}

// int MonaMain(List<char*>* pekoe)
// {
//     int id;

//     id = syscall_mthread_create((dword)sub1);
//     printf("join result = %x", syscall_mthread_join(id));

//     id = syscall_mthread_create((dword)sub2);
//     printf("join result = %x", syscall_mthread_join(id));

//     id = syscall_mthread_create((dword)sub3);
//     printf("join result = %x", syscall_mthread_join(id));

//     for (;;)
//     {
//         printf("main");
//     }

//     return 0;
// }

//     Floppy fd(Floppy::FLOPPY_1);
//     fd.open();
//     printf("changed %s\n", fd.diskChanged() ? "true" : "false");
//     sleep(10000);
//     printf("changed %s\n", fd.diskChanged() ? "true" : "false");
//     fd.close();

#include <userlib.h>

// マルチスレッドなやつ
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

// 共有メモリなやつ
int MonaMain(List<char*>* pekoe)
{
    /* 共有したい相手 */
    dword pid = Message::lookup("SHELL.SVR");

    /* メモリマップオブジェクト取得 */
    MemoryMap* mm = MemoryMap::create();

    /* 自分の0x90005000にpidの0x90000000を1Mバイトマッピングする */
    dword sharedid = mm->map(pid, 0x90000000, 0x90005000, 1 * 1024 * 1024);

    /* 共有エリアに書き込み */
    strcpy((char*)0x90005000, "data share top");
    strcpy((char*)0x90095000, "data share bottom");

    //    mm->unmap(sharedid);

    for (;;);
}


// ファイルの中身をはくやつ
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

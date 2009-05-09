#include <monapi.h>
#include <monapi/CString.h>
#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi/io.h>

#include <monapi.h>

#if 1

#define PCI_VENDOR_ID_REDHAT_QUMRANET    0x1af4
#define PCI_SUBVENDOR_ID_REDHAT_QUMRANET 0x1af4
#define PCI_SUBDEVICE_ID_QEMU            0x1100

#define PCI_DEVICE_ID_VIRTIO_NET         0x1000
#define PCI_DEVICE_ID_VIRTIO_BLOCK       0x1001
#define PCI_DEVICE_ID_VIRTIO_BALLOON     0x1002
#define PCI_DEVICE_ID_VIRTIO_CONSOLE     0x1003

/* A 32-bit r/o bitmask of the features supported by the host */
#define VIRTIO_PCI_HOST_FEATURES        0

/* A 32-bit r/w bitmask of features activated by the guest */
#define VIRTIO_PCI_GUEST_FEATURES       4

/* A 32-bit r/w PFN for the currently selected queue */
#define VIRTIO_PCI_QUEUE_PFN            8

/* A 16-bit r/o queue size for the currently selected queue */
#define VIRTIO_PCI_QUEUE_NUM            12

/* A 16-bit r/w queue selector */
#define VIRTIO_PCI_QUEUE_SEL            14

/* A 16-bit r/w queue notifier */
#define VIRTIO_PCI_QUEUE_NOTIFY         16

/* An 8-bit device status register.  */
#define VIRTIO_PCI_STATUS               18

#define VIRTIO_PCI_CONFIG               20
#define PAGE_SHIFT (12)
#define PAGE_SIZE  (1<<PAGE_SHIFT)
#define PAGE_MASK  (PAGE_SIZE-1)

/* Status byte for guest to report progress, and synchronize features. */
/* We have seen device and processed generic fields (VIRTIO_CONFIG_F_VIRTIO) */
#define VIRTIO_CONFIG_S_ACKNOWLEDGE     1
/* We have found a driver for the device. */
#define VIRTIO_CONFIG_S_DRIVER          2
/* Driver has used its parts of the config, and is happy */
#define VIRTIO_CONFIG_S_DRIVER_OK       4
/* We've given up on this device. */
#define VIRTIO_CONFIG_S_FAILED          0x80

#define MAX_QUEUE_NUM      (512)

#define VRING_DESC_F_NEXT  1
#define VRING_DESC_F_WRITE 2

#define VRING_AVAIL_F_NO_INTERRUPT 1

#define VRING_USED_F_NO_NOTIFY     1

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

struct vring_desc
{
   u64 addr;
   u32 len;
   u16 flags;
   u16 next;
};

struct vring_avail
{
   u16 flags;
   u16 idx;
   u16 ring[0];
};

struct vring_used_elem
{
   u32 id;
   u32 len;
};

struct vring_used
{
   u16 flags;
   u16 idx;
   struct vring_used_elem ring[];
};

struct vring {
   unsigned int num;
   struct vring_desc *desc;
   struct vring_avail *avail;
   struct vring_used *used;
};

#define vring_size(num) \
   (((((sizeof(struct vring_desc) * num) + \
      (sizeof(struct vring_avail) + sizeof(u16) * num)) \
         + PAGE_MASK) & ~PAGE_MASK) + \
    (sizeof(struct vring_used) + sizeof(struct vring_used_elem) * num))

int main(int argc, char* argv[])
{
    // 1. Device probe
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_CONSOLE, &pciInf);

    if (!pciInf.isExist)
    {
        printf("device not found\n");
        exit(-1);
    }

    printf("device found\n");
    printf("baseAdress=%x\n", pciInf.baseAdress);
    printf("irqLine=%x\n", pciInf.irqLine);

    const uintptr_t baseAddress = pciInf.baseAdress & ~1;

    // 2. Select the queue to use
    outp16(baseAddress + VIRTIO_PCI_QUEUE_SEL, 1); // 0: read queue, 1:write queue

    // 3. how many descriptors do the queue have?
    const int numberOfDesc = inp16(baseAddress + VIRTIO_PCI_QUEUE_NUM);
    printf("[virtio] numberOfDesc=%d\n", numberOfDesc);
    ASSERT(numberOfDesc > 0);

    // 4. Check wheter the queue is already set vring (necessary?).
    uint16_t pfn = inp16(baseAddress + VIRTIO_PCI_QUEUE_PFN);
    if (pfn != 0) {
        printf("[virtio] pfn=%x\n", pfn);
        exit(-1);
    }

    // 5. setup vring
    const int MAX_QUEUE_SIZE = PAGE_MASK + vring_size(MAX_QUEUE_NUM);
    printf("[virtio] MAX_QUEUE_SIZE=%d\n", MAX_QUEUE_SIZE);
    uint8_t queueData[MAX_QUEUE_SIZE];

    struct vring vring;
    vring.num = numberOfDesc;

    // page align is required
    const uintptr_t physicalAddress = syscall_get_physical_address((uintptr_t)queueData, NULL);
    printf("[virtio] physicalAddress=%x\n", physicalAddress);
    const uintptr_t alignedAddress = (physicalAddress + PAGE_MASK) & ~PAGE_MASK;
    printf("[virtio] alignedAddress=%x\n", alignedAddress);

    ASSERT((alignedAddress % PAGE_SIZE) == 0);

    // vring.desc is page aligned
    vring.desc = (struct vring_desc*)(queueData + alignedAddress - physicalAddress);

    // vring.avail is follow after the array of desc
    vring.avail = (struct vring_avail *)&vring.desc[numberOfDesc];




    // addr は vring->desc
    // vring->desc に実際の物理ページのアドレス、長さなどを設定しないとだめ。
    char* p = new char[4096];
    memset(p, 0xfe, 4096);

#define PAGE_SHIFT (12)
#define PAGE_SIZE  (1<<PAGE_SHIFT)
#define PAGE_MASK  (PAGE_SIZE-1)

    p = (char*)(((uint32_t)p + PAGE_MASK) & ~PAGE_MASK);
    struct vring_desc* desc = (struct vring_desc*)p;

    ASSERT(((uint32_t)desc % 4096) == 0);

    char* q = new char[4096];
    memset(q, 0xfe, 4096);
    p = (char*)(((uint32_t)q + PAGE_MASK) & ~PAGE_MASK);

    strcpy(q, "Hello world aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    desc->addr = syscall_get_physical_address((uint32_t)q, NULL) >> 12;
    ASSERT(((uint32_t)q % 4096) == 0);
    desc->len = strlen(q) + 1;

    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, syscall_get_physical_address((uint32_t)desc, NULL) >> 12); // sel = 0

//    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, 0 /* means reset sel = 1 */);
//    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, (uint32_t)addr >> 12); // sel = 0
    outp16((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_NOTIFY, /* sel = 1 */ 1);

    return 0;
}


#if 0

struct virtio_net_config
{
    /* The config defining mac address (6 bytes) */
    uint8_t mac[6];
    /* See VIRTIO_NET_F_STATUS and VIRTIO_NET_S_* above */
    uint16_t status;
} __attribute__((packed));

int main(int argc, char* argv[])
{
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_NET, &pciInf);

    if (pciInf.isExist)
    {
        _printf("device found\n");
        _printf("base=%x\n", pciInf.baseAdress);
        _printf("base=%x\n", pciInf.baseAdress & ~1);
        _printf("irqLine=%x\n", pciInf.irqLine);


        _printf("%x", inp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_HOST_FEATURES));

        struct virtio_net_config config;
        for (int i = 0; i < sizeof(config); i += 4) {
            (((uint32_t*)&config)[i / 4]) = inp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_CONFIG + i);
//            (((uint32_t*)&config)[i / 4]) = pci.ReadConfig(0, pciInf.device, 0, (pciInf.baseAdress & ~1) + VIRTIO_PCI_CONFIG + i, 4);
        }

        printf("config.mac= %x:%x:%x:%x:%x:%x\n", config.mac[0], config.mac[1], config.mac[2], config.mac[3], config.mac[4], config.mac[5]);
    } else {
        _printf("device not found\n");
    }

    char* p = new char[4096];
    memset(p, 0xfe, 4096);
    _printf("addr[0] = %x\n", p[0]);

    char* addr = (char*)syscall_get_physical_address((uint32_t)p, 0);

    printf("addr index = %d\n", (uint32_t)addr / 4096);
    // 0 means reset
    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, 0);
    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_PFN, (uint32_t)addr / 4096);
    outp32((pciInf.baseAdress & ~1) + VIRTIO_PCI_QUEUE_NOTIFY, 0);

    return 0;
}

#endif
#if 0
int main(int argc, char* argv[])
{
    PciInf pciInf;
    Pci pci;
    pci.CheckPciExist(PCI_VENDOR_ID_REDHAT_QUMRANET, PCI_DEVICE_ID_VIRTIO_CONSOLE, &pciInf);

    if (pciInf.isExist)
    {
        _printf("device found\n");
        _printf("base=%x\n", pciInf.baseAdress & ~1);
        _printf("irqLine=%x\n", pciInf.irqLine);

        for (int i = 0; i < 0xff; i++) {
            outp8(pciInf.baseAdress, i);
        }

    } else {
        _printf("device not found\n");
    }
    return 0;
}
#endif

#endif

#if 0
/*
 * simple.c
 */
//#include <stdio.h>
//#include <string.h>
#include "oniguruma.h"

extern int main(int argc, char* argv[])
{
  int r;
  unsigned char *start, *range, *end;
  regex_t* reg;
  OnigErrorInfo einfo;
  OnigRegion *region;

  char buf[32];
  malloc(30);
  sprintf(buf, "hige");

  static UChar* pattern = (UChar* )"a(.*)b|[e-f]+";
  static UChar* str     = (UChar* )"zzzzaffffffffb";
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  r = onig_new(&reg, pattern, pattern + strlen((char* )pattern),
    ONIG_OPTION_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT, &einfo);
  if (r != ONIG_NORMAL) {
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((uint8_t*)s, r, &einfo);
    fprintf(stderr, "ERROR: %s\n", s);
    return -1;
  }
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  region = onig_region_new();
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  end   = str + strlen((char* )str);
  start = str;
  range = end;
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  r = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
  if (r >= 0) {
    int i;
  _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    fprintf(stderr, "match at %d\n", r);
    for (i = 0; i < region->num_regs; i++) {
      fprintf(stderr, "%d: (%d-%d)\n", i, region->beg[i], region->end[i]);
    }
  }
  else if (r == ONIG_MISMATCH) {
    fprintf(stderr, "search fail\n");
  }
  else { /* error */
    char s[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str((uint8_t*)s, r);
    fprintf(stderr, "ERROR: %s\n", s);
    return -1;
  }

  onig_region_free(region, 1 /* 1:free self, 0:free contents only */);
  onig_free(reg);
  onig_end();
  return 0;
}

#endif

#if 0

#include <gui/System/Mona/Info.h>

int main(int argc, char* argv[])
{
//    char buf[128];
    printf("Hello, World\n");
    int num;
    uint32_t* handles = System::Mona::gui_enum_windows(&num);
    for (int i = 0; i < num; i++)
    {
        char buffer[WINDOW_TITLE_MAX_LENGTH];
        System::Mona::gui_get_window_title(handles[i], buffer);
        printf("handle=%x %s\n", handles[i], buffer);
    }
    delete[] handles;
//    _printf(buf);
    return -1;
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

#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>


#include <vector>
#include <drivers/virtio/VirtioDevice.h>
#include <drivers/virtio/virtio_ids.h>
#include <drivers/virtio/virtio_blk.h>
#include <drivers/virtio/VirtioBlock.h>

static void test_probe()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
    ASSERT_TRUE(vdev.get() != NULL);
}

static void test_probe_not_found()
{
    // There is only two virtio net devices.
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET, 2));
    ASSERT_TRUE(vdev.get() == NULL);
}

static void test_get_irq()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
    int irq = vdev->getIRQ();
    EXPECT_TRUE(irq >= 0 && irq <= 15);
}

static void test_get_basereg()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
    int basereg = vdev->getBaseReg();
    EXPECT_TRUE(basereg > 0);
}

static void test_get_features()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK));
    ASSERT_TRUE(vdev.get() != NULL);
    EXPECT_EQ(false, vdev->hasFeature(VIRTIO_BLK_F_RO));
}

static void test_get_config()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    struct virtio_blk_config config;
    vdev->getConfig(&config, 0, sizeof(struct virtio_blk_config));
    const int SECTOR_SIZE = 512;
    const int FLOPPY_CAPACITY = 1440 * 1024;
    EXPECT_EQ(FLOPPY_CAPACITY / SECTOR_SIZE, config.capacity);
}

static void test_get_status()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    uint8_t status = 0xff;
    status = vdev->getStatus();
    EXPECT_TRUE(status != 0xff);
}

static void test_reset()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    vdev->reset();
    EXPECT_EQ(VIRTIO_BLK_S_OK, vdev->getStatus());
}

static void test_find_vq()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    EXPECT_TRUE(vq.get() != NULL);

    scoped_ptr<VirtQueue> vq2(vdev->findVirtQueue(1));
    // virtio block has only one queue
    EXPECT_EQ(NULL, vq2.get());
}

static void test_virtqueue_add_buf()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));

    int freeNum = vq->getFreeDescCount();
    std::vector<VirtBuffer> out;
    std::vector<VirtBuffer> in;
    out.push_back(VirtBuffer((void*)0x3000, 0x4000));
    in.push_back(VirtBuffer((void*)0x1000, 0x2000));


    EXPECT_EQ(M_OK, vq->addBuf(out, in, NULL));
    EXPECT_EQ(freeNum - 2, vq->getFreeDescCount());
    EXPECT_EQ(1, vq->getAddedBufCount());

    struct vring& vring = vq->getVring();
    EXPECT_EQ(0x4000, vring.desc[0].len);
    EXPECT_EQ(1, vring.desc[0].next);
    EXPECT_EQ(0x2000, vring.desc[1].len);
    EXPECT_EQ(0, vring.avail->idx);
    EXPECT_EQ(0, vring.avail->ring[0]);
}

static void test_virtqueue_kick()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    vq->kick();
}

static uint8_t* virtio_blk_read(ContigousMemory* m, int sector, int sizeToRead, int& readStatus)
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    struct virtio_blk_outhdr* hdr = (struct virtio_blk_outhdr*)m->get();
    std::vector<VirtBuffer> out;
    hdr->type = VIRTIO_BLK_T_IN ;
    hdr->ioprio = 0;
    hdr->sector = sector;
    out.push_back(VirtBuffer(hdr, sizeof(struct virtio_blk_outhdr)));
    std::vector<VirtBuffer> in;
    uint8_t* status = (uint8_t*)((uintptr_t)m->get() + sizeof(struct virtio_blk_outhdr));
    *status = 0xff;
    uint8_t* buf = (uint8_t*)((uintptr_t)m->get() + sizeof(struct virtio_blk_outhdr) + 1);
    memset(buf, 0, sizeToRead);
    in.push_back(VirtBuffer(buf, sizeToRead));
    in.push_back(VirtBuffer(status, 1));

    // this assert is raised
    EXPECT_EQ(M_OK, vq->addBuf(out, in, (void*)0x1234));
    vq->kick();
    while (!vq->isUsedBufExist()) {
    }
    int sizeRead = 0;
    void* ret = vq->getBuf(sizeRead);
    EXPECT_EQ(0x1234, (uintptr_t)ret);
    EXPECT_EQ(sizeToRead, sizeRead - sizeof(*status));

    readStatus = *status;
    return buf;
}

static void test_virtio_blk_read()
{
    scoped_ptr<ContigousMemory> m(ContigousMemory::allocate(4096));
    int status;
    uint8_t* buf = virtio_blk_read(m.get(), 0, 1023, status);
    EXPECT_EQ(0xeb, buf[0]);
    EXPECT_EQ(VIRTIO_BLK_S_OK, status);
    EXPECT_TRUE(buf[1024] == 0);
}

static uint8_t* issue_block_read_request(VirtQueue* vq, ContigousMemory* m, int sector, int sizeToRead, int cookie)
{
    struct virtio_blk_outhdr* hdr = (struct virtio_blk_outhdr*)m->get();

    std::vector<VirtBuffer> out;
    hdr->type = VIRTIO_BLK_T_IN ;
    hdr->ioprio = 0;
    hdr->sector = sector;
    out.push_back(VirtBuffer(hdr, sizeof(struct virtio_blk_outhdr)));

    std::vector<VirtBuffer> in;
    uint8_t* status = (uint8_t*)((uintptr_t)m->get() + sizeof(struct virtio_blk_outhdr));
    *status = 0xff;

    uint8_t* buf = (uint8_t*)((uintptr_t)m->get() + sizeof(struct virtio_blk_outhdr) + 1);
    memset(buf, 0, sizeToRead);
    in.push_back(VirtBuffer(buf, sizeToRead));

    in.push_back(VirtBuffer(status, 1));

    EXPECT_EQ(M_OK, vq->addBuf(out, in, (void*)cookie));
    return status;
}

static void test_virtio_blk_read_get_buf_shoud_return_desc()
{
    scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    scoped_ptr<ContigousMemory> req1(ContigousMemory::allocate(4096));
    scoped_ptr<ContigousMemory> req2(ContigousMemory::allocate(4096));

    const int sizeToRead1 = 512;
    const int sizeToRead2 = 513;
    uint8_t* status1 = issue_block_read_request(vq.get(), req1.get(), 0, sizeToRead1, 0x1000);
    uint8_t* status2 = issue_block_read_request(vq.get(), req1.get(), 1, sizeToRead2, 0x1001);
    vq->kick();
    while (!vq->isUsedBufExist()) {
    }

    int sizeRead = 0;

    EXPECT_TRUE(vq->isUsedBufExist());
    void* ret2 = vq->getBuf(sizeRead);
    EXPECT_EQ(0x1001, (uintptr_t)ret2);
    EXPECT_EQ(sizeToRead2, sizeRead - sizeof(uint8_t));
    EXPECT_EQ(VIRTIO_BLK_S_OK, *status2);
    EXPECT_EQ(125, vq->getFreeDescCount());

    while (!vq->isUsedBufExist()) {
    }
    void* ret = vq->getBuf(sizeRead);
    EXPECT_EQ(0x1000, (uintptr_t)ret);
    EXPECT_EQ(sizeToRead1, sizeRead - sizeof(uint8_t));
    EXPECT_EQ(VIRTIO_BLK_S_OK, *status1);
    EXPECT_EQ(128, vq->getFreeDescCount());
}

static void test_virtio_blk_read_out_of_range()
{
    scoped_ptr<ContigousMemory> m(ContigousMemory::allocate(4096));
    int status;
    virtio_blk_read(m.get(), 9999999, 1024, status);
    EXPECT_EQ(VIRTIO_BLK_S_IOERR, status);
}

static void test_contigous_memory()
{
    scoped_ptr<ContigousMemory> m(ContigousMemory::allocate(5000));
    ASSERT_TRUE(m != NULL);
    ASSERT_TRUE(m->get() != NULL);
    uintptr_t paddr1 = syscall_get_physical_address((uintptr_t)m->get());
    uintptr_t paddr2 = syscall_get_physical_address((uintptr_t)m->get() + MAP_PAGE_SIZE);
    EXPECT_EQ(0, paddr1 % MAP_PAGE_SIZE);
    EXPECT_EQ(paddr1 + MAP_PAGE_SIZE, paddr2);
}

static void test_contigous_memory_laddress_should_be_reused()
{
    ContigousMemory* m1 = ContigousMemory::allocate(5000);
    uintptr_t paddr1 = syscall_get_physical_address((uintptr_t)m1);
    delete m1;

    ContigousMemory* m2 = ContigousMemory::allocate(5000);
    uintptr_t paddr2 = syscall_get_physical_address((uintptr_t)m2);
    delete m2;
    EXPECT_EQ(paddr2, paddr1);
}

static void test_virtio_block_class_probe()
{
    scoped_ptr<VirtioBlock> vb(VirtioBlock::probe(1));
    ASSERT_TRUE(vb.get() != NULL);
    EXPECT_EQ(512, vb->getSectorSize());

    const int FLOPPY_CAPACITY = 1440 * 1024;
    EXPECT_EQ(FLOPPY_CAPACITY / vb->getSectorSize(), vb->getCapacity());
}

static void test_virtio_block_class_read()
{
    scoped_ptr<VirtioBlock> vb(VirtioBlock::probe(1));
    ASSERT_TRUE(vb.get() != NULL);
    uint8_t buf[512];
    EXPECT_EQ(512, vb->read(buf, 0, 512));
    EXPECT_EQ(0xeb, buf[0]);
}

static void test_virtio_block_class_read_many_times()
{
    scoped_ptr<VirtioBlock> vb(VirtioBlock::probe(1));
    ASSERT_TRUE(vb.get() != NULL);
    uint64_t s = MonAPI::Date::nowInMsec();
    for (int i = 0; i < 1000; i++) {
        uint8_t buf[512];
        EXPECT_EQ(512, vb->read(buf, i, 512));
    }
    uint64_t e = MonAPI::Date::nowInMsec();
    EXPECT_TRUE(e - s < 300);
}

static void test_virtio_block_class_read_large()
{
    scoped_ptr<VirtioBlock> vb(VirtioBlock::probe(1));
    ASSERT_TRUE(vb.get() != NULL);
    uint64_t s = MonAPI::Date::nowInMsec();
    const int oneMegaByte = 1024 * 1024;
    uint8_t* buf = new uint8_t[oneMegaByte];
    EXPECT_EQ(oneMegaByte, vb->read(buf, 0, oneMegaByte));
    uint64_t e = MonAPI::Date::nowInMsec();
    EXPECT_TRUE(e - s < 100);
    EXPECT_EQ(0xeb, buf[0]);
    delete[] buf;
}

#if 0
static void test_contigous_memory_overflow_should_die()
{
    scoped_ptr<ContigousMemory> m(ContigousMemory::allocate(4096));
    memset(m->get(), 0, 5000);
}
#endif

int main(int argc, char *argv[])
{
    test_probe();
    test_probe_not_found();
    test_get_irq();
    test_get_basereg();
    test_get_features();
    test_get_config();
    test_get_status();
    test_reset();
    test_find_vq();
    test_virtqueue_add_buf();
    test_virtqueue_kick();
    test_virtio_blk_read();
    test_virtio_blk_read_get_buf_shoud_return_desc();
    test_virtio_blk_read_out_of_range();
    test_contigous_memory();
    test_contigous_memory_laddress_should_be_reused();

    test_virtio_block_class_probe();
    test_virtio_block_class_read();
    test_virtio_block_class_read_many_times();
    test_virtio_block_class_read_large();

    // todo
    //   with interrupt
    //   contigous size over
    // read many
    //   memory pool


// Will die!
//    test_contigous_memory_overflow_should_die();

    TEST_RESULTS(virtio);
    return 0;
}


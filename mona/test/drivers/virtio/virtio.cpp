#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>


#include <vector>
#include <drivers/virtio/VirtioDevice.h>
#include <drivers/virtio/virtio_ids.h>
#include <drivers/virtio/virtio_blk.h>
#include <boost/scoped_ptr.hpp>

static void test_probe()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
    ASSERT_TRUE(vdev.get() != NULL);
}

static void test_probe_not_found()
{
    // There is only two virtio net devices.
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET, 2));
    ASSERT_TRUE(vdev.get() == NULL);
}

static void test_get_irq()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
    int irq = vdev->getIRQ();
    EXPECT_TRUE(irq >= 0 && irq <= 15);
}

static void test_get_basereg()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
    int basereg = vdev->getBaseReg();
    EXPECT_TRUE(basereg > 0);
}

static void test_get_features()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK));
    ASSERT_TRUE(vdev.get() != NULL);
    EXPECT_EQ(false, vdev->hasFeature(VIRTIO_BLK_F_RO));
}

static void test_get_config()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    struct virtio_blk_config config;
    vdev->getConfig(&config, 0, sizeof(struct virtio_blk_config));
    const int SECTOR_SIZE = 512;
    const int FLOPPY_CAPACITY = 1440 * 1024;
    EXPECT_EQ(FLOPPY_CAPACITY / SECTOR_SIZE, config.capacity);
}

static void test_get_status()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    uint8_t status = 0xff;
    status = vdev->getStatus();
    EXPECT_TRUE(status != 0xff);
}

static void test_reset()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    vdev->reset();
    EXPECT_EQ(VIRTIO_BLK_S_OK, vdev->getStatus());
}

static void test_find_vq()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    boost::scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    EXPECT_TRUE(vq.get() != NULL);

    boost::scoped_ptr<VirtQueue> vq2(vdev->findVirtQueue(1));
    // virtio block has only one queue
    EXPECT_EQ(NULL, vq2.get());
}

static void test_virtqueue_add_buf()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    boost::scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));

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
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    boost::scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    vq->kick();
}

static void test_virtio_blk_read()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    boost::scoped_ptr<VirtQueue> vq(vdev->findVirtQueue(0));
    boost::scoped_ptr<ContigousMemory> m(ContigousMemory::allocate(4096));

    struct virtio_blk_outhdr* hdr = (struct virtio_blk_outhdr*)m->get();
    std::vector<VirtBuffer> out;
    hdr->type = VIRTIO_BLK_T_IN ;
    hdr->ioprio = 0;
    hdr->sector = 0;
    out.push_back(VirtBuffer(hdr, sizeof(struct virtio_blk_outhdr)));

    std::vector<VirtBuffer> in;
    uint8_t* status = (uint8_t*)((uintptr_t)m->get() + sizeof(struct virtio_blk_outhdr));
    *status = 0xff;

    uint8_t* buf = (uint8_t*)((uintptr_t)m->get() + sizeof(struct virtio_blk_outhdr) + 1);
    const int SIZE_TO_READ = 1024;
    memset(buf, 0, SIZE_TO_READ);
    in.push_back(VirtBuffer(buf, SIZE_TO_READ));

    in.push_back(VirtBuffer(status, 1));

    EXPECT_EQ(M_OK, vq->addBuf(out, in, NULL));
    vq->kick();

    while (!vq->isUsedBufExist()) {
    }

    EXPECT_EQ(VIRTIO_BLK_S_OK, *status);
    EXPECT_EQ(0xeb, buf[0]);
    EXPECT_TRUE(buf[SIZE_TO_READ - 1] != 0);
}

static void test_contigous_memory()
{
    boost::scoped_ptr<ContigousMemory> m(ContigousMemory::allocate(5000));
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
    test_contigous_memory();
    test_contigous_memory_laddress_should_be_reused();

    TEST_RESULTS(virtio);
    return 0;
}

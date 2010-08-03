#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

#include <drivers/virtio/VirtioDevice.h>
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
    uint8_t status = 0xff; //
    status = vdev->getStatus();
    logprintf("status=%d\n", status);
    EXPECT_TRUE(status != 0xff);
}

static void test_reset()
{
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_BLOCK, 1));
    vdev->reset();
    EXPECT_EQ(VIRTIO_BLK_S_OK, vdev->getStatus());
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
    TEST_RESULTS(virtio);
    return 0;
}

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
    // There is only one virtio net device.
    boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET, 1));
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
   boost::scoped_ptr<VirtioDevice> vdev(VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET));
   vdev->getConfig(dest, offset, length);
}

int main(int argc, char *argv[])
{
    syscall_get_io();
    test_probe();
    test_probe_not_found();
    test_get_irq();
    test_get_basereg();
    test_get_features();

    TEST_RESULTS(virtio);
    return 0;
}

#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

#include <drivers/virtio/VirtioDevice.h>

static void test_probe()
{
    VirtioDevice* vdev = VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET);
    ASSERT_TRUE(vdev != NULL);
    delete vdev;
}

static void test_probe_not_found()
{
    // There is only one virtio net device.
    VirtioDevice* vdev = VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET, 1);
    ASSERT_TRUE(vdev == NULL);
}

static void test_get_irq()
{
    VirtioDevice* vdev = VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET);
    int irq = vdev->getIRQ();
    EXPECT_TRUE(irq >= 0 && irq <= 15);
    delete vdev;
}

static void test_get_basereg()
{
    VirtioDevice* vdev = VirtioDevice::probe(PCI_DEVICE_ID_VIRTIO_NET);
    int basereg = vdev->getBaseReg();
    EXPECT_TRUE(basereg > 0);
    delete vdev;
}

int main(int argc, char *argv[])
{
    test_probe();
    test_probe_not_found();
    test_get_irq();
    test_get_basereg();

    TEST_RESULTS(virtio);
    return 0;
}

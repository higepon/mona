/*!
    \file  tulipdiag.cpp
    \brief tulip diag

    \author  http://www.scyld.com/ethercard_diag.html
    \version $Revision$
    \date   create:2005/12/09 update:$Date$
*/

#include <monapi.h>
//#include <monapi/messages.h>
#include <monapi/io.h>
//#include <monapi/CString.h>
//#include <monalibc/stdio.h>


using namespace MonAPI;


/* Values read from the EEPROM, and the new image to write. */
#define EEPROM_SIZE 256         /* Size may be 256x16 for CardBus. */
static uint16_t eeprom_contents[EEPROM_SIZE];
//static uint16_t new_ee_contents[EEPROM_SIZE];

enum tulip_offsets {
    CSR0=0,    CSR1=0x08, CSR2=0x10, CSR3=0x18, CSR4=0x20, CSR5=0x28,
    CSR6=0x30, CSR7=0x38, CSR8=0x40, CSR9=0x48, CSR10=0x50, CSR11=0x58,
    CSR12=0x60, CSR13=0x68, CSR14=0x70, CSR15=0x78 };


//#define eeprom_delay() do { int _i = 3; while (--_i > 0) { ; }} while (0)
#define eeprom_delay() inp32(ee_addr)



/*  EEPROM_Ctrl bits. */
#define EE_SHIFT_CLK    0x02    /* EEPROM shift clock. */
#define EE_CS           0x01    /* EEPROM chip select. */
#define EE_DATA_WRITE   0x04    /* EEPROM chip data in. */
#define EE_WRITE_0      0x4801
#define EE_WRITE_1      0x4805
#define EE_DATA_READ    0x08    /* EEPROM chip data out. */
#define EE_ENB          (0x4800 | EE_CS)

/* The EEPROM commands include the alway-set leading bit. */
#define EE_WRITE_CMD    (5)
#define EE_READ_CMD     (6)
#define EE_ERASE_CMD    (7)

/* Note: this routine returns extra data bits for size detection. */
static int read_eeprom(long ioaddr, int location, int addr_len)
{
    int i;
    unsigned retval = 0;
    long ee_addr = ioaddr + CSR9;
    int read_cmd = location | (EE_READ_CMD << addr_len);

    outp32(EE_ENB & ~EE_CS, ee_addr);
    outp32(EE_ENB, ee_addr);

    //printf(" EEPROM read at %d ", location);

    /* Shift the read command bits out. */
    for (i = 4 + addr_len; i >= 0; i--) {
        short dataval = (read_cmd & (1 << i)) ? EE_DATA_WRITE : 0;
        outp32(EE_ENB | dataval, ee_addr);
        eeprom_delay();
        outp32(EE_ENB | dataval | EE_SHIFT_CLK, ee_addr);
        eeprom_delay();
        //printf("%x", inp32(ee_addr) & 15);
        retval = (retval << 1) | ((inp32(ee_addr) & EE_DATA_READ) ? 1 : 0);
    }
    outp32(EE_ENB, ee_addr);
    eeprom_delay();
    //printf(" :%x:", inp32(ee_addr) & 15);

    for (i = 16; i > 0; i--) {
        outp32(EE_ENB | EE_SHIFT_CLK, ee_addr);
        eeprom_delay();
        //printf("%x", inp32(ee_addr) & 15);
        retval = (retval << 1) | ((inp32(ee_addr) & EE_DATA_READ) ? 1 : 0);
        outp32(EE_ENB, ee_addr);
        eeprom_delay();
    }

    /* Terminate the EEPROM access. */
    outp32(EE_ENB & ~EE_CS, ee_addr);
    //printf(" EEPROM value at %d is %x.\n", location, retval);
    return retval;
}


static void parse_eeprom(long ioaddr, unsigned char *ee_data, int flags)
{
    int sum;
    int i;
    
    for (i = 0, sum = 0xff; i < 6; i++)
        sum &= ee_data[i];
    if (sum == 0xff) {
        printf(" This interface is missing the EEPROM.\n  This is likely the "
               "non-primary interface on a multiport board.\n");
        return;
    }
    
    printf("PCI Subsystem IDs, vendor %2x%2x, device %2x%2x.\n"
           "CardBus Information Structure at offset %2x%2x%2x%2x.\n",
           ee_data[1], ee_data[0], ee_data[3], ee_data[2],
           ee_data[7], ee_data[6], ee_data[5], ee_data[4]);

    printf("Ethernet MAC Station Address "
           "%2x:%2x:%2x:%2x:%2x:%2x.\n",
           ee_data[20], ee_data[21], ee_data[22],
           ee_data[23], ee_data[24], ee_data[25]);
}



int main(int argc, char* argv[]) 
{

    syscall_get_io();
    long ioaddr = 0xEC00;    
    
    
    /* It's mostly safe to use the Tulip EEPROM and MDIO register during
   operation.  But warn the user, and make then pass '-f'. */
    if ((inp32(ioaddr + CSR6) & 0x2002) != 0) {
        printf(" * A potential Tulip chip has been found, but it appears to "
               "be active.\n * Either shutdown the network, or use the"
               " '-f' flag to see all values.\n");
    }
    
    int eeprom_uint16_ts = 64;
    int i;

    /* Read the EEPROM. */
    memset(eeprom_contents, 0, sizeof(eeprom_contents));    
    


    uint16_t *eew = (uint16_t *)eeprom_contents;
    uint16_t andsum = 0xffff;
    int ee_addr_size = read_eeprom(ioaddr, 0xff, 8) & 0x40000 ? 8 : 6;
    eeprom_uint16_ts = ee_addr_size == 8 ? 256 : 64;
    
    
    printf("ee_addr_size = %d  eeprom_uint16_ts = %d\n",ee_addr_size,eeprom_uint16_ts);

    for (i = 0; i < eeprom_uint16_ts; i++)
        andsum &= (eew[i] = read_eeprom(ioaddr, i, ee_addr_size));
    if (andsum == 0xffff)
        printf("WARNING: The EEPROM is missing or erased!\n");
    
    
    
    parse_eeprom(ioaddr, (unsigned char *)eeprom_contents, 16);
    //‚±‚±‚ÅI/O ‚Ç‚ê‚à 0xFF _|P|Z
//    printf("Read EEP20 = %x\n",read_eeprom(0xEC00 , 20 ,8));
//    printf("Read EEP21 = %x\n",read_eeprom(0xEC00 , 21 ,8));
//    printf("Read EEP22 = %x\n",read_eeprom(0xEC00 , 22 ,8));
//    printf("Read EEP23 = %x\n",read_eeprom(0xEC00 , 23 ,8));
//    printf("Read EEP24 = %x\n",read_eeprom(0xEC00 , 24 ,8));
//    printf("Read EEP25 = %x\n",read_eeprom(0xEC00 , 25 ,8));


    return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HogeDriver.h"
#include "FAT12.h"

int main(int argc, char *argv[]) {

    DiskDriver* driver = new HogeDriver("./floppy.dat", 50);
    FAT12*      fat    = new FAT12(driver);

    if (!fat->initilize()) printf("\nfat initilze error\n");

    /* FAT12#bpp_ should be private, but this is test */
    for (int i = 0; i < 8; i++) printf("%c", (char)(fat->bpb_.oemName[i]));
    printf("\n");
    printf("sizeOfSector=%d\n", fat->bpb_.sizeOfSector);
    printf("sectorPerCluster=%d\n", fat->bpb_.sectorPerCluster);
    printf("reservedSector=  %d\n", fat->bpb_.reservedSector  );
    printf("NumberOfFat=     %d\n", fat->bpb_.NumberOfFat     );
    printf("rootEntries=     %d\n", fat->bpb_.rootEntries     );
    printf("totalSector=     %d\n", fat->bpb_.totalSector     );
    printf("media=           %d\n", fat->bpb_.media           );
    printf("fatSize=         %d\n", fat->bpb_.fatSize         );
    printf("sectorPerTrack=  %d\n", fat->bpb_.sectorPerTrack  );
    printf("numberOfHeads=   %d\n", fat->bpb_.numberOfHeads   );
    printf("hiddenSector=    %d\n", fat->bpb_.hiddenSector    );
    printf("totalBigSector=  %d\n", fat->bpb_.totalBigSector  );
    printf("driveId=         %d\n", fat->bpb_.driveId         );
    printf("dirtyFlag=       %d\n", fat->bpb_.dirtyFlag       );
    printf("extendBootSign=  %d\n", fat->bpb_.extendBootSign  );
    printf(" serialNo=       %d\n", fat->bpb_. serialNo       );
    for (int i = 0; i < 11; i++) printf("%c", (char)(fat->bpb_.volume[i]));
    printf("\n");
    for (int i = 0; i < 8; i++) printf("%c", (char)(fat->bpb_.type[i]));
    printf("\n");
    delete fat;
    delete driver;

    printf("size of FTIME = %d FDATE = %d", sizeof(FTIME), sizeof(FDATE));

    return 0;
}

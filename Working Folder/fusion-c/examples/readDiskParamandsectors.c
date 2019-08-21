//
// FUSION-C  Example
// Read and Show Disk Param  (Only with MSX-DOS2)
// Read Sectors to VRAM
//
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/vdp_graph2.h"
#include "fusion-c/header/io.h"
#include <stdio.h>


DSKPARAMS info;             // Initialising variable Structure for the GetDiskParam function

unsigned char Buffer[512];  // Buffer to store 1 Sector data

void main(void)
{
    unsigned int adr;
    unsigned char r;
    int i;

    adr=GetDiskTrAdress();
    printf("\n\r Current Disk transfer Adress is : %x \n\r",adr);

    r=GetDiskParam(&info,0);
    if (r==0) 
    {
        printf("\n\r DriveN : %d",info.DriveN);
        printf("\n\r SectorSize : %d",info.SectorSize);
        printf("\n\r SectorPerCluster : %d",info.SectorPerCluster);
        printf("\n\r ReservedSector : %d",info.NumberReservedSector);
        printf("\n\r NumberFatCopy : %d",info.NumberFatCopy);
        printf("\n\r NumberRootDirEntries : %d",info.NumberRootDirEntries);
        printf("\n\r TotalLogicalSectors : %d",info.TotalLogicalSectors);
        printf("\n\r MediaDescriptionByte : %x",info.MediaDescriptionByte);
        printf("\n\r NumberSectorsPerFat : %d",info.NumberSectorsPerFat);
        printf("\n\r FirstRootSectorNumber : %d",info.FirstRootSectorNumber);
        printf("\n\r FirstDataSectorNumber : %d",info.FirstDataSectorNumber);
        printf("\n\r MaximumCluster : %d",info.MaximumCluster);
        printf("\n\r DirtyFlag : %d",info.DirtyFlag);
        printf("\n\r VolumeId : %x",info.VolumeId);
        printf("\n\r Reserved : %x \n\r",info.Reserved);
    }
    else 
    {
      printf("\n\r ERROR : %d",r);
    }
    WaitForKey();
    Screen(8);
    FillVram(0, 255, 54272);            // Clear Image to White

    SetDiskTrAdress(&Buffer[0]);        // Set Transfer Disk Adress to Buffer 
    for (i = 0; i < 106; ++i)           // Read 106 first Sectors
    {
      SectorRead(i, 0, 1);              // Read 1 sector to Buffer
      HMMC( &Buffer, 0, i*2, 256, 2 );  // transfer Buffer to VRAM
    }

    WaitForKey();
    Screen(0);
    adr=GetDiskTrAdress();
    printf("\n\rCurrent Disk transfer Adress is: %x \n\r",adr);
    Exit(0);
}
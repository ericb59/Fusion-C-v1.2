#include <stdlib.h>
#include <stdio.h>
#include "fusion-c/header/msx_fusion.h"
#include "fusion-c/header/rammapper.h"


#define  HALT __asm halt __endasm   //wait for the next interrupt


// Generates a pause in the execution of n interruptions.
// PAL: 50=1second. ; NTSC: 60=1second. 
void Wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) HALT;
  return;
}





    MAPPERINFOBLOCK *table;


void printRamMapperStatus() {
    printf("Mappers status: \r\n");
    printf("============== \r\n");  

    table = _GetRamMapperBaseTable();
    while (table->slot!=0) {
        printf("Slot #%x \r\n",table->slot);
        printf("16KB Segments #%x \r\n",table->number16KBSegments);
        printf("Free 16KB Segments #%x \r\n",table->numberFree16KBSegments);
        printf("Allocated System 16KB Segments #%x \r\n",table->numberAllocatedSystem16KBSegments);
        printf("User 16KB Segments #%x \r\n",table->numberUser16KBSegments);                              
        printf("========================================== \r\n");      
        table = table + 1;      //one record (8 bytes per record)
    }
}


void main(void)
{ 

    unsigned char *p;
    SEGMENTSTATUS *status;
    unsigned char segmentId0,segmentId1,initialSegment;

    InitRamMapperInfo(4);
     Cls();  
    printRamMapperStatus();
    printf("\r\n\r\n***********Allocating 2 user blocks\r\n");
    WaitKey();
    Cls();

    status = AllocateSegment(0,0);
    segmentId0 = status->allocatedSegmentNumber;
    printf("+++++++++++++++++++++  Block allocated , segmentId=#%x slot=#%x error=#%x \r\n",segmentId0, status->slotAddressOfMapper, status->carryFlag);
    printf("\r\n\r\n");
    
    status = AllocateSegment(0,0);
    segmentId1 = status->allocatedSegmentNumber;
    printf("+++++++++++++++++++++  Block allocated , segmentId=#%x slot=#%x error=#%x \r\n",segmentId1, status->slotAddressOfMapper, status->carryFlag);
    printf("\r\n\r\n");

    printRamMapperStatus();
    WaitKey();
    Cls();
    
    p = (unsigned char *) 0x8000;
    initialSegment = Get_PN(2);
    
    printf("Before settings segments  #%x \r\n",initialSegment);
    printf("[0x8000]=#%x \r\n",p[0]); 


    printf("Setting segment #%x at page 2 (from 0x8000 to 0xBFFF)\r\n",segmentId0);
    Put_PN(2,segmentId0);       
    printf("[0x8000]=#%x \r\n",p[0]); 
    p[0]=0xdd;
    printf("[0x8000]=#%x \r\n",p[0]); 
    
    
    printf("GET_PN = #%x \r\n",Get_PN(2));    
    
    WaitKey();
    Cls();
    
    printf("Setting segment #%x at page 2 (from 0x8000 to 0xBFFF)\r\n",segmentId1);
    Put_PN(2,segmentId1);
    printf("[0x8000]=#%x \r\n",p[0]); 
    p[0]=0xaa;
    printf("[0x8000]=#%x \r\n",p[0]); 


    printf("GET_PN = #%x \r\n",Get_PN(2));    

    WaitKey();
    Cls();
    
    printf("Setting segment #%x at page 2 (from 0x8000 to 0xBFFF)\r\n",segmentId0);
    Put_PN(2,segmentId0);       
    printf("[0x8000]=#%x \r\n",p[0]); 

    printf("GET_PN = #%x \r\n",Get_PN(2));

    printf("Setting segment #%x at page 2 (from 0x8000 to 0xBFFF)\r\n",segmentId1);
    Put_PN(2,segmentId1);       
    printf("[0x8000]=#%x \r\n",p[0]); 
    
    printf("GET_PN = #%x \r\n",Get_PN(2));    

    printf("Restoring original segment at page 2 (from 0x8000 to 0xBFFF)\r\n",initialSegment);
    Put_PN(2,initialSegment);       
    printf("[0x8000]=#%x \r\n",p[0]); 

    printf("GET_PN = #%x \r\n",Get_PN(2));        
    
    WaitKey();
    Cls();
    
    printf("\r\n\r\n***********Freeing block #%x\r\n",segmentId0);
    status = FreeSegment(segmentId0,0);
    printf("+++++++++++++++++++++++++++  Block freed, error=#%x \r\n",status->carryFlag); 
    printf("\r\n\r\n***********Freeing block #%x\r\n",segmentId1);
    status = FreeSegment(segmentId1,0);
    printf("+++++++++++++++++++++++++++  Block freed, error=#%x \r\n",status->carryFlag);
    printRamMapperStatus();


    


}

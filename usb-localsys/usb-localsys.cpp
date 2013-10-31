#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <memory.h>

#include "CyApi.h"

#define BULKS_PER_TIME 10

unsigned char data[512*BULKS_PER_TIME];
int ifcollect_data = 0;

unsigned char* localsys_bulk_transfer(CCyBulkEndPoint *BulkIn)
{
	unsigned char data_in[512];
	LONG bulksize = 512;
	//unsigned char buf2[2695680+192];
	if (!BulkIn -> XferData(data_in, bulksize))
	{
		printf("Time Out or Error in Bulk transfer!\n");
	}
	printf("%s\n", data_in);
	return data_in;
}


unsigned char localsys_transfer_start(CCyBulkEndPoint *BulkIn)
{
	LONG bulksize = 512;
	unsigned char data_collected[512];
	int i = 0;
	while(i<12)
	{
		memcpy(data_collected, localsys_bulk_transfer(BulkIn), bulksize);
		if(i>1) // reject first and secound bulks of data, true data is valid in and after the 3rd bulk.
		{
			memcpy(data + 512*(i-2), data_collected, bulksize); // 512*BULKS_PER_TIME data saved to data[]
		}
	}
	return data[512*BULKS_PER_TIME];
}

void main(void) {
	printf("%s\n", "Program starts.");
	
	//  Look for a device having VID = 05B4, PID = 1004 

    CCyUSBDevice *USBDevice = new CCyUSBDevice();   //  Create an instance of CCyUSBDevice 
    int devices = USBDevice->DeviceCount(); 
    int vID, pID; 
    int d = 0; 

    do 
	{ 
        USBDevice->Open(d);   // Open automatically  calls Close() if necessary 
        vID = USBDevice->VendorID; 
        pID  = USBDevice->ProductID;
        d++;             
    }  
	while ((d < devices ) && (vID != 0x04B4) &&  (pID != 0x1004)); 

	if (d > devices) 
	{
		printf("%s\n", "No device found.");
		exit(1);
	}
	else 
	{
		printf("%s\n", "Device found.");
	}
	
	// Control Endpoint

	/*CCyControlEndPoint *cept = USBDevice->ControlEndPt;     

	cept->Target    = TGT_DEVICE; 
	cept->ReqType   = REQ_VENDOR; 
	cept->Direction = DIR_TO_DEVICE;  
	cept->ReqCode   = 0xa2;   
	cept->Value     = 0;  
	cept->Index     = 0; 

	unsigned char  buf;
	LONG buflen = 0; 

	cept->XferData(&buf,  buflen); */    

	
	// Bulk endpoint 
	CCyBulkEndPoint *BulkIn = NULL; 
	int  eptCount = USBDevice->EndPointCount(); 
	/*
	for(int  i=1; i<eptCount;  i++) 
	{ 
		  bool bIn = USBDevice->EndPoints[i]->Address  & 0x80; 
		  bool bBulk = (USBDevice->EndPoints[i]->Attributes == 2); 

		  if (bBulk  && bIn) BulkIn = (CCyBulkEndPoint *) USBDevice->EndPoints[i];  // find bulk IN endpoint
	} 
	*/

	BulkIn = (CCyBulkEndPoint *) USBDevice->EndPoints[3];  // find EP6 (in firmwire EP6 is the only valid IN endpoint)

	// Transfer data

Transfer_loop:
	printf("Start collecting data? Y/N/E(xit)\n");
	char ifCollect_data = NULL;
	scanf("%c", &ifCollect_data);
	
	if(ifCollect_data == (char)'Y')
	{
		localsys_transfer_start(BulkIn);
		goto Transfer_loop;
	}
	else if(ifCollect_data == (char)'E') 
		{
			exit(0);
		}

	printf("%s\n", "Done");
}
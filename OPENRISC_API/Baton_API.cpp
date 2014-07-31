#include "Baton_API.h"
#include <unistd.h>

#define bufSize 1024
#define RAMSize 65536
#define NOW_USE_BOARD 0

int myHexToInt(char * cHexStr); //change .hex file data type

int size = bufSize;
WORD WriteBuffer [bufSize] ;
WORD ReadBuffer [bufSize] ;
//For USB driver using  
bool bRet;

int Baton_Init(char *SN, char *BitName){
	LoadShareObject();
	
	if ( !SMIMS_VEXT2_ProgramFPGA(NOW_USE_BOARD, BitName) )
        {
                printf( SMIMS_VEXT2_GetLastErrorMsg(NOW_USE_BOARD) );		
                return 1;
        }

	if ( !SMIMS_VEXT2_AppOpen(NOW_USE_BOARD, SN, 0) )
	{
			printf("SMIMS_VEXT2_AppOpen error.\n");
			printf( SMIMS_VEXT2_GetLastErrorMsg(NOW_USE_BOARD) );				
			return 1;
	}
	return 0;
}

int Baton_UART(void){
	WORD temp;
	WORD Ack;
	int sum = 0;

	while(1){
		if(SMIMS_VEXT2_WaitInterrupt(0, 1000)){				
			sum++;
			temp = 0;
			bRet = SMIMS_VEXT2_AppFIFOReadData(0, &temp, 1);
			if(!bRet){
				printf("%d : ",sum);
				printf("SMIMS_VEXT2_AppFIFOReadData failed!\n");
				printf(SMIMS_VEXT2_GetLastErrorMsg(NOW_USE_BOARD));
			}
			else{
				Ack = (temp%256) + 0xff00;					
				SMIMS_VEXT2_AppFIFOWriteData(0, &Ack, 1);
				if((temp/256) == 127){
					printf("%x",temp);
					printf("\n%d : End of core\n",sum);
					break;
				}
				else if((temp/256) == 128){
					printf("%c",(char)temp%256);
				}
				else{						
					printf("\nInvalid UART data : %04x\n",temp);
				}
			}
		}
		else printf("Time out!!\n");
	}
	printf("Core0 Finished\nSum : %d\n",sum);
	/*while(1){
		if(SMIMS_VEXT2_WaitInterrupt(0, 1000)){
			temp = 0;
			printf("GET INT\n");
			bRet = SMIMS_VEXT2_AppFIFOReadData(0, &temp, 1);
			printf("ReadData finished\n");
			Ack = (temp%256) + 0xff00;					
			SMIMS_VEXT2_AppFIFOWriteData(0, &Ack, 1);
			printf("DATA : %04x\n",temp);
		}
		else printf("Time out!!\n");
	}*/
	return 0;
}

int Baton_Close(void){
	bRet = SMIMS_VEXT2_AppClose(NOW_USE_BOARD);
	if ( !bRet )
	{
		printf("error: SMIMS_VEXT2_AppClose\n");
		printf(SMIMS_VEXT2_GetLastErrorMsg(NOW_USE_BOARD));
		return 1;
	}
	else
	{
		printf("SMIMS_VEXT2_AppClose OK!\n");
		return 0;
	}

}


/**
*	Download program 
*/
int Baton_Program(int coreID, char* fileName)
{
	FILE *fp = NULL;
	int data[RAMSize];
	int i,j,addr;

	int wr_data_size = RAMSize*4; //RAMSize*4 bytes
	char tmp[20];

	fp = fopen(fileName,"r");
	if(fp == NULL)
	{
		printf("open file error : <%s>\n",fileName);
		return -1;
	}
	
	i=0;
	while(fscanf(fp,"%s",&tmp[0]) != EOF ) //.hex to integer data
	{
		data[i] = myHexToInt(&tmp[0]);
		i++;
	}

	addr = 0;  // takecare !!
	while(1){
		if(wr_data_size > ((size-5-1)*2)){ 
		//	printf("Write-----------%d bytes-------\n",k*4+size);	
			//write data flag
			WriteBuffer[0] = 0xfff0; 
			WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 	//id 
			WriteBuffer[2] = 0xfff1;					//type write 
			WriteBuffer[3] = (WORD)(addr & 0xffff);			//start addr
			WriteBuffer[4] = (WORD)(((size-5-1)*2) & 0xffff);	//size ; 5 = flag ; 1 = unused space

			for(i=5; i<(size-1); i=i+2) {
		
				WriteBuffer[i] = (WORD) ( (data[addr] >> 16) & 0x0000ffff);
				WriteBuffer[i+1] =   (WORD) ( data[addr]     & 0x0000ffff);		
				addr++;
			}	
			/*
			* Write data to FPGA
			*/				
			bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, size-1);
			if ( !bRet )
			{
				printf("Write data to FPGA error\n");
				getchar();
			}		
			wr_data_size = wr_data_size - ((size-1-5)*2);
		}
		else{  			 //remained data
			//printf("remained data\n");
			//write data flag
			WriteBuffer[0] = 0xfff0; 
			WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 	//id 
			WriteBuffer[2] = 0xfff1;					//type write 
			WriteBuffer[3] = (WORD)(addr & 0xffff);			//start addr
			WriteBuffer[4] = (WORD)(wr_data_size & 0xffff);	//size 
			
			for(i=5; i<(wr_data_size+10)/2; i=i+2) {
				WriteBuffer[i] = (WORD) ( (data[addr] >> 16) & 0x0000ffff);
				WriteBuffer[i+1] =   (WORD) ( data[addr]     & 0x0000ffff);		
				addr++;
			}	

			/*
			* Write data to FPGA
			*/				
			bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, (wr_data_size+10)/2);
			if ( !bRet )
			{
				printf("Write data to FPGA error\n");
				getchar();
			}		
			wr_data_size = 0;
			break;
		}

	}	
	printf("===program  core %d OK===\n\n",coreID);
	return 0;
}
int Baton_Start(int coreID)
{		
	//write data flag
	WriteBuffer[0] = 0xfff0; 
	WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 	//id 
	WriteBuffer[2] = 0xfff3;				//type start

	bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, 3); //6 byte == 3 WORD
	if ( !bRet )
	{
		printf("Write data to FPGA error\n");
		getchar();
	}			
	printf("Start core %d\n\n",coreID);
	return 0;
}

// Baton_Send: Under constructing

/*int Baton_Send(int coreID, int dataSize, int startAddr,WORD *buf)
{
	int addr,i;
		
	addr = 0;  
	while(1){
		if(dataSize > (size-5-1)){ 
			//write data flag			
			WriteBuffer[0] = 0xfff0; 
			WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 	//id 
			WriteBuffer[2] = 0xfff1;					//type write 
			WriteBuffer[3] = (WORD)((startAddr + addr) & 0xffff);			//start addr
			WriteBuffer[4] = (WORD)(((size-5-1)*2) & 0xffff);	//size ; 5 = flag ; 1 = unused space
	
			for(i=5; i<(size-1); i=i+2) {
				WriteBuffer[i] = buf[addr*2];
				WriteBuffer[i+1] = buf[addr*2+1];
				addr++;
			}
			bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, size-1); //WORD = 2 bytes
			if ( !bRet )
			{
				printf("Write data to FPGA error\n");
				getchar();
			}			
			dataSize = dataSize - (size-5-1);
		}
		else{  			 //remained data	
			//write data flag
			WriteBuffer[0] = 0xfff0; 
			WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 			//id 
			WriteBuffer[2] = 0xfff1;					//type write 
			WriteBuffer[3] = (WORD)((startAddr + addr) & 0xffff);		//start addr
			WriteBuffer[4] = (WORD)((dataSize*2) & 0xffff);			//size
			
			for(i=5; i<dataSize+5; i=i+2) {		
				WriteBuffer[i] = buf[addr*2];
				WriteBuffer[i+1] = buf[addr*2+1];
		
				addr++;
			}		
			bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, dataSize+5); 
			if ( !bRet )
			{
				printf("Write data to FPGA error\n");
				getchar();
			}			
			dataSize = 0;
			break;
		}
	}
	printf("Baton_Send done\n\n");	
	return 0;
}*/

int Baton_Receive(int coreID, int dataSize, int startAddr,WORD *buf)  //Must ignore MPI header 32 bytes!! 
{
	int addr,i;
	
	addr = 0; //takecare !!	
	while(1){
		if(dataSize > size){
			//printf("Receieve from address : %d --- \n",startAddr + addr);
			//write data flag
			WriteBuffer[0] = 0xfff0; 
			WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 	//id 
			WriteBuffer[2] = 0xfff2;			//type read
			WriteBuffer[3] = (WORD)( (startAddr + addr) & 0x0000ffff); 	 //start addr 
			WriteBuffer[4] = (WORD)((size*2) & 0x0000ffff);	    		 //size 
			
			//Write to FPGA
			bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, 5); //WORD = 2 bytes
			if ( !bRet )
			{
				printf("Write data to FPGA error\n");
				getchar();
			}

			//if(SMIMS_VEXT2_WaitInterrupt(0, 1000)){
			bRet = SMIMS_VEXT2_AppFIFOReadData(0, ReadBuffer, size);
			//}
			//else{sleep(10);printf("456\n");}
			if ( !bRet )
			{
				printf("read from fifo error\n");
				getchar();
			}			
			
			for(i=0; i<size ;i=i+2){
				buf[i] = ReadBuffer[i]; 
				buf[i+1] = ReadBuffer[i+1];
				addr = addr + 1;
			}			
			dataSize = dataSize - size;
		}
		else{
			//printf("Receieve from address(remained) : %d --- \n",startAddr + addr);
			//write data flag
			WriteBuffer[0] = 0xfff0; 
			WriteBuffer[1] = (WORD)(coreID & 0x0000ffff); 	//id 
			WriteBuffer[2] = 0xfff2;			//type read
			WriteBuffer[3] = (WORD)( (startAddr + addr) & 0x0000ffff); 	 //start addr 
			WriteBuffer[4] = (WORD)((dataSize*2) & 0x0000ffff);	    		 //size 
			
			//Write to FPGA
			bRet = SMIMS_VEXT2_AppFIFOWriteData(0, WriteBuffer, 5); //WORD = 2 bytes
			if ( !bRet )
			{
				printf("Write data to FPGA error\n");
				getchar();
			}
			//if(SMIMS_VEXT2_WaitInterrupt(0, 1000)){
			//Reveive from FPGA
			bRet = SMIMS_VEXT2_AppFIFOReadData(0, ReadBuffer, dataSize);
			//}
			//else{sleep(10);printf("123\n");}
			if ( !bRet )
			{
				printf("read from fifo error\n");
				getchar();
				return false;
			}			
			
			for(i=0; i<dataSize ;i=i+2){
				buf[i] = ReadBuffer[i]; 
				buf[i+1] = ReadBuffer[i+1];  	
				addr = addr + 1;
			}			
			dataSize = 0;
			break;
			
		}
	}

//	printf("Baton_Receive done\n\n");
	return 0;
}

int myHexToInt(char * cHexStr)
{
	int Strlen = strlen(cHexStr);
        char cNowHexStr[128];
        int iValue = 0;
	int i;

        if ( Strlen <= 0 )
        	return -1;

        if ( strlen(cHexStr) > 2 )
        {
        	if ( cHexStr[0] == '0' && cHexStr[1] == 'x' ||
                     cHexStr[0] == '0' && cHexStr[1] == 'X' )
                {
                	strcpy(cNowHexStr, cHexStr + 2);
                        Strlen = strlen(cNowHexStr);	
                }
                else
                {
        		strcpy(cNowHexStr, cHexStr);
        		Strlen = strlen(cNowHexStr);
                }
        }
        else
        {
        	strcpy(cNowHexStr, cHexStr);
        	Strlen = strlen(cNowHexStr);
        }

        iValue = 0;
        for( i=0; i<Strlen; i++)
        {
                if ( i > 0 )
        		iValue *= 16;

                if ( cNowHexStr[ i ] >= 'A' && cNowHexStr[ i ] <= 'F')
                {
	                iValue += cNowHexStr[ i ] - 'A' + 10;
                }
                else
                if ( cNowHexStr[ i ] >= 'a' && cNowHexStr[ i ] <= 'f')
                {
                        iValue += cNowHexStr[ i ] - 'a' + 10;
                }
                else
                if ( cNowHexStr[ i ] >= '0' && cNowHexStr[ i ] <= '9')
                {
                        iValue += cNowHexStr[ i ] - '0';
                }
                else
                	return -1;
        }

        return iValue;
}

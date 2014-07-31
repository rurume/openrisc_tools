//#include"conio.h"
#include "Baton_API.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>

#define NOW_USE_BOARD 0
#define DEPTH 65536    //Depth of RAM

void *connection_handler(void *);
void generateAssemblyCode(char *, char *);
void generateHexFile(char *, char *, char *);
void process(char *, char *, char*, int);
void process(char *, char *, int);

int main(int argc, char* argv[])
{	

	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;
	
	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_desc == -1) {
		printf("Could not create socket.");
	}
	puts("Socket created.");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);
	
	//Bind
	if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Bind failed, ERROR.");
		return 1;
	}
	puts("Bind done.");
	
	//Listen
	listen(socket_desc, 3);
	
	while(1) {
		c = sizeof(struct sockaddr_in);
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if(client_sock < 0) {
			perror("Accept failed.");
			break;
		}
		
		pthread_t new_thread;
		
		if(pthread_create(&new_thread, NULL, connection_handler, (void*)&client_sock) < 0) {
			perror("Could not create thread.");
			return 1;
		}
		puts("handler assigned.");
	}
	
	return 0;
}

void *connection_handler(void *socket_desc) {
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char *client_message = (char*)malloc(sizeof(char) * 2000);

	int cnt = 0;
	char path[2000], filename[2000], deviceType[2000];

	memset(client_message, 0, 2000);
	while((read_size = recv(sock, client_message, 2000, 0)) > 0) {
		if(!strcmp(client_message, "GAC")) {
			write(sock, "ACK\0", 4);
			memset(client_message, 0, 2000);
			while((read_size = recv(sock, client_message, 2000, 0)) > 0) {
				if(cnt == 0) {
					strcpy(path, client_message);
					memset(client_message, 0, 2000);
					cnt++;
				} else if(cnt == 1) {
					strcpy(filename, client_message);
					memset(client_message, 0, 2000);
					cnt++;
				} else if(cnt == 2) {
					strcpy(deviceType, client_message);
					memset(client_message, 0, 2000);
					cnt = 0;
					break;
				}
			}
			
			generateAssemblyCode(path, filename);
			generateHexFile(path, filename, deviceType);
			
			write(sock, "ACK\0", 4);
		} else if(!strcmp(client_message, "GHF")) {
			write(sock, "ACK\0", 4);
			memset(client_message, 0, 2000);
			while((read_size = recv(sock, client_message, 2000, 0)) > 0) {
				if(cnt == 0) {
					strcpy(path, client_message);
					memset(client_message, 0, 2000);
					cnt++;
				} else if(cnt == 1) {
					strcpy(filename, client_message);
					memset(client_message, 0, 2000);
					cnt = 0;
					break;
				}
			}
			
			//memset(client_message, 0, 2000);
			process(path, filename, deviceType, sock);
			
			//write(sock, "ACK\0", 4);
			//write(sock, client_message, strlen(client_message));
		}
		memset(client_message, 0, 2000);
	}
	if(read_size == 0) {
		puts("Client disconnected.");
		fflush(stdout);
	} else if(read_size == -1) {
		perror("recv failed.");
	}
	
	//free(socket_desc);
	
	return 0;
}

void generateAssemblyCode(char *path, char *filename) {
	char fullPath[2000], cmd[2000];
	
	sprintf(fullPath, "%s/%s", path, filename);
	sprintf(cmd, "/opt/openrisc/toolchain/bin/or32-elf-gcc -S %s.c -o %s.S", fullPath, fullPath);
	system(cmd);
}

void generateHexFile(char *path, char *filename, char *deviceType) {
	char cwd[1024];
	char fullPath[2000], cmd[2000];
	
	getcwd(cwd, sizeof(cwd));
	sprintf(fullPath, "%s/%s", path, filename);	
	sprintf(cmd, "/opt/openrisc/toolchain/bin/or32-elf-gcc -fleading-underscore -g -c \
			-Wunknown-pragmas -mhard-mul -msoft-div -msoft-float %s.S -o %s.o", fullPath, fullPath);
	system(cmd);

	memset(cmd, 0, 2000);
	sprintf(cmd, "/opt/openrisc/toolchain/bin/or32-elf-ld -T %s/ram.ld %s/reset.o %s.o %s/uart.o -o %s.or32", 
			path, path, fullPath, path, fullPath);
	system(cmd);

	memset(cmd, 0, 2000);
	sprintf(cmd, "/opt/openrisc/toolchain/bin/or32-elf-objcopy -O ihex %s.or32 %s.ihex", fullPath, fullPath);
	system(cmd);

	memset(cmd, 0, 2000);
	sprintf(cmd, "%s/res/ihex2mif -f %s.ihex -o %s/ram0.mif", cwd, fullPath, path);
	system(cmd);

	memset(cmd, 0, 2000);
	sprintf(cmd, "%s/res/mif_gen %s/ram0.mif %s.hex", cwd, path, fullPath);
	system(cmd);

	char mode[] = "0777";
	int mode_int = strtol(mode, 0, 8);
	char output_file[2000];
	sprintf(output_file, "%s.hex", fullPath);
	chmod(output_file, mode_int);
}

void process(char *path, char *filename, char *deviceType, int sock) {
	if(strcmp(deviceType, "Or1ksim") == 0) {
		char cwd[1024];
		char fullPath[2000], cmd[2000];
	
		getcwd(cwd, sizeof(cwd));
		sprintf(fullPath, "%s/%s", path, filename);	
		memset(cmd, 0, 2000);
		sprintf(cmd, "/opt/or1ksim/bin/or32-elf-sim -f %s/device/Or1ksim/sim.cfg %s.or32", cwd, fullPath);
		system(cmd);
	} else {
		char input_file[2000], output_file[2000];
		sprintf(input_file, "%s/%s.hex", path, filename);
		sprintf(output_file, "%s/%s.out", path, filename);
	
		WORD sendBuf[DEPTH];
		WORD receiveBuf[DEPTH];
		int i,j;
		FILE *fp;
		int core_id = 0; // core ID : 0 ~ 3

		fp = fopen(output_file,"w");
	
		// initial buffers
		for(i = 0; i < DEPTH; i++) {
			sendBuf[i] = i;
			receiveBuf[i] = 99;
		}

		//************program circuit to FPGA*************
		char devicePath[100];
		sprintf(devicePath, "device/%s/top.bit", deviceType);
		Baton_Init("B4RM-G2OF-U0N0-9A0A-FF08-6C76", devicePath);
		Baton_Program(core_id, input_file);
		Baton_Start(core_id);  
		   
		// UART part
		unsigned short temp;
		unsigned short Ack;
		bool bRet;
		char *output_str = (char *)malloc(sizeof(char) * 2000);
		char *reply = (char *)malloc(sizeof(char) * 4);
		int cnt = 0;
	
		memset(output_str, 0, 2000);
		while(1) {
			if(SMIMS_VEXT2_WaitInterrupt(0, 1000)) {
				temp = 0;
				bRet = SMIMS_VEXT2_AppFIFOReadData(0, &temp, 1);
				if(!bRet) {
					//printf("%d : ",sum);
					printf("SMIMS_VEXT2_AppFIFOReadData failed!\n");
					printf(SMIMS_VEXT2_GetLastErrorMsg(NOW_USE_BOARD));
				}
				else{
					if((temp/256) == 127) {
						printf("\nEND\n");
						write(sock, "", 1);
						Ack = (temp%256) + 0xff00;					
						//printf("%x\n",Ack);
						SMIMS_VEXT2_AppFIFOWriteData(0, &Ack, 1);				
						break;
					}
					else if((temp/256) == 128) {
						output_str[cnt++] = (char)temp%256;
						if((char)temp%256 == 13) {
							output_str[cnt++] = '\0';
							while(1) {
								cnt = write(sock, output_str, cnt);
								read(sock, reply, sizeof(char) * 4);
								if(!strcmp(reply, "ACK")) break;
								memset(reply, 0, 4);
							}
							memset(output_str, 0, 2000);
							//usleep(20*1000);
							cnt = 0;
						}
						printf("%c",(char)temp%256);
						memset(reply, 0, 4);
						Ack = (temp%256) + 0xff00;
						//printf("%x\n",Ack);
						SMIMS_VEXT2_AppFIFOWriteData(0, &Ack, 1);
					}
					else {						
						printf("%04x\nInvalid UART data\n",temp);
					}
								
					//getchar();
				}
			}
			else printf("Time out!!\n");
		}
	
		for(j=0; j<DEPTH; j=j+512) {
			Baton_Receive(0, 1024, j, receiveBuf);	//receive 1024 * sizeof(WORD)
			for(i=0; i<1024; i=i+2) {
				if(receiveBuf[i] == 0) {
					if(receiveBuf[i+1] == 0) {
						fprintf(fp,"0\n");
					} else {
						fprintf(fp,"%x\n",receiveBuf[i+1]);
					}
				} else {
					fprintf(fp,"%x%04x\n",receiveBuf[i],receiveBuf[i+1]);
				}
			}		
		}
	
		fclose(fp);
		
		printf("########## END ###########\n");
		Baton_Close();
	
		char mode[] = "0777";
		int mode_int = strtol(mode, 0, 8);
		chmod(output_file, mode_int);
	}
	//************************************************
/*
	//test 4 cores operation
	Baton_Send(0, 4, 0, sendBuf);
	Baton_Send(0, 8, 1, sendBuf);
	Baton_Send(1, 4, 0, sendBuf);
	Baton_Send(1, 8, 1, sendBuf);
	Baton_Send(2, 4, 0, sendBuf);

	Baton_Send(2, 8, 1, sendBuf);
	Baton_Send(3, 4, 0, sendBuf);
	Baton_Send(3, 8, 1, sendBuf);


	Baton_Receive(0, 12, 0, receiveBuf);
	for(i=0;i<20;i++)
		printf("core0: %x\n",receiveBuf[i]);

	Baton_Receive(1, 12, 0, receiveBuf);
	for(i=0;i<20;i++)
		printf("core1: %x\n",receiveBuf[i]);
	
	Baton_Receive(2, 12, 0, receiveBuf);
	for(i=0;i<20;i++)
		printf("core2: %x\n",receiveBuf[i]);
	
	Baton_Receive(3, 12, 0, receiveBuf);
	for(i=0;i<20;i++)
		printf("core3: %x\n",receiveBuf[i]);

	Baton_Program(0 , "test.hex");
	Baton_Program(1 , "test.hex");
	Baton_Program(2 , "test.hex");
	Baton_Program(3 , "test.hex");
	///////////////////////////////////////////

	Baton_Program(core_id , "test.hex");
	Baton_Start(core_id);

    
	//read 32k
	for(j=0; j<8192; j=j+512)

	{
		Baton_Receive(core_id, 2048, j, receiveBuf);
		for(i=0; i<2048; i=i+4)

		{
			fprintf(fp,"%x %x  %x %x\n",receiveBuf[i+1],receiveBuf[i],receiveBuf[i+3],receiveBuf[i+2]);
		}
		
	}
	fclose(fp);

	//test0
	Baton_Send(core_id, 4, 0, sendBuf);
	Baton_Send(core_id, 8, 1, sendBuf);
	
	Baton_Receive(core_id, 12, 0, receiveBuf);
	for(i=0;i<20;i++)
		printf("%x\n",receiveBuf[i]);
		
	
	
	//test1
	for(j=0; j<8192; j=j+512)
	{
		Baton_Send(core_id, 2048, j, sendBuf);
		Baton_Receive(core_id, 2048, j, receiveBuf);
		for(i=0; i<2048; i=i+4)
		{

			if((sendBuf[i]!=receiveBuf[i]) || (sendBuf[i+1]!=receiveBuf[i+1]) || (sendBuf[i+2]!=receiveBuf[i+2]) || (sendBuf[i+3]!=receiveBuf[i+3]))
			{	

				printf("Error ! \n");
				break;
			}

		}
		
	}

	//test2 
	int send_size = 8*1024;
	for(j=0; j<8192; j=j+(send_size/4))
	{	
		Baton_Send(core_id, send_size, j, sendBuf);
	}
	
	int receieve_size = 8*1024;
	for(j=0; j<8192; j=j+ (receieve_size/4))
	{
		Baton_Receive(core_id, receieve_size, j, receiveBuf);
		for(i=0; i<receieve_size; i++)
		{
				if(sendBuf[i] != receiveBuf[i])
				{	
					printf("Error ! \n");
					break;
				}
		}
	}
	
	*/
	
	//return output_str;

	


	//getchar();
}


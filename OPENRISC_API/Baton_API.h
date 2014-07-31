#ifndef __Baton_API_H
#define __Baton_API_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SMIMS_VEXT2.h"

int Baton_Init(char*, char*);
int Baton_UART(void);
int Baton_Close(void);
int Baton_Program(int coreID, char* fileName);
//int Baton_Send(int coreID, int dataSize, int startAddr,WORD *buf);
int Baton_Receive(int coreID, int dataSize, int startAddr,WORD *buf);
int Baton_Start(int coreID);

#endif

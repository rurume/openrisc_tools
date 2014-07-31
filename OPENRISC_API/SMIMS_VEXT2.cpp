#include <dlfcn.h> 
#include "SMIMS_VEXT2.h"

void * handle;

DEF_SMIMS_VEXT2_ProgramFPGA SMIMS_VEXT2_ProgramFPGA;
DEF_SMIMS_VEXT2_ProgramFPGAFromSD SMIMS_VEXT2_ProgramFPGAFromSD;
DEF_SMIMS_VEXT2_AppOpen SMIMS_VEXT2_AppOpen;
DEF_SMIMS_VEXT2_AppFIFOReadData SMIMS_VEXT2_AppFIFOReadData;
DEF_SMIMS_VEXT2_AppFIFOWriteData SMIMS_VEXT2_AppFIFOWriteData;
DEF_SMIMS_VEXT2_AppChannelSelector SMIMS_VEXT2_AppChannelSelector;
DEF_SMIMS_VEXT2_AppClose SMIMS_VEXT2_AppClose;
DEF_SMIMS_VEXT2_GetLastErrorMsg SMIMS_VEXT2_GetLastErrorMsg;

DEF_SMIMS_VEXT2_Reset SMIMS_VEXT2_Reset;
DEF_SMIMS_VEXT2_WaitInterrupt SMIMS_VEXT2_WaitInterrupt;
DEF_SMIMS_VEXT2_NBReadData SMIMS_VEXT2_NBReadData;
DEF_SMIMS_VEXT2_NBWriteData SMIMS_VEXT2_NBWriteData;
DEF_SMIMS_VEXT2_WaitReadComplete SMIMS_VEXT2_WaitReadComplete;
DEF_SMIMS_VEXT2_WaitWriteComplete SMIMS_VEXT2_WaitWriteComplete;

bool LoadShareObject()
{
	handle = dlopen("./libsmimsvext2.so", RTLD_LAZY);

	if (handle == 0)
	{
		return false;
	}

	SMIMS_VEXT2_ProgramFPGA = (DEF_SMIMS_VEXT2_ProgramFPGA)dlsym(handle, "SMIMS_VEXT2_ProgramFPGA");
	SMIMS_VEXT2_AppOpen = (DEF_SMIMS_VEXT2_AppOpen)dlsym(handle, "SMIMS_VEXT2_AppOpen");
	SMIMS_VEXT2_AppFIFOReadData = (DEF_SMIMS_VEXT2_AppFIFOReadData)dlsym(handle, "SMIMS_VEXT2_AppFIFOReadData");
	SMIMS_VEXT2_WaitInterrupt = (DEF_SMIMS_VEXT2_WaitInterrupt)dlsym(handle, "SMIMS_VEXT2_WaitInterrupt");
	SMIMS_VEXT2_AppFIFOWriteData = (DEF_SMIMS_VEXT2_AppFIFOWriteData)dlsym(handle, "SMIMS_VEXT2_AppFIFOWriteData");
	SMIMS_VEXT2_AppChannelSelector = (DEF_SMIMS_VEXT2_AppChannelSelector)dlsym(handle, "SMIMS_VEXT2_AppChannelSelector");
	SMIMS_VEXT2_AppClose = (DEF_SMIMS_VEXT2_AppClose)dlsym(handle, "SMIMS_VEXT2_AppClose");
	SMIMS_VEXT2_GetLastErrorMsg = (DEF_SMIMS_VEXT2_GetLastErrorMsg)dlsym(handle, "SMIMS_VEXT2_GetLastErrorMsg");
	
	SMIMS_VEXT2_Reset = (DEF_SMIMS_VEXT2_Reset)dlsym(handle, "SMIMS_VEXT2_Reset");
	SMIMS_VEXT2_WaitInterrupt = (DEF_SMIMS_VEXT2_WaitInterrupt)dlsym(handle, "SMIMS_VEXT2_WaitInterrupt");
	SMIMS_VEXT2_NBReadData = (DEF_SMIMS_VEXT2_NBReadData)dlsym(handle, "SMIMS_VEXT2_NBReadData");
	SMIMS_VEXT2_NBWriteData = (DEF_SMIMS_VEXT2_NBWriteData)dlsym(handle, "SMIMS_VEXT2_NBWriteData");
	SMIMS_VEXT2_WaitReadComplete = (DEF_SMIMS_VEXT2_WaitReadComplete)dlsym(handle, "SMIMS_VEXT2_WaitReadComplete");
	SMIMS_VEXT2_WaitWriteComplete = (DEF_SMIMS_VEXT2_WaitWriteComplete)dlsym(handle, "SMIMS_VEXT2_WaitWriteComplete");

	return true;
}

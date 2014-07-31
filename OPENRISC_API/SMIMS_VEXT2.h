//
// VEXT2 Linux SDK Header File
//
// 2013.09.05 ikki Create

#include "SMIMS_define.h"


typedef BOOL (*DEF_SMIMS_VEXT2_ProgramFPGA)(int iBoard, char * BitFile);
typedef BOOL (*DEF_SMIMS_VEXT2_ProgramFPGAFromSD)(int iBoard, int iBitFileIndex);
typedef BOOL (*DEF_SMIMS_VEXT2_AppOpen)(int iBoard, char * SerialNO, int ClkMode);
typedef BOOL (*DEF_SMIMS_VEXT2_AppFIFOReadData)(int iBoard, WORD *Buffer, unsigned size);
typedef BOOL (*DEF_SMIMS_VEXT2_AppFIFOWriteData)(int iBoard, WORD *Buffer, unsigned size);
typedef BOOL (*DEF_SMIMS_VEXT2_AppChannelSelector)(int iBoard, BYTE channel);
typedef BOOL (*DEF_SMIMS_VEXT2_AppClose)(int iBoard);
typedef char *(*DEF_SMIMS_VEXT2_GetLastErrorMsg)(int iBoard);

typedef BOOL (*DEF_SMIMS_VEXT2_Reset)(int iBoard);
typedef BOOL (*DEF_SMIMS_VEXT2_WaitInterrupt)(int iBoard, unsigned TimeOut);
typedef BOOL (*DEF_SMIMS_VEXT2_NBReadData)(int iBoard, WORD *Buffer, unsigned size);
typedef BOOL (*DEF_SMIMS_VEXT2_NBWriteData)(int iBoard, WORD *Buffer, unsigned size);
typedef BOOL (*DEF_SMIMS_VEXT2_WaitReadComplete)(int iBoard, unsigned TimeOut);
typedef BOOL (*DEF_SMIMS_VEXT2_WaitWriteComplete)(int iBoard, unsigned TimeOut);

extern DEF_SMIMS_VEXT2_ProgramFPGA SMIMS_VEXT2_ProgramFPGA;
extern DEF_SMIMS_VEXT2_ProgramFPGAFromSD SMIMS_VEXT2_ProgramFPGAFromSD;
extern DEF_SMIMS_VEXT2_AppOpen SMIMS_VEXT2_AppOpen;
extern DEF_SMIMS_VEXT2_AppFIFOReadData SMIMS_VEXT2_AppFIFOReadData;
extern DEF_SMIMS_VEXT2_AppFIFOWriteData SMIMS_VEXT2_AppFIFOWriteData;
extern DEF_SMIMS_VEXT2_AppChannelSelector SMIMS_VEXT2_AppChannelSelector;
extern DEF_SMIMS_VEXT2_AppClose SMIMS_VEXT2_AppClose;
extern DEF_SMIMS_VEXT2_GetLastErrorMsg SMIMS_VEXT2_GetLastErrorMsg;

extern DEF_SMIMS_VEXT2_Reset SMIMS_VEXT2_Reset;
extern DEF_SMIMS_VEXT2_WaitInterrupt SMIMS_VEXT2_WaitInterrupt;
extern DEF_SMIMS_VEXT2_NBReadData SMIMS_VEXT2_NBReadData;
extern DEF_SMIMS_VEXT2_NBWriteData SMIMS_VEXT2_NBWriteData;
extern DEF_SMIMS_VEXT2_WaitReadComplete SMIMS_VEXT2_WaitReadComplete;
extern DEF_SMIMS_VEXT2_WaitWriteComplete SMIMS_VEXT2_WaitWriteComplete;

bool LoadShareObject();



#ifndef __DATA_H__
#define __DATA_H__

#include "types.h"

#define SECTOR_NUM 2880
#define SECTOR_SIZE 512 
#define CLUSER SECTOR_SIZE
#define NAME_LENGTH 64
#define FATEntry 12


#define BOOT_RECORD_SIZE SECTOR_SIZE
#define FAT_SIZE SECTOR_SIZE * 9
#define FAT_ENTRY_NUM FAT_SIZE / FAT_ENTRY_SIZE

#define UNKNOWN_TYPE 0
#define REGULAR_TYPE 1
#define DIRECTORY_TYPE 2
#define CHARACTER_TYPE 3
#define BLOCK_TYPE 4
#define FIFO_TYPE 5
#define SOCKET_TYPE 6
#define SYMBOLIC_TYPE 7


union BOOT_RECORD{

    uint8_t byte[BOOT_RECORD_SIZE];
    struct
    {   
        uint8_t BS_jmpBOOT[3];
        uint8_t BS_OEMName[8];
        uint16_t BPB_BytesPerSec;
        uint8_t BPB_SecPerClus;
        uint16_t BPB_ResvdSecCnt;
        uint8_t BPB_NumFATs;
        uint16_t BPB_RootEntCnt;
        uint16_t BPB_TotSec16;
        uint8_t BPB_Media;
        uint16_t BPB_FATSz16;
        uint16_t BPB_SecPerTrk;
        uint16_t BPB_NumHeads;
        uint32_t BPB_HiddSec;
        uint32_t BPB_TotSec32;
        uint8_t BS_DrvNum;
        uint8_t BS_Reserved1;
        uint8_t BS_BootSig;
        uint32_t BS_VolID;
        uint8_t BS_VolLab[11];
        uint8_t BS_FileSysType[8];
        uint8_t BS_CODE[448];
        uint16_t BS_MN;
    };
};

typedef union BOOT_RECORD BOOT_RECORD;

union FAT_ENTRY_TWICE{
    uint8_t byte[3];
};


typedef union FAT_ENTRY_TWICE FAT_ENTRY_TWICE;

typedef union FAT FAT;

#endif
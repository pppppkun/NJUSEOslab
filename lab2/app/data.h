#ifndef __DATA_H__
#define __DATA_H__
#pragma pack(1)
#include "types.h"

#define SECTOR_NUM 2880
#define SECTOR_SIZE 512 
#define CLUSER SECTOR_SIZE
#define NAME_LENGTH 64
#define FATEntry 12


#define BOOT_RECORD_SIZE SECTOR_SIZE
#define FAT_SIZE SECTOR_SIZE * 9
#define DIR_ENTRY_SIZE 32
#define FAT_ONE_OFFSET 512
#define DIR_SIZE 14 * SECTOR_SIZE
#define DIR_ENTRY_OFFSET 19 * SECTOR_SIZE
#define FAT_ENTRY_NUM FAT_SIZE / FAT_ENTRY_SIZE

#define UNKNOWN_TYPE 0
#define READ_ONLY 0x01
#define HIDDEN 0x02
#define SYSTEM 0x04
#define VOLUME_ID 0x08
#define DIRECTORY 0x10
#define ARCHIVE 0x20
#define LEN READ_ONLY | HIDDEN | SYSTEM | VOLUME_ID


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

union DIR_ENTRY
{
    uint8_t byte[DIR_ENTRY_SIZE];
    struct
    {
        uint8_t fileName[11];
        uint8_t type;
        uint8_t reserve[10];
        uint16_t WRT_TIME;
        uint16_t WRT_DATE;
        uint16_t FST_CLUS;
        uint32_t FILE_SIZE;
    };
    
};

typedef union DIR_ENTRY DIR_ENTRY;

typedef union FAT_ENTRY_TWICE FAT_ENTRY_TWICE;

union FAT
{
    uint8_t byte[FAT_SIZE];
};


typedef union FAT FAT;

#endif
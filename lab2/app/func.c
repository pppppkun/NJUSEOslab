#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"

int format (const char *driver){
    FILE *file = NULL;
    uint8_t byte[SECTOR_SIZE];
    if (driver == NULL) {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r");
    for(int i = 0;i<SECTOR_SIZE;i++){
        byte[i] = 0;
    }
    fread((void *)byte, sizeof(uint8_t), SECTOR_SIZE, file);
    BOOT_RECORD *boot_record = (BOOT_RECORD * )byte;
    uint8_t OEMName[8] = {0x6d, 0x6b, 0x66, 0x73, 0x2e, 0x66, 0x61, 0x74};
    for(int i = 0;i<8;i++){
        if(boot_record->BS_OEMName[i] != OEMName[i]){
            printf("File System is not FAT12. Please input other image");
            return -1;
        }
    }
    if(boot_record->BS_MN != 0xaa55){
        printf("File System is not FAT12. Please input other image");
        return -1;   
    }
    printf("Load Success!\n");
    // printf("%x\n", boot_record->BPB_RootEntCnt);
    return 0;
}

int ls (const char *driver, const char *destFilePath, const char *flag){
    
    FILE *file = NULL;
    uint8_t byte[DIR_ENTRY_SIZE];
    DIR_ENTRY * dir_entry;
    DIR_ENTRY dirs[DIR_SIZE/DIR_ENTRY_SIZE];
    if (driver == NULL) {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r+");
    for(int i = 0;i<DIR_ENTRY_SIZE;i++){
        byte[i] = 0;
    }
    //41 68 00 6f 00 75 00 73  00 65 00 0f 00 08 00 00 ff ff ff ff ff ff ff ff ff ff 00 00 ff ff ff ff
    //41 68 00 6f 00 75 00 73  00 65 00 0f 00 08 00 00 ff ff ff ff ff ff ff ff ff ff 00 00 ff ff ff ff   
    fseek(file, DIR_ENTRY_OFFSET, SEEK_SET);
    for(int i = 0;i<DIR_SIZE/DIR_ENTRY_SIZE;i++){
        fread((void *)byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
        // for(int i = 0;i<DIR_ENTRY_SIZE;i++){
        //     printf("%x ", byte[i]);
        // }
        // printf("\n");
        dir_entry = (DIR_ENTRY *) byte;
        printf("%s\n", dir_entry->fileName);
        if(dir_entry->type == 0){
            break;
        }
        // printf("%x\n", dir_entry->type);
    }
    return 0;
}
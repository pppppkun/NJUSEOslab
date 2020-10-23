#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"
//2 -> 32
//3 -> 33
int lsSubDir(FILE * file, DIR_ENTRY * dir_entry, FAT fat){
    uint16_t first_cluser = dir_entry->FST_CLUS;
    uint16_t ping = 0x0F;
    uint16_t pint_ = 0xF0;
    int index = 12 * (first_cluser-1) / 8;

    //TODO FIND THE LAST CLUSER
    // while(){

    // }

    DIR_ENTRY dirs[CLUSER/DIR_ENTRY_SIZE];
    fseek(file, 0, SEEK_SET);
    fseek(file,DATA_OFFSET+(first_cluser-2)*CLUSER, SEEK_SET);
    int i = 0;
    for(i = 0;i<CLUSER/DIR_ENTRY_SIZE;i++){
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    printf("%s\n", dir_entry->fileName);
    for(int j = 0;j<i;j++){
        dir_entry = &dirs[j];
        if(dir_entry->type == ARCHIVE || dir_entry->type == DIRECTORY){
            if(dir_entry->byte[0] == 0xe5){
                continue;
            }
            printf("%s ", dir_entry->fileName);
        }
    }
    printf("\n");
    for(int j = 0;j<i;j++){
        dir_entry = &dirs[j];
        if(dir_entry->type == DIRECTORY){
            if(dir_entry->byte[0] == 0xe5){
                continue;
            }
            if(dir_entry->byte[0] ==0x2e){
                continue;
            }
            lsSubDir(file, dir_entry, fat);
        }
    }
    // printf("%d ", index);
    // printf("%x ", fat.byte[index]);
    // printf("%x \n", ping & fat.byte[index+1]);
    return 0;
}

int readFAT(FILE * file, FAT * fat){
    fseek(file, SECTOR_SIZE, SEEK_SET);
    // fread((void *)fat->byte, sizeof(uint8_t), FAT_SIZE, file);
    FAT fat_;
    fread((void *)fat_.byte, sizeof(uint8_t), FAT_SIZE, file);
    *fat = fat_;
    fseek(file, FAT_SIZE, SEEK_SET);
    return 0;
}


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
    printf("BPB_SecPerClus: %x\n", boot_record->BPB_SecPerClus);
    printf("BPB_BytesPerSec: %x\n", boot_record->BPB_BytesPerSec);
    printf("BPB_RootEntCnt: %x\n", boot_record->BPB_RootEntCnt);
    printf("BPB_RootEntCnt: %x\n", boot_record->BPB_FATSz16);
    // printf("%x\n", boot_record->BPB_RootEntCnt);
    fclose(file);
    return 0;
}

int ls (const char *driver, const char *destFilePath, const char *flag){
    
    FILE *file = NULL;
    DIR_ENTRY * dir_entry;
    FAT fat;
    DIR_ENTRY dirs[DIR_SIZE/DIR_ENTRY_SIZE];
    int i = 0;
    if (driver == NULL) {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r");
    uint8_t byte[DIR_ENTRY_SIZE];
    for(int i = 0;i<DIR_ENTRY_SIZE;i++){
        byte[i] = 0;
    }
    
    readFAT(file, &fat);

    for(i = 0;i<DIR_SIZE/DIR_ENTRY_SIZE;i++){
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    printf("/\n");
    for(int j = 0;j<i;j++){
        dir_entry = &dirs[j];
        if(dir_entry->type == ARCHIVE || dir_entry->type == DIRECTORY){
            if(dir_entry->byte[0] == 0xe5){
                continue;
            }
            printf("%s ", dir_entry->fileName);
        }
    }

    printf("\n");
    for(int j = 0;j<i;j++){
        dir_entry = &dirs[j];
        if(dir_entry->type == DIRECTORY){
            if(dir_entry->byte[0] == 0xe5){
                continue;
            }
            lsSubDir(file, dir_entry, fat);
        }
    }


    fclose(file);
    return 0;
}

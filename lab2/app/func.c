#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"
int printDirEntry(DIR_ENTRY *dirs, const int size)
{
    DIR_ENTRY *dir_entry;
    for (int i = 0; i < size; i++)
    {
        dir_entry = &dirs[i];
        if (dir_entry->type == ARCHIVE || dir_entry->type == DIRECTORY)
        {
            if (dir_entry->byte[0] == 0xe5)
            {
                continue;
            }
            for (int z = 0; z < 11; z++)
            {
                if (dir_entry->fileName[z] == ' ')
                {
                    continue;
                }
                if (dir_entry->fileName[z - 1] == ' ' && dir_entry->fileName[z] != ' ')
                {
                    printf(".");
                }
                printf("%c", dir_entry->fileName[z]);
            }
            printf(" ");
        }
    }
}

int printDirEntriesWithAdditions(FILE *file, DIR_ENTRY *dirs, const int size, FAT fat, int *additions)
{
    DIR_ENTRY *dir_entry;
    for (int j = 0; j < size; j++)
    {
        dir_entry = &dirs[j];
        if (dir_entry->type == ARCHIVE || dir_entry->type == DIRECTORY)
        {
            if (dir_entry->byte[0] == 0xe5 || dir_entry->byte[0] == 0x2e)
            {
                continue;
            }
            additions[0] = 0;
            additions[1] = 0;
            additions[2] = 0;
            countSub(file, dir_entry, additions, fat);
            for (int z = 0; z < 11; z++)
            {
                if (dir_entry->fileName[z] == ' ')
                {
                    continue;
                }
                if (dir_entry->fileName[z - 1] == ' ' && dir_entry->fileName[z] != ' ')
                {
                    printf(".");
                }
                printf("%c", dir_entry->fileName[z]);
            }
            if (dir_entry->type == DIRECTORY)
            {
                printf(" %d %d\n", additions[0], additions[1]);
            }
            if (dir_entry->type == ARCHIVE)
            {
                printf(" %d\n", additions[2]);
            }
        }
    }
}

int readFile(FILE * file, FAT fat, DIR_ENTRY * dir_entry, int flag, int *file_size, uint8_t *byte)
{
    uint16_t first_cluser = dir_entry->FST_CLUS;
    fseek(file, 0, SEEK_SET);
    fseek(file, DATA_OFFSET + (first_cluser - 2) * CLUSER, SEEK_SET);
    fread((void *)byte, sizeof(uint8_t), CLUSER, file);
    uint16_t next_cluser = 0;
    uint8_t ping = 0x0F;
    uint8_t ping_ = 0xF0;
    uint32_t index = 3 * (first_cluser / 2);
    uint16_t a = fat.byte[index];
    uint16_t b = (fat.byte[index + 1] & ping) << 8;
    uint16_t c = (fat.byte[index + 1] & ping_) >> 4;
    uint16_t d = fat.byte[index + 2] << 4;
    if (first_cluser % 2 == 0)
    {
        next_cluser = a + b;
    }
    else
    {
        next_cluser = c + d;
    }
    int size = 1;
    if (flag==1)
    {
        for (int i = 0; i < CLUSER; i++)
        {
            printf("%c", byte[i]);
        }
    }
    while (next_cluser < 0xFF8)
    {
        size++;
        fread((void *)byte, sizeof(uint8_t), CLUSER, file);
        //printf("%x\n", next_cluser);
        if (flag==1)
        {
            for (int i = 0; i < CLUSER; i++)
            {
                printf("%c", byte[i]);
            }
        }
        index = 3 * (next_cluser / 2);
        a = fat.byte[index];
        b = (fat.byte[index + 1] & ping) << 8;
        c = (fat.byte[index + 1] & ping_) >> 4;
        d = fat.byte[index + 2] << 4;
        if (next_cluser % 2 == 0)
        {
            next_cluser = a + b;
        }
        else
        {
            next_cluser = c + d;
        }
    }
    *file_size = size;
}

int countRoot(DIR_ENTRY *dirs, const int i, int *additions)
{
    DIR_ENTRY *dir_entry;
    for (int j = 0; j < i; j++)
    {
        dir_entry = &dirs[j];
        if (dir_entry->byte[0] == 0xe5 || dir_entry->byte[0] == 0x2e)
        {
            continue;
        }
        if (dir_entry->type == DIRECTORY)
        {
            additions[0]++;
        }
        else if (dir_entry->type == ARCHIVE)
        {
            additions[1]++;
        }
    }
}

int countSub(FILE *file, const DIR_ENTRY *dir_entry, int *additions, FAT fat)
{
    uint16_t first_cluser = dir_entry->FST_CLUS;
    uint8_t ping = 0x0F;
    uint8_t ping_ = 0xF0;
    uint32_t index = 3 * (first_cluser / 2);
    if (dir_entry->type == DIRECTORY)
    {
        DIR_ENTRY dirs[CLUSER / DIR_ENTRY_SIZE];
        fseek(file, 0, SEEK_SET);
        fseek(file, DATA_OFFSET + (first_cluser - 2) * CLUSER, SEEK_SET);
        for (int i = 0; i < CLUSER / DIR_ENTRY_SIZE; i++)
        {
            fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
        }
        countRoot(dirs, CLUSER / DIR_ENTRY_SIZE, additions);
    }
    else if (dir_entry->type == ARCHIVE)
    {
        // block f;
        int size = 0;
        uint8_t byte[CLUSER];
        readFile(file, fat, dir_entry, 0, &size, byte);
        // printf("%d\n", size);
        for (int i = 0; i < CLUSER; i++)
        {
            if (byte[i] == 0x0a)
            {
                size = (size - 1) * 512 + i + 1;
                break;
            }
        }
        // printf("last cluser %x %d\n", next_cluser, size);
        additions[2] = size;
    }
}

int lsSubDir(FILE *file, DIR_ENTRY *this_dir_entry, FAT fat, char *father_path)
{
    uint16_t first_cluser = this_dir_entry->FST_CLUS;
    DIR_ENTRY *dir_entry;
    int additions[2];
    int index = 12 * (first_cluser - 1) / 8;
    DIR_ENTRY dirs[CLUSER / DIR_ENTRY_SIZE];
    fseek(file, 0, SEEK_SET);
    fseek(file, DATA_OFFSET + (first_cluser - 2) * CLUSER, SEEK_SET);
    for (int i = 0; i < CLUSER / DIR_ENTRY_SIZE; i++)
    {
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    int size = 0;
    stringChr(this_dir_entry->fileName, ' ', &size);
    this_dir_entry->fileName[size] = '\0';
    size = stringLen(father_path);
    char print_father_path[size + 1];
    stringCpy(father_path, print_father_path, size);
    print_father_path[size] = '/';
    print_father_path[size + 1] = '\0';
    if (father_path[0] == '/')
        print_father_path[size] = '\0';
    else
        printf("/");
    // deal_father_path(this_dir_entry, father_path, print_father_path);

    printf("%s%s/\n", print_father_path, this_dir_entry->fileName);

    printDirEntry(dirs, CLUSER / DIR_ENTRY_SIZE);

    printf("\n");
    size = stringLen(print_father_path);
    for (int i = 0; i < CLUSER / DIR_ENTRY_SIZE; i++)
    {
        dir_entry = &dirs[i];
        if (dir_entry->type == DIRECTORY)
        {
            if (dir_entry->byte[0] == 0xe5)
            {
                continue;
            }
            if (dir_entry->byte[0] == 0x2e)
            {
                continue;
            }
            int size2 = stringLen(this_dir_entry->fileName);
            char new_father_path[size + size2];
            for (int i = 0; i < size; i++)
                new_father_path[i] = print_father_path[i];
            for (int i = size; i < size + size2; i++)
            {
                new_father_path[i] = this_dir_entry->fileName[i];
                if (this_dir_entry->fileName[i] == ' ')
                    new_father_path[i] = '\0';
            }
            lsSubDir(file, dir_entry, fat, this_dir_entry);
        }
    }
    return 0;
}

int readFAT(FILE *file, FAT *fat)
{
    fseek(file, SECTOR_SIZE, SEEK_SET);
    // fread((void *)fat->byte, sizeof(uint8_t), FAT_SIZE, file);
    FAT fat_;
    fread((void *)fat_.byte, sizeof(uint8_t), FAT_SIZE, file);
    *fat = fat_;
    fseek(file, FAT_SIZE, SEEK_SET);
    return 0;
}

int format(const char *driver)
{
    FILE *file = NULL;
    uint8_t byte[SECTOR_SIZE];
    if (driver == NULL)
    {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r");
    if (file == NULL) {
        printf("Failed to open image.\n");
        return -1;
    }
    for (int i = 0; i < SECTOR_SIZE; i++)
    {
        byte[i] = 0;
    }
    fread((void *)byte, sizeof(uint8_t), SECTOR_SIZE, file);
    BOOT_RECORD *boot_record = (BOOT_RECORD *)byte;
    uint8_t OEMName[8] = {0x6d, 0x6b, 0x66, 0x73, 0x2e, 0x66, 0x61, 0x74};
    for (int i = 0; i < 8; i++)
    {
        if (boot_record->BS_OEMName[i] != OEMName[i])
        {
            printf("File System is not FAT12. Please input other image\n");
            return -1;
        }
    }
    if (boot_record->BS_MN != 0xaa55)
    {
        printf("File System is not FAT12. Please input other image\n");
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

int ls(const char *driver)
{

    FILE *file = NULL;
    DIR_ENTRY *dir_entry;
    FAT fat;
    DIR_ENTRY dirs[DIR_SIZE / DIR_ENTRY_SIZE];
    int additions[2];
    int i = 0;
    if (driver == NULL)
    {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r");
    uint8_t byte[DIR_ENTRY_SIZE];
    for (int i = 0; i < DIR_ENTRY_SIZE; i++)
    {
        byte[i] = 0;
    }
    readFAT(file, &fat);
    for (i = 0; i < DIR_SIZE / DIR_ENTRY_SIZE; i++)
    {
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    printf("/:\n");
    printDirEntry(dirs, DIR_SIZE / DIR_ENTRY_SIZE);
    printf("\n");
    for (int j = 0; j < i; j++)
    {
        dir_entry = &dirs[j];
        if (dir_entry->type == DIRECTORY)
        {
            if (dir_entry->byte[0] == 0xe5)
            {
                continue;
            }
            lsSubDir(file, dir_entry, fat, "/");
        }
    }

    fclose(file);
    return 0;
}

int lsSubDirHelper(const char *father_path,const char *this_path,const char *target_path){
    int size = stringLen(father_path);
    int size2 = stringLen(this_path);
    char new_father_path[size + size2+1];
    for (int i = 0; i < size; i++) new_father_path[i] = father_path[i];
    for (int i = size; i < size + size2; i++) new_father_path[i] = this_path[i-size];
    if(stringCmp(target_path, new_father_path, stringLen(target_path))==0) return 0;
    else if(stringCmp(new_father_path, target_path, stringLen(new_father_path)) == 0) return 1;
    return -1;

}

int lsSubDirAddition(FILE *file, DIR_ENTRY *this_dir_entry, FAT fat, char *father_path, int flag, char *FilePath)
{
    uint16_t first_cluser = this_dir_entry->FST_CLUS;
    uint16_t ping = 0x0F;
    uint16_t pint_ = 0xF0;
    DIR_ENTRY *dir_entry;
    int additions[3];
    additions[0] = 0;
    additions[1] = 0;
    additions[2] = 0;
    DIR_ENTRY dirs[CLUSER / DIR_ENTRY_SIZE];
    fseek(file, 0, SEEK_SET);
    fseek(file, DATA_OFFSET + (first_cluser - 2) * CLUSER, SEEK_SET);
    int i = 0;
    for (i = 0; i < CLUSER / DIR_ENTRY_SIZE; i++)
    {
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    int size = 0;

    stringChr(this_dir_entry->fileName, ' ', &size);
    this_dir_entry->fileName[size] = '\0';
    countRoot(dirs, i, additions);
    size = stringLen(father_path);
    char print_father_path[size + 1];
    stringCpy(father_path, print_father_path, size);
    print_father_path[size] = '/';
    print_father_path[size + 1] = '\0';
    if (father_path[0] == '/')
        print_father_path[size] = '\0';
    int FilePathSize = stringLen(FilePath);
    if(FilePath == NULL || lsSubDirHelper(print_father_path, this_dir_entry->fileName, FilePath)==0)
    {
        if (father_path[0] == '/')
            print_father_path[size] = '\0';
        else
            printf("/");
        printf("%s%s/ %d %d \n", print_father_path, this_dir_entry->fileName, additions[0], additions[1]);
        printDirEntriesWithAdditions(file, dirs, CLUSER / DIR_ENTRY_SIZE, fat, additions);
        printf("\n");
    }
    else if(lsSubDirHelper(print_father_path, this_dir_entry->fileName, FilePath)==1){
        
    }
    else{
        return 0;
    }
    size = stringLen(print_father_path);
    for (int j = 0; j < i; j++)
    {
        dir_entry = &dirs[j];
        if (dir_entry->type == DIRECTORY)
        {
            if (dir_entry->byte[0] == 0xe5)
            {
                continue;
            }
            if (dir_entry->byte[0] == 0x2e)
            {
                continue;
            }
            int size2 = stringLen(this_dir_entry->fileName);
            char new_father_path[size + size2+1];
            for (int i = 0; i < size; i++)
                new_father_path[i] = print_father_path[i];
            for (int i = size; i < size + size2; i++)
            {
                new_father_path[i] = this_dir_entry->fileName[i-size];
                if (this_dir_entry->fileName[i] == ' ')
                    new_father_path[i] = '\0';
            }
            new_father_path[size+size2] = '/';
            lsSubDirAddition(file, dir_entry, fat, new_father_path, 0, FilePath);
        }
    }
    return 0;
}

int ls_addition(const char *driver, const char *FilePath)
{
    FILE *file = NULL;
    DIR_ENTRY *dir_entry;
    FAT fat;
    DIR_ENTRY dirs[DIR_SIZE / DIR_ENTRY_SIZE];
    int additions[3];additions[0] = 0;additions[1]=0;additions[2]=0;
    if (driver == NULL)
    {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r");
    uint8_t byte[DIR_ENTRY_SIZE];
    for (int i = 0; i < DIR_ENTRY_SIZE; i++)
    {
        byte[i] = 0;
    }

    readFAT(file, &fat);

    for (int i = 0; i < DIR_SIZE / DIR_ENTRY_SIZE; i++)
    {
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    countRoot(dirs, DIR_SIZE / DIR_ENTRY_SIZE, additions);
    if (FilePath == NULL)
    {
        printf("/ %d %d :\n", additions[0], additions[1]);
        printDirEntriesWithAdditions(file, dirs, DIR_SIZE / DIR_ENTRY_SIZE, fat, additions);
    }else{
        int FilePathSize = stringLen(FilePath);
        if(stringCmp(FilePath, "/", 1) == 0 && FilePathSize == 1){
            printf("/ %d %d :\n", additions[0], additions[1]);
            printDirEntriesWithAdditions(file, dirs, DIR_SIZE / DIR_ENTRY_SIZE, fat, additions);
        }
    }


    
    printf("\n");
    for (int i = 0; i < DIR_SIZE / DIR_ENTRY_SIZE; i++)
    {
        dir_entry = &dirs[i];
        if (dir_entry->type == DIRECTORY)
        {
            if (dir_entry->byte[0] == 0xe5)
            {
                continue;
            }
            if (dir_entry->byte[0] == 0x2e)
            {
                continue;
            }
            lsSubDirAddition(file, dir_entry, fat, "/", 0, FilePath);
        }
    }
    fclose(file);
    return 0;
}

int cat(const char *driver, const char *destFilePath)
{
    int ret = 0;
    int count = 0;
    int cond = 0;
    int flag = 0;
    FILE *file = NULL;
    DIR_ENTRY *dir_entry;
    FAT fat;
    DIR_ENTRY dirs[DIR_SIZE / DIR_ENTRY_SIZE];
    int i = 0;
    if (driver == NULL)
    {
        printf("FS == NULL\n");
        return -1;
    }
    file = fopen(driver, "r");
    uint8_t byte[DIR_ENTRY_SIZE];
    for (int i = 0; i < DIR_ENTRY_SIZE; i++)
    {
        byte[i] = 0;
    }
    readFAT(file, &fat);
    for (i = 0; i < DIR_SIZE / DIR_ENTRY_SIZE; i++)
    {
        fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
    }
    int size = 0;
    ret = stringChr(destFilePath, '/', &size);
    if (ret == -1 || size != 0)
        return -1;
    count += 1;
    while (destFilePath[count] != 0)
    {
        //printf("%s\n", destFilePath+count);
        ret = stringChr(destFilePath + count, '/', &size);
        if (cond == 1)
        {
            break;
        }
        if (ret == 0 && size == 0)
            return -1;
        if (ret == -1) // no '/'
            cond = 1;
        for (int j = 0; j < i; j++)
        {
            dir_entry = &dirs[j];
            if (dir_entry->type == ARCHIVE || dir_entry->type == DIRECTORY)
            {
                if (dir_entry->byte[0] == 0xe5)
                {
                    continue;
                }
                if (stringCmpFileName(dir_entry->fileName, destFilePath + count, size) == 0)
                {
                    if (cond == 0)
                    {
                        int first_cluser = dir_entry->FST_CLUS;
                        fseek(file, 0, SEEK_SET);
                        fseek(file, DATA_OFFSET + (first_cluser - 2) * CLUSER, SEEK_SET);
                        for (i = 0; i < CLUSER / DIR_ENTRY_SIZE; i++)
                        {
                            fread((void *)dirs[i].byte, sizeof(uint8_t), DIR_ENTRY_SIZE, file);
                        }
                        break;
                    }
                    else
                    {
                        uint8_t byte[CLUSER];
                        int size = 0;
                        readFile(file, fat, dir_entry, 1, &size, byte);
                        flag = 1;
                    }
                }
            }
        }
        if (cond == 0)
        {
            count += size + 1;
        }
    }
    if (flag == 0)
    {
        printf("invalid path! please input another path or enter ls\n");
        printf("???\n");
    }
    fclose(file);
    return 0;
}

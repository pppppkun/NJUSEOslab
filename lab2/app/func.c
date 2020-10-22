#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"



int format (const char *driver){
    int i = 0;
    int ret = 0;
    FILE *file = NULL;
    uint8_t byte[SECTOR_SIZE];
    if (driver == NULL) {
        printf("driver == NULL\n");
        return -1;
    }
    file = fopen(driver, "w+");
    for(int i = 0;i<SECTOR_SIZE;i++){
        byte[i] = 0;
    }

    fread((void *)byte, sizeof(uint8_t), SECTOR_SIZE, file);

    for(int i = 0;i<SECTOR_SIZE;i++){
        printf("%x", byte[i]);
    }
    printf("\n");
    return 0;
}
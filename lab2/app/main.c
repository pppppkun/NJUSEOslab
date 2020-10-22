#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"

int main(){	
	int fileSize = 0;
	char filename[100];
	scanf("%s", filename);
	for(int i = 0;i<100;i++){
		if(filename[i]=='\0'){
			fileSize = i;
			break;
		}
	}
	printf("%s\n", filename);
	format(filename);
}

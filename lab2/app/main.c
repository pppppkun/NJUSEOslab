#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"
char exec[2000];
void deal_ls(){
	//scanf("%s",exec);
	int size = stringLen(exec);
	int flag = 0;

	int ret = stringChrR(exec, ' ', &size);

	

	if(flag==1){
		ls_addition("../os.img", NULL);
	}
	else{
		ls("../os.img");
	}
}

void deal_cat(){
	int size=0;
	int ret = stringChrR(exec, ' ', &size);
	if(ret == -1 || size + 1 == stringLen(exec)){
		printf("error! please input a path\n");
		return;
	}
	//printf("%s\n", exec);
	if(cat("../os.img", exec+size+1)==-1){
        printf("invalid path! please input another path or enter ls\n");
	}
}


int main(){	
	int fileSize = 0;
	char filename[100];
	// scanf("%s", filename);
	format("../os.img");
	while(gets(exec)){
		int i = 0;
		i = stringCmp("ls", exec, 2);
		if(i == 0){
			deal_ls(exec);
			continue;
		}
		i = stringCmp("cat", exec, 3);
		if(i==0){
			deal_cat();
			continue;
		}
		i = stringCmp("exit", exec, 4);
		if(i==0){
			//printf("exit\n");
			return 0;
		}
		else{
			printf("error operation!\n");
		}
	}
}

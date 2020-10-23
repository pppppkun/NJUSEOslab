#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"

void deal_ls(char * exec){
	int size = stringLen(exec);
	int flag = 0;
	for(int i = 1;i<size-1;i++){
		if (exec[i-1]=='-'&&exec[i]=='l'&&exec[i+1]==' ')
		{
			flag = 1;
			break;
		}
		else if(i>=2 && exec[i-2]==' '&&exec[i-1]=='-'&&exec[i]=='l'&&exec[i+1]=='\0'){
			flag = 1;
			break;
		}
	}
	if(flag==1){
		ls_addition("../os.img", NULL);
	}
	else{
		ls("../os.img");
	}
}


int main(){	
	int fileSize = 0;
	char filename[100];
	// scanf("%s", filename);
	format("../os.img");
	char exec[2000];
	while(gets(exec)){
		int i = 0;
		i = stringCmp("ls", exec, 2);
		if(i == 0){
			deal_ls(exec);
			continue;
		}
		i = stringCmp("cat", exec, 3);
		if(i==0){
			printf("cat\n");
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

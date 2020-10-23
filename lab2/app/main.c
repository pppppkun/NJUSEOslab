#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"

void deal_ls(char * exec){

	int size = stringLen(exec);
	int flag = 0;
	for(int i = 1;i<size;i++){
		if (exec[i-1]=='-'&&exec[i]=='l')
		{
			flag = 1;
			break;
		}
	}
}


int main(){	
	int fileSize = 0;
	char filename[100];
	// scanf("%s", filename);
	format("../os.img");
	char exec[2000];
	while(scanf("%s",exec)){
		int i = 0;
		i = stringCmp("ls", exec, 2);
		if(i == 0){
			deal_ls(exec);
		}
		i = stringCmp("cat", exec, 3);
		if(i==0){
			printf("cat\n");
		}
		i = stringCmp("exit", exec, 4);
		if(i==0){
			printf("exit\n");
			return 0;
		}
	}
}

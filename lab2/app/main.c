#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"
char exec[2000];
char filename[100];
char exec_opr[100][100];
int exec_size = 0;
/**
 * ls
/ 2 2 :
HOUSE 0 2
NJU 1 1
���_�~1 3091
INDEX.TXT 1009

/HOUSE/ 0 2 
ROOM 18
NIUNIU.TXT 2315

/NJU/ 1 1 
ABOUT.TXT 16
SOFTWARE 0 2

/NJU/SOFTWARE/ 0 2 
SE1.TXT 22
SE2.TXT 12*/
void deal_ls(){
	//scanf("%s",exec);
	int index = -1;
	int flag = 0;
	for(int i = 1;i<exec_size;i++){
		if(exec_opr[i][0]=='-'){
			for(int j = 1;j<stringLen(exec_opr[i]);j++){
				if(exec_opr[i][j]!='l'){
					printf("error operation! please input correct instruction\n");
					return;
				}
			}
			flag = 1;
		}
		else if(exec_opr[i][0]=='/'){
			index = i;
		}
		else{
			printf("error operation! please input correct instruction\n");
			return;
		}
	}
	if(flag == 0 && index != -1){
		printf("please set '-l' when you input file index!\n");
		return;
	}
	if(flag == 0)
		ls(filename);
	if(flag == 1){
		if(index == -1)
			ls_addition(filename, NULL);
		else 
			ls_addition(filename, exec_opr[index]);
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
	int ret = 1;
	do{
		scanf("%s", filename);
		if(stringCmp(filename, "exit", 4)==0){
			return 0;
		}
		ret = format(filename);
	}while(ret == -1);
	while(gets(exec)){
		int i = 0;
		int flag = 0; // space
		int start = 0;
		exec_size = 0;
		for(int i = 0;i<stringLen(exec);i++){
			if(exec[i]==' '){
				if(flag == 0){
					stringCpy(exec+start, exec_opr[exec_size++], i - start);
					flag = 1;
				}
				else{
					continue;
				}
			}else{
				if(flag == 1){
					start = i;
				}
				flag = 0;
			}
		}
		stringCpy(exec+start, exec_opr[exec_size++], stringLen(exec) - start);
		i = stringCmp("ls", exec_opr[0], 2);
		if(i == 0){
			deal_ls();
			continue;
		}
		i = stringCmp("cat", exec_opr[0], 3);
		if(i==0){
			deal_cat();
			continue;
		}
		i = stringCmp("exit", exec_opr[0], 4);
		if(i==0){
			//printf("exit\n");
			return 0;
		}
		else{
			printf("error operation! please input correct instruction\n");
		}
	}
}

#include <stdio.h>
#include "utils.h"
#include "data.h"
#include "func.h"

char exec[2000];
char filename[100];
char exec_opr[100][100];
int exec_size = 0;
void deal_ls(){
	int index = -1;
	int flag = 0;
	for(int i = 1;i<exec_size;i++){
		if(exec_opr[i][0]=='-'){
			for(int j = 1;j<stringLen(exec_opr[i]);j++){
				if(exec_opr[i][j]!='l'){
					agent("error operation! please input correct instruction\n");
					return;
				}
			}
			flag = 1;
		}
		else if(exec_opr[i][0]=='/'){
			index = i;
		}
		else{
			agent("error operation! please input correct instruction\n");
			return;
		}
	}
	if(flag == 0 && index != -1){
		agent("please set '-l' when you input file index!\n");
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
		agent("error! please input a path\n");
		return;
	}
	//agent("%s\n", exec);
	if(cat(filename, exec+size+1)==-1){
        agent("invalid path! please input another path or enter ls\n");
	}
}


int main(){	
	int fileSize = 0;
	int ret = 1;
	agent("\033[31m");
	agent("aksdjkalsd ");
	agent("askdjaskldjaskdjkalsd");
	agent("\033[0m");
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
			//agent("exit\n");
			return 0;
		}
		else{
			agent("error operation! please input correct instruction\n");
		}
	}
}

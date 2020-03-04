#include "helper.h"
#include <stdlib.h>

int main(int argc,char* argv[]){
	if(argc != 3){
		printf("参数数量有误\n");
		return 1;
	}
	
	int x = atoi(argv[1]);
	int y = atoi(argv[2]);
	printf("%d+%d=%d,%d-%d=%d\n",x,y,add(x,y),x,y,sub(x,y));
	return 0;
}

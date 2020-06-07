#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>

#define T_DEVPATH "/dev/scull"
#define H_DEVPATH "/dev/myscull"
#define SAVE_PATH "/mnt/scullfs/env.txt"
#define BUFSIZE 20

char* template = "20";
char* humidity = "30";

int simulater(char* message,char* path, char* buf){
	int fd;
	
    if (access(path,F_OK)) {
        printf("%s not exist\n",path);
        return -1;
    }

	fd=open(path, O_RDWR);
	if(write(fd, message, strlen(message)) < 0){
		printf("write failed (%s)\n",strerror(errno));
		close(fd);
		return -1;
	}
	lseek(fd, 0, 0);
	if(read(fd, buf, BUFSIZE) < 0){
		printf("read failed (%s)\n",strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
	
}

int main(){
	char t_buf[BUFSIZE] = {0};
	char h_buf[BUFSIZE] = {0};
	if(simulater(template, T_DEVPATH,t_buf) == 0){
		printk("read template OK\n");
	}else return -1;
	
	if(simulater(humidity, H_DEVPATH,h_buf) == 0){
		printk("read humidity OK\n");
		
	}else return -1;
	
	int fd = fopen(SAVE_PATH, "w");
	fprintf(fd,"%s %s", t_buf, h_buf);
	fclose(fd);
	return 0;
}
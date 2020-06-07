ifneq ($(KERNELRELEASE),)  #第二次从这里运行 
#modemo-objs := file1.o file2.o file3.o  #如果有多个文件  
	obj-m += scull.o  #生成modemo.ko 
else   #第一次从这里运行        
	KERNELDIR := /home/xulei/teamwork/teamwork5/linux  
	PWD := $(shell pwd)   #模块程序所在的目录，在该目录运行make 
all:  
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
clean:             
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*
endif 



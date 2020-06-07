#### upuzu

#### 徐磊 李亚男 丁帅

## 一、实验目的

1. Socket通信
2. 多进程、多线程编程
3. 交叉调试目标端程序
4. 磁盘分区与文件系统创建
5. 模块与驱动编程
## 二、实验内容

1. 将树莓派设为智能家居Linux服务器，可用来采集并维护环境数据，如PM2.5、温度、湿度、气味、电器状态数据等。在实际环境中数据来自相应的传感器，本次试验中用scull设备模拟。
2. 要求创建2个以上scull设备，设备驱动从内核源码树外(Kbuild)编译安装或加入到内核源码树内(需重新编译内核)。驱动要求包括：open、release、read、write、llseek、ioctl。
3. 实验中的环境数据存储在特定文件系统中。该文件系统要求具备属性：在线写入、持久性、断电可靠性。
4. PC机、移动设备或另外一个树莓派用作远程客户端，随时请求获取环境数据，客户端和服务器之间采用Socket通信。
5. APP编译采用交叉编译，用gdb-gdbserver交叉调试APP。
## 三、实验流程

### 3.1SCULL

1. 内核的准备

使用第五次作业时的内核即可。在makefile中指定kdir，同时使用交叉编译。

```makefile
ifneq ($(KERNELRELEASE),)  #第二次从这里运行 
#modemo-objs := file1.o file2.o file3.o  #如果有多个文件  
	obj-m += scull.o  #生成modemo.ko 
else   #第一次从这里运行        
	KERNELDIR := /home/xulei/teamwork/teamwork5/linux  
	PWD := $(shell pwd)   #模块程序所在的目录，在该目录运行make 
all:  
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-
clean:             
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*
endif 
```
1. scull设备

Ubuntu下测试：

![图片](https://uploader.shimo.im/f/0bDVKva8WIx8mSAL.png!thumbnail)
![图片](https://uploader.shimo.im/f/ZPsM1wVAWsl4b6Vu.png!thumbnail)

树莓派下测试：

![图片](https://uploader.shimo.im/f/JRwfJA01YYlHy0fj.png!thumbnail)

主设备号为240

![图片](https://uploader.shimo.im/f/MrmYcXiqEhZpSEch.png!thumbnail)

创建设备文件

![图片](https://uploader.shimo.im/f/JyYllHW61ZV0STwg.png!thumbnail)

尝试读写设备文件

![图片](https://uploader.shimo.im/f/p5A0kJ4XkARxjRWK.png!thumbnail)

可以看到，在root权限下成功将内容写入到设备文件中，并读取出来。

### 3.2文件系统

查看当前文件系统：

```plain
sudo fdisk -l
```
![图片](https://uploader.shimo.im/f/5xpL0b612VDPMvrs.png!thumbnail)

使用fdisk增加分区：

```plain
sudo fdisk /dev/mmcblk0
```
新建拓展分区：
```plain
n add a new partition
```
选择e，默认
![图片](https://uploader.shimo.im/f/KVWvw5jg09emuHUS.png!thumbnail)

由于逻辑分区需要建立在拓展分区之上，所以必须先建立拓展分区，然后在拓展分区上建立逻辑分区。

![图片](https://uploader.shimo.im/f/d8JZj7HBk0rVaA7Q.png!thumbnail)

接下来新建逻辑分区

![图片](https://uploader.shimo.im/f/SekKHtGVxy5V8bj9.png!thumbnail)

可以看到，确实满足之前所说，逻辑分区的终点和拓展分区相同，起点在拓展分区内，即逻辑分区是拓展分区的子集。

格式化之后，挂载到/mnt/scullfs下，作为本次作业的数据存储位置

![图片](https://uploader.shimo.im/f/vzXp6wdq7b7mvrDS.png!thumbnail)

![图片](https://uploader.shimo.im/f/I2EK7EFHmAssBdt2.png!thumbnail)

### 3.3 Socket通信

#### 3.3.1Socket介绍

**概念**

我们会利用三元组“ip地址，端口号，协议”进行网络间的进程通信，而socket就是利用这三元组解决网络通信的一个中间件工具。

     也就是说，Socket是应用层与TCP/IP协议族通信的中间软件抽象层，是一组接口。它把复杂的TCP/IP协议族隐藏在Socket接口后面，对使用者来说，一组简单的接口就是全部，让Socket去组织数据，以符合指定的协议。

**两种传输方式**

Socket有两种数据传输方式：

1）SOCK_STREAM：表示面向连接的数据传输方式。数据可以准确无误地到达另一台计算机，如果损坏或丢失，可以重新发送，但效率相对较慢。

2）SOCK_DGRAM：表示无连接的数据传输方式。计算机只管传输数据，不作数据校验，如果数据在传输中损坏，或者没有到达另一台计算机，没有办法补救。效率比 SOCK_STREAM 高。

这里的第一种传输方式对应了传输层的TCP协议，第二种对应了UDP协议。两种协议的区别如下表所示：

![图片](https://uploader.shimo.im/f/cBamUY4mmu1EREcy.png!thumbnail)

我们可以根据自己的场景要求选择合适的传输方式。

#### 3.3.2 实现原理

根据实验要求：使用树莓派作为服务端，客户端请求数据（温湿度）时，服务端向客户端发送数据，客户端接收数据。**为了保证数据的可靠性**、不丢失，我们采用TCP传输方式。

TCP中客户端与服务端的连接过程如下：

![图片](https://uploader.shimo.im/f/7f8mWP8W3nkvwFql.png!thumbnail)

其中在建立连接阶段，即客户端调用connect()函数后，客户端和服务端会有三次握手：

![图片](https://uploader.shimo.im/f/9fmRt2dGyHPYyKSk.png!thumbnail)

#### 3.3.3 客户端：

1、建立socket，确定传输方式为：SOCK_STREAM

2、使用connect与服务器建立连接

3、发送请求数据

4、打印接收消息

5、关闭连接，sleep5分钟后再次建立连接。

每5分钟向服务器连接一次，请求最新数据。

```python
import time
from socket import *
serverName = '127.0.0.1'
serverPort = 6789
while(True):
    try:
        clientSocket = socket(AF_INET, SOCK_STREAM)
        clientSocket.connect((serverName, serverPort))
        outputdata = 'temperature'
        clientSocket.send(outputdata.encode())
        data = 1
        data = clientSocket.recv(1024)
        print(data.decode(), end='')
        print('\r')
    finally:
        clientSocket.close()
        print("五分钟刷新中")
        time.sleep(5 * 60)
```
#### 3.3.4 服务端

1、建立Socket，绑定ip地址与端口号

2、使用listen()监听

3、使用accept()接收客户端连接

4、根据客户端请求发送特定消息

```python
# import socket module
from socket import *
import json
import time
serverSocket = socket(AF_INET, SOCK_STREAM)
# Prepare a sever socket
serverSocket.bind(('localhost', 6789))  # 将TCP欢迎套接字绑定到指定端口
serverSocket.listen(1)  # 最大连接数为1
return_data = {'data': '', 'temperature': '0.0', 'humidity': '0.0'}
while True:
    # Establish the connection
    print('Ready to serve...')
    connectionSocket, addr = serverSocket.accept()  # 接收到客户连接请求后，建立新的TCP连接套接字
    try:
        message = connectionSocket.recv(1024).decode()  # 获取客户发送的报文
        print(message)
        raw_data = ""
        if message == 'temperature':
            f = open('./cache.txt')
            raw_data = f.readline()
            f.close()
            local_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            return_data['data'] = local_time
            return_data['temperature'] = raw_data.split()[0]
            return_data['humidity'] = raw_data.split()[1]
            connectionSocket.send(json.dumps(return_data).encode())
            connectionSocket.close()
        else:
            msg = 'no commend'
            connectionSocket.send(msg.encode())
            # Close client socket
            connectionSocket.close()
    except IOError:
        # Send response message for file not found
        header = ' HTTP/1.1 404 Found'
        connectionSocket.send(header.encode())
    finally:
        # Close client socket
        connectionSocket.close()
```
#### 3.3.5 结果

![图片](https://uploader.shimo.im/f/8sZ6g9j3QKQPgoUY.png!thumbnail)![图片](https://uploader.shimo.im/f/25ctiioWjdiJSWfH.png!thumbnail)

#### 3.3.6 总结

在这一部分实验中，熟悉了Socket通信的方式，并知道了TCP与UDP两种传输的区别；另外，使用TCP传输完成了客户端与服务端的通信。

在实验过程中，完成客户端5分钟进行一次请求，会有下述错误：

```python
OSError: [WinError 10038] 在一个非套接字上尝试了一个操作。
```
查阅资料后，明白这是由于客户端在每次连接后会释放连接，在下次连接中，需要新建一个Socket在进行连接，解决了问题。
### 3.4 集成

首先，创建三个设备文件。实际只用到了两个。

![图片](https://uploader.shimo.im/f/4fELokN8BpFFp9Rt.png!thumbnail)

将数据从设备文件中读出，并写到/mnt/scullfs/env.txt

```c++
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#define T_DEVPATH "/dev/scull1"
#define H_DEVPATH "/dev/scull2"
#define SAVE_PATH "/mnt/scullfs/env.txt"
#define BUFSIZE 20
char* template = "20";
char* humidity = "30";
//将数据写入设备文件中，并读到buffer中
int simulater(char* message,char* path, char* buf){
	int fd;
    if (access(path,F_OK)) {
        printf("%s not exist\n",path);
        return -1;
    }
	if((fd=open(path, O_RDWR))<0){
		printf("open file failed(%s)\n",strerror(errno));
		return -1;
	}
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
    //获取温度数据
	if(simulater(template, T_DEVPATH,t_buf) == 0){
		printf("read template OK\n");
	}else return -1;
	//获取湿度数据
	if(simulater(humidity, H_DEVPATH,h_buf) == 0){
		printf("read humidity OK\n");
		
	}else return -1;
	//写入/mnt/scullfs/env.txt
	int fd = fopen(SAVE_PATH, "w");
	fprintf(fd,"%s %s", t_buf, h_buf);
	fclose(fd);
	return 0;
}
```
请求数据：
![图片](https://uploader.shimo.im/f/XTSZ7rANJTpePSfW.png!thumbnail)

发送数据：

![图片](https://uploader.shimo.im/f/CgprhoI2QeqRv69C.png!thumbnail)





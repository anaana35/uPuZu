# Teamwork1

**uPuZu 丁帅 李亚男 徐磊(按姓氏首字母排序)**

## 实验目的

配置Linux环境，安装相应工具并掌握使用方法

## 实验内容

1. [安装Linux环境](#安装Linux环境)
2. [编写C文件](#编写C文件)
3. [编写Makefile](#编写Makefile)
4. [通过autotools生成Makefile](#通过autotools生成Makefile)
5. [使用git提交作业](#使用git提交作业)

## 实验内容与结果

- ### <a name="安装Linux环境" >安装Linux环境</a>

  1、下载安装VMware，进行相关配置；

  <img src="./pic/21.png" alt="VMware" style="zoom:75%;" />

  2、在VMware上安装操作系统（这里以Ubuntu为例），进行有关配置，安装完成。

  <img src='./pic/22.png' alt="Ubuntu桌面">

  3、打开终端，进行简单命令：

  <img src='./pic/23.png' alt="Ubuntu桌面">

- ### <a name="编写C源文件" >编写C源文件</a>

  - main.c

    ```c
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
    ```

  - helper.c

    ```c
    #include "helper.h"
    
    int add(int x,int y){
        return x+y;
    }
    
    int sub(int x,int y){
        return x-y;
    }	
    ```

  - helper.h

    ```c
    #ifndef _HELPER_H
    #define _HELPER_H
    #include <stdio.h>
    
    int add(int x,int y);
    int sub(int x,int y);
    
    #endif
    ```

  - 操作截图

    ![图片引用错误](./pic/2.png)

    ![图片引用错误](./pic/3.png)

- ### <a name="编写Makefile" >编写Makefile</a>

  - Makefile

    ```makefile
    #My makefile
    TARGETS = program 
    OBJS = main.o helper.o
    HDRS = helper.h 
    CFLAGS = -g 
    CC = gcc 
    
    #-c表示只编译不链接
    program: $(OBJS)
    	$(CC) $(CFLAGS) $(OBJS) -o program
    main.o: main.c helper.h
    	$(CC) $(CFLAGS) -c $< -o $@
    helper.o: helper.c helper.h
    	gcc -g -c helper.c -o helper.o
    	
    .PHONY: clean 
    clean:  
    	rm -f $(TARGETS) $(OBJS) 
    ```

  - 操作流程

    ![图片引用错误](./pic/4.png)

    ![图片引用错误](./pic/5.png)

  - 运行结果

    > ![图片引用错误](./pic/16.png)

    

- ### <a name="通过autotools生成Makefile" >通过autotools生成Makefile</a>

  - autoscan及configure.ac重命名

    > ![图片引用错误](./pic/7.png)

  - configure.ac

    ```
    #                                               -*- Autoconf -*-
    # Process this file with autoconf to produce a configure script.
    
    AC_PREREQ([2.69])
    AC_INIT(program,1.0)
    AM_INIT_AUTOMAKE(program,1.0)
    AC_CONFIG_SRCDIR([main.c])
    AC_CONFIG_HEADERS([config.h])
    
    # Checks for programs.
    AC_PROG_CC
    
    # Checks for libraries.
    
    # Checks for header files.
    AC_CHECK_HEADERS([stdlib.h])
    
    # Checks for typedefs, structures, and compiler characteristics.
    
    # Checks for library functions.
    AC_CONFIG_FILES([Makefile]) 
    AC_OUTPUT
    ```

  - aclocal, autoconf, autoheader

    > ![](./pic/8.png)

  - 创建Makefile.am

    ```
    AUTOMAKE_OPTIONS = foreign 
     
    bin_PROGRAMS = program 
    program_SOURCES = main.c helper.c
    ```

    > ![](./pic/10.png)

  - automake --add-missing以及创建相应的error文件

    > ![](./pic/11.png)
    >
    > ![](./pic/13.png)

  - ./configure

    > ![](./pic/14.png)

  - make指令测试

    > ![](./pic/15.png)

    > ![](./pic/6.png)
    >
    > ![](./pic/17.png)
    >
    > ![](./pic/18.png)

- ### <a name="使用git提交作业" >使用git提交作业</a>

  1、fork小组项目到自己的github

  2、使用git clone命令下载到本地，补充修改作业

  ![下载作业到本地](./pic/24.png)

  3、补充修改完成后，使用如下命令

  git add  . :                              将文件夹下文件添加进来

  git commit -m "3.8" ：        提交信息

  git push -u origin master    输入账号密码后将更新后的作业上传至github中。

  ![作业上传](./pic/25.png)

  ![上传作业](./pic/26.png)

## 实验总结

​	   在这次实验中，配置了linux环境并学会使用了linux的一些命令，包括文件浏览、操作等，对后续的课程实验打下基础。另外，在linux系统下安装了make、autotools管理工具，熟悉了makefile的编写及使用这些管理工具管理项目的基本流程，感受到使用管理工具的便捷性，最后，安装了git工具，学会了使用git克隆、推送的命令，这可以帮助我们管理项目版本及文件变更。在实验过程中不可避免会遇到一些问题，经过网上查阅、讨论得以解决。

​	

## 实验源代码

code文件夹下存放
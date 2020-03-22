# Teamwork2

**uPuZu 丁帅 李亚男 徐磊(按姓氏首字母排序)**

## 1、fork()与vfork()

### fork函数代码

* （1）fork函数能够创建子进程

***代码***：

<img src="./pic/issue1/1.png">



***结果***:

<img src="./pic/issue1/2.png">

***分析***：

​        代码中的第一个fork函数是父进程创建了一个子进程，父子进程打印输出，第二个fork函数使父进程和子进程都创建了一个子进程并打印；并且***子进程继承了父进程的stdout文件，可以输出指向屏幕，父子进程***。

​		因此使用fork函数创建进程有***共享文件***的特点。

* （2）wait+fork函数

***代码***

<img src="./pic/issue1/3.png">

***结果***

<img src="./pic/issue1/4.png">

***分析***

① 使用wait函数可以使父进程阻塞到子进程执行结束后退出。

② fork函数创建进程的***”相同的但是独立的地址空间“***特点。可见父进程和子进程都是独立的进程，具有自己的私有地址空间，父进程对变量x的修改不会反映在子进程中。

* （3）fork+wait+execve函数

***代码***

<img src ="./pic/issue1/5.png">

***结果***

<img src="./pic/issue1/6.png">

***分析***

在父进程执行打印后，wait函数会阻塞父进程直到子进程执行完代码。

在子进程中会执行execlp函数，并且看到执行完execlp函数后没有再返回，***说明exec不返回到调用进程***。

### vfork函数

### 代码

<img src="./pic/issue1/7.png">

### 结果

<img src="./pic/issue1/8.png">



### 分析

​		使用vfork函数创建进程，***子进程先于父进程运行***，不需要在父进程中使用wait阻塞。

​		vfork***使用复制指针的方法使父子进程内存共享***，因此看到子进程中对x的操作会影响父进程对x的操作，因为x仅此一份。

### 比较

​		经过上述实验，可以看到fork与vfork都是创建进程，但有以下不同：

​	1、***父子进程执行顺序***。fork的父子进程，谁先执行不确定，可以使用wait阻塞；而vfork的父子进程，确定子进程执行完毕后父进程执行。

​	2、***地址空间***。fork的父子进程相互独立，子进程完全复制父进程的资源；而vfork并没有把父进程的映像全部复制到子进程中，父子进程用复制指针的方法内存共享。

## 2. signal实验

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

void handler(int sig);
void handler2(int sig);

int main() 
{
    int i;
    pid_t pid;
	signal(SIGCHLD, handler2); /* install SIGCHLD handler*/
	printf("Hello from parent %d\n",getpid());

    /* Parent creates children */
    for (i = 0; i < 3; i++) {
        pid = fork();
        if (pid == 0) {
			signal(SIGALRM, handler); /* install SIGALRM handler */
			alarm(1); /* Next SIGALRM will be delivered in 1s */
            printf("Hello from child %d\n", (int)getpid());
            pause();
        }
    }

  	while(1){
		char c;
		if((c=getchar())=='q')
			break;
	}
    exit(0);
}

void handler(int sig) 
{
    printf("BEEP from %d\n",getpid());
	printf("BOOM!\n");
	exit(0);
}

void handler2(int sig) 
{
    pid_t pid;
  
    while ((pid = waitpid(-1, NULL, 0)) > 0)
		printf("Handler reaped child %d\n", (int)pid);
    sleep(1);
    return;
}
```

- 运行结果


```
xulei@xulei-virtual-machine:~/文档$ ./test
Hello from parent 44044
Hello from child 44045
Hello from child 44046
Hello from child 44047
BEEP from 44045
BOOM!
BEEP from 44046
BOOM!
Handler reaped child 44045
BEEP from 44047
BOOM!
Handler reaped child 44046
Handler reaped child 44047
q
```

- question


```
BEEP from 43812
BEEP from 43813
BOOM!
BOOM!
```

​		有时输出会存在上述情况。可能的分析：printf在写入字符中有换行符时会刷新缓冲区，从而发生系统调用，返回是发生调度。
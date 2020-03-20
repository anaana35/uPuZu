# Teamwork2

**uPuZu 丁帅 李亚男 徐磊(按姓氏首字母排序)**

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
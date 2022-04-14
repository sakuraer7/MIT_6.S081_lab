#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void get_prime(int fd);
int main(int argc, char *argv[]) {
    int fd[2];
    if(pipe(fd) < 0) {
        printf("init pipe error\n");
        exit(1);
    }
    char buf[2];
    int pid;
    pid = fork();
    if(pid < 0) {
        printf("fork error\n");
        exit(1);
    }
    // 核心思想是父进程负责写入数据，子进程负责筛选素数
    // 这样可以充分利用pipe实现并发筛选
    if(pid == 0) {
        close(fd[0]);
        for(int i=2; i<=35; ++i) {
            buf[0] = (char)i;
            if(write(fd[1], buf, 1) != 1) {
                printf("write error\n");
                exit(1);
            }
        }
        // 0为标志位，读取到0即为结束
        buf[0] = (char)0;
        if(write(fd[1], buf, 1) != 1) {
            printf("write 0 error\n");
            exit(1);
        }
        wait((int *)0);
    } else {
        close(fd[1]);
        //子进程调用筛选函数
        get_prime(fd[0]);
    }
    exit(0);
}
void get_prime(int fd) {
    char buf[2];
    if(read(fd, buf, 1) < 1) {
        exit(1);
    }
    if((int)buf[0] == 0) {
        exit(0);
    }
    int base = (int)buf[0];
    printf("prime %d\n", base);
    int fds[2];
    if(pipe(fds) < 0) {
        printf("pipe error\n");
        exit(1);
    }
    int pid = fork();
    if(pid < 0) {
        printf("fork error\n");
        exit(1);
    }
    // 子进程筛选之后，化身为父进程，创建pipe写入筛选后的数据，子进程再次进行后续的筛选
    if(pid == 0) {
        close(fds[0]);
        while(read(fd, buf, 1) != 0) {
            char temp[2];
            temp[0] = buf[0];
            int n = (int)buf[0];
            if(n == 0 || n % base != 0) {
                write(fds[1], temp, 1);
            }
        }
        wait((int *)0);
    } else {
        close(fds[1]);
        get_prime(fds[0]);
    }
}
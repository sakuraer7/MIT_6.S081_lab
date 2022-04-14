#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd1[2];
    int fd2[2];
    if(pipe(fd1) < 0) {
        printf("pipe init error\n");
        exit(1);
    }
    if(pipe(fd2) < 0) {
        printf("pipe init error\n");
        exit(1);
    }
    int pid = fork();
    if(pid < 0) {
        printf("fork error\n");
        exit(1);
    }
    if(pid == 0) {
        char buf[16];
        close(fd2[1]);
        close(fd1[0]);
        int n = read(fd2[0], buf, 1);
        if(n != 1) {
            printf("child read pipe error\n");
            exit(1);
        } else {
            printf("%d: received ping\n", getpid());
            write(fd1[1], "a", 1);
        }
    } else {
        char buf[16];
        close(fd2[0]);
        close(fd1[1]);
        if(write(fd2[1], "a", 1) != 1) {
            printf("parent write pipe error");
            exit(1);
        }
        int n = read(fd1[0], buf, 1);
        if(n != 1) {
            printf("child read error\n");
            exit(1);
        } else {
            wait((int *)0);
            printf("%d: received pong\n", getpid());    
        }
    }
    exit(0);
}
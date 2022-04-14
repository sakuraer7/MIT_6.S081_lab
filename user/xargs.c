#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("xargs: need at least 2 param\n");
        exit(1);
    }
    char ch;
    // args是一个指针数组，里面只有指针，没有字符串空间
    char *args[MAXARG];
    int s = 0;
    // xargs后面命令的参数
    for(int i=1; i<argc; ++i) {
        args[s++] = argv[i];
    }
    // 从标准输入端读取数据，存入buf里
    char buf[512];
    int idx = 0;
    char *p;
    // p是当前字符串的首地址,下面是例子：
    //      p    p     p
    // buf: grep|b.txt|hello
    p = buf;
    // 从标准输入读取命令
    while(read(0, &ch, 1) > 0) {
        // 读取到换行即结束，开始fork子进程执行
        if(ch == '\n') {
            buf[idx] = 0;
            args[s++] = p;
            args[s] = 0;
            int pid = fork();
            if(pid < 0) {
                exit(1);
            }
            if(pid > 0){
                wait((int *)0);
                // 执行完，将这些地址复位，进行下一个命令的执行
                idx = 0;
                p = buf;
                s = argc - 1;
            } else {
                exec(args[0], args);
                printf("exec error\n");
                exit(1);
            }
        } else if(ch == ' ') { //如果遇到空格，则一个参数读取结束，字符串尾部置0并且存入到args。然后进行下一个命令的读取，将p指向下一命令的字符串首地址
            buf[idx++] = 0;
            args[s++] = p;
            p = buf + idx;
        } else {
            buf[idx++] = ch;
        }
    }
    exit(0);
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  int p[2];

  if (pipe(p) < 0) { // 创建管道
    printf("pipe creation failed\n");
    exit(1);
  }

  int pid = fork(); // 创建子进程

  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) { // 子进程
    close(p[0]); // 关闭读取端

    int byte = 'A';
    printf("%d: received ping\n", getpid()); // 打印接收到的消息
    write(p[1], &byte, sizeof(byte)); // 写入管道

    close(p[1]); // 关闭写入端
    exit(0);
  } else { // 父进程
    close(p[1]); // 关闭写入端

    int byte;
    read(p[0], &byte, sizeof(byte)); // 从管道读取数据
    printf("%d: received pong\n", getpid()); // 打印接收到的消息

    close(p[0]); // 关闭读取端
    exit(0);
  }
}
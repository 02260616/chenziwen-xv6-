#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

// 去除字符串后面的空格
char*
rtrim(char* path)
{
    static char newStr[DIRSIZ+1];
    int whiteSpaceSize = 0;
    int bufSize = 0;
    for(char* p = path + strlen(path) - 1; p >= path && *p == ' '; --p) {
        ++whiteSpaceSize;
    }
    bufSize = DIRSIZ - whiteSpaceSize;
    memmove(newStr, path, bufSize);
    newStr[bufSize] = '\0';
    return newStr;
}

void
find(char* path, char* name)
{
    char buf[512], *p;
    int fd;
     // 目录描述符
    struct dirent de;
    // 文件描述符
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) == -1) {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return;
    }
    
    switch (st.type) {
        
        case T_DEVICE:
        case T_FILE:
            fprintf(2, "find: %s not a path value.\n", path);
            close(fd);
            
            break;
        case T_DIR://ru
            
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            // 创建完整路径
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';
            // 读取文件和目录的目录信息
            while (read(fd, &de, sizeof(de)) == sizeof de) {
                if (de.inum == 0)
                    continue;
                if (strcmp(".", rtrim(de.name)) == 0 || strcmp("..", rtrim(de.name)) == 0)
                    continue;
                // 将文件名复制到完整路径
                memmove(p, de.name, DIRSIZ);
                // 创建以零结尾的字符串
                p[DIRSIZ] = '\0';
                  // 获取每个文件的状态信息
                if (stat(buf, &st) == -1) {
                    fprintf(2, "find: cannot stat '%s'\n", buf);
                    continue;
                }
                
                if (st.type == T_DEVICE || st.type == T_FILE) {
                    if (strcmp(name, rtrim(de.name)) == 0) {
                        printf("%s\n", buf);
                    }
                }
                else if (st.type == T_DIR) {
                    find(buf, name);
                }
            }
    }
}

int
main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(2, "Incorrect input format\n");
        exit(0);
    }
    char* path = argv[1];
    char* name = argv[2];
    
    find(path, name);
    exit(0);
}
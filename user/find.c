#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *Extraname(char *path) { // 在完整路径中提取文件名
    // 找到最后一个左斜线 '/'
    char *p;
    for (p = path + strlen( path ); p >= path && *p != '/'; p --) ;// 从路径末尾开始找
    p ++;

    if (strlen( p ) >= DIRSIZ)
        return p;

    static char buf[DIRSIZ + 1];
    memmove(buf, p, strlen( p ));
    buf[strlen( p )] = '\0';
    return buf;
}

void Comparename(char *path,char *target) {
    if (! strcmp(Extraname(path), target))
        printf("%s\n", path);
}
void find(char *path,char *name) {
    int fd; // 文件描述符

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return ;
    }

    struct stat st; // 文件状态结构体
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        return ;
    }

    char buf[512], *p;
    struct dirent de; // 目录项结构体
    switch ( st.type ) {
        case T_FILE: { // 当前路径是文件
            break;
        }
        case T_DIR: {
            if (strlen( path ) + 1 + DIRSIZ + 1 > sizeof( buf )) { // 路径 + '/' + 最大文件名 + '\0'
                printf("find: path too long to print");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen( buf );
            *p ++ = '/';

            // 遍历目录
            while (read(fd, &de, sizeof( de )) == sizeof( de )) {
                // 跳过无效目录和 '.' ".."
                if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;
                memmove(p, de.name, strlen( de.name )); // 拼接完整路径
                p[strlen( de.name )] = '\0';

                Comparename(buf, name);
                find(buf, name);
            }
            break;
        }
    }
    close( fd );
}

int main(int argc,char *argv[]) {
    if (argc < 3) {
        printf("find implements with format <find path filename>\n");
        exit( -1 );
    }
    find(argv[1], argv[2]); // file name
    exit( 0 );
}
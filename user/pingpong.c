#include "kernel/types.h"
#include "user.h"
#include "stddef.h"

int main(int argc,char* argv[]) {
   if (argc != 1) {
        printf("pingpong needs only one augment");
        exit( -1 );
   }
   int p[2]; // p[0] for read; p[1] for write
   char buf[5];
   pipe( p );
   int pid;

   int Res = fork();
   if (Res == 0) { // 子进程
     read(p[0], buf, sizeof( buf ));
     close( p[0] );

     pid = getpid();
     printf("%d: received ping from pid %s\n", pid, buf);

     itoa(pid, buf);
     write(p[1], buf, sizeof( buf ));
     close( p[1] );
   }
   else { // 父进程
     pid = getpid();
     itoa(pid, buf);

     write(p[1], buf, sizeof( buf ));
     close( p[1] );

     wait( 0 ); // 用一根管子需要等待子进程结束，两根管子则独立
     read(p[0], buf, sizeof( buf ));
     close( p[0] );

     printf("%d: received pong from pid %s\n", pid, buf);
   }
   exit( 0 );
}
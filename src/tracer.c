#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include "/home/wally/SO_aulas/SO/include/tracer.h"

int main(int args, char* argv[]){
    int fifo = open("./obj/fifo", O_WRONLY);
    int fd[2];
    processo name;
    struct timeval tv;
    char** arg = &argv[3]; 

    if(pipe(fd) < 0){
        return 2;
    }

    if(strcmp("status",argv[1]) == 0){

    }
    else{
        int pid = fork();
        if(pid == -1){
            printf("Fork nao abriu");
            return 2;
        }
        if (pid == 0){
            close(fd[0]);
            int x = getpid();
            name.programa = argv[3];
            name.processo = x;
            name.timestamp = gettimeofday(&tv,NULL);
            write(fd[1],&name,sizeof(processo));
            execvp(argv[3],arg);
        }
        else{
            close(fd[1]);
            read(fd[0],&name,sizeof(processo));
            write(fifo,&name,sizeof(processo));
            close(fd[1]);
        }
    }
}
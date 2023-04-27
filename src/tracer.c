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
    int fd = open("./obj/fifo", O_WRONLY);
    char** arg = &argv[3]; 
    if(strcmp("status",argv[1]) == 0){
        read(fd)
    }
    else{
        int pid = fork();
        if(pid == -1){
            printf("Fork nao abriu");
            return 2;
        }

        int time = gettimeofday(NULL,NULL);
        if (pid == 0){
            execvp(argv[2],arg);
        }
    }
}
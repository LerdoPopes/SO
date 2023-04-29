#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "/home/wally/SO_aulas/SO/include/tracer.h"

int main(int args, char* argv[]){
    int fifo = open("./obj/fifo", O_WRONLY);
    processo name;
    struct timeval tv;
    char** arg = &argv[3];

    if(strcmp("status",argv[1]) == 0){
        name.processo = 0;
        write(fifo, &name, sizeof(processo));
        mkfifo("./obj/status", 0666); 
        status status_f;
        char str[100] = {0};
        int stats = open("./obj/status",O_RDONLY);
        while(read(stats,&status_f,sizeof(status)) > 0){
            sprintf(str,"%d %s %ld ms\n", status_f.processo, status_f.programa, status_f.time);
            write(1,str,strlen(str));
        }
        close(stats);
    }
    else if(strcmp("execute",argv[1]) == 0){
        int pid = fork();
        if(pid == -1){
            printf("Fork nao abriu");
            return 2;
        }
        if (pid == 0){
            int x = getpid();
            strcpy(name.programa,argv[3]);
            name.processo = x;
            gettimeofday(&tv,NULL);
            name.tv = tv;
            write(fifo,&name,sizeof(processo));
            execvp(argv[3],arg);
        }
        else{
            wait(NULL);
            name.processo = pid;
            name.programa[0] = 0;
            gettimeofday(&tv,NULL);;
            name.tv = tv;
            write(fifo,&name,sizeof(processo));
            close(fifo);
        }
    }
}
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

char** parse(char* argv){
    char** answer = malloc(sizeof(char*)*20);
    int n = 0;
    char* tok;
    while((tok = strtok_r(argv, " ", &argv)) != NULL){
        answer[n] = tok;
        n++;
    }
    answer[n] = NULL;
    return answer;
}

int main(int args, char* argv[]){
    int fifo = open("./obj/fifo", O_WRONLY);
    processo name;
    struct timeval tv;
    char** arg;
    if(args > 2){
        arg = parse(argv[3]);
        name.processo = getpid();
        if((strcmp("execute",argv[1]) == 0) && (argv[2][1] == 'p')){
            int n = 0;
            int start, final = 0;
            int current = 1,np = 1;
            strcpy(name.programa, arg[0]);
            for(int i = 0; arg[i];i++,final++){
                if(arg[i][0] == '|'){
                    strcat(name.programa, " | \0");
                    strcat(name.programa, arg[i+1]);
                    np++;
                }
            }
            int fd[np][2];
            for(int i = 0; i<np;i++){
                pipe(fd[i]);
            }
            while(n<=final){
                start = n;
                while(n < final && arg[n][0] != '|'){
                    n++;
                }
                int pid1 = fork();
                if(pid1 == 0){
                    char** arguments = malloc(sizeof(char*)*n);
                    for(int i = start;i < n;i++){
                        arguments[i] = arg[i];
                    }
                    if(start != 0){
                        dup2(fd[current-2][0],0);
                        close(fd[current-2][0]);
                    }
                    else{
                        write(fifo,&name,sizeof(processo));
                    }
                    if(n < final){
                        dup2(fd[current-1][1],1);
                        close(fd[current-1][1]);
                    }
                    execvp(arguments[start], arguments);
                    _exit(0);
                }
                close(fd[current-1][1]);
                close(fd[current-1][0]);
                wait(NULL);
                current++;
                n++;
            }
            close(fd[current][0]);
            close(fd[current][1]);
            // name.processo = getpid();
            // name.programa[0] = 0;
            // gettimeofday(&tv,NULL);
            // name.tv = tv;
            // write(fifo,&name,sizeof(processo));
        }

        else if(strcmp("execute",argv[1]) == 0){
            int pid = fork();
            if(pid == -1){
                printf("Fork nao abriu");
                return 2;
            }
            if (pid == 0){
                int x = getppid();
                strcpy(name.programa,arg[0]);
                name.processo = x;
                gettimeofday(&tv,NULL);
                name.tv = tv;
                write(fifo,&name,sizeof(processo));
                execvp(arg[0],arg);
            }
            else{
                wait(NULL);
                // name.processo = getpid();
                // name.programa[0] = 0;
                // gettimeofday(&tv,NULL);
                // name.tv = tv;
                // write(fifo,&name,sizeof(processo));
                // close(fifo);
            }
        }
    }
    else if(strcmp("status",argv[1]) == 0){
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
        return 0;
    }
    name.processo = getpid();
    name.programa[0] = 0;
    gettimeofday(&tv,NULL);
    name.tv = tv;
    write(fifo,&name,sizeof(processo));
    close(fifo);
}
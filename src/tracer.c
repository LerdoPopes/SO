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

void send_request_time(int fd, int* pid_g, int size){
    for(int i = 0; i < size;i++){
        int x = pid_g[i];
        write(fd,&x,sizeof(int));
    }
}

int main(int args, char* argv[]){
    int fifo = open("./obj/fifo", O_WRONLY);
    processo name;
    struct timeval tv1;
    struct timeval tv2;
    char** arg;
    if(args > 2){
        arg = parse(argv[3]);
        name.processo = getpid();
        char str[40] = {0};
        sprintf(str,"Running PID %d\n",name.processo);
        write(1,str,strlen(str));
        if((strcmp("execute",argv[1]) == 0) && (strcmp("-p",argv[2]) == 0)){

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
            int fd[np-1][2];
            for(int i = 0; i<np;i++){
                pipe(fd[i]);
            }
            gettimeofday(&tv1,NULL);
            name.tv = tv1;
            while(n<=final){
                start = n;
                while(n < final && arg[n][0] != '|'){
                    n++;
                }
                int pid1 = fork();
                if(pid1 == 0){
                    char** arguments = malloc(sizeof(char*)*(n-start));
                    for(int i = start;i < n;i++){
                        arguments[i-start] = arg[i];
                    }
                    if(start != 0){
                        dup2(fd[current-2][0],STDIN_FILENO);
                        close(fd[current-2][0]);
                    }
                    else{
                        write(fifo,&name,sizeof(processo));
                    }
                    if(n < final){
                        dup2(fd[current-1][1],STDOUT_FILENO);
                        close(fd[current-1][1]);
                    }
                    execvp(arguments[0], arguments);
                }
                close(fd[current-1][1]);
                wait(NULL);
                current++;
                n++;
            }
        }

        else if((strcmp("execute",argv[1]) == 0) && (strcmp("-u",argv[2]) == 0)){
            gettimeofday(&tv1,NULL);
            name.tv = tv1;
            int pid = fork();
            if(pid == -1){
                printf("Fork nao abriu");
                return 2;
            }
            if (pid == 0){
                strcpy(name.programa,arg[0]);
                write(fifo,&name,sizeof(processo));
                execvp(arg[0],arg);
            }
            else{
                wait(NULL);
            }
        }
    }
    else if(strcmp("status",argv[1]) == 0){
        name.processo = 0;
        write(fifo, &name, sizeof(processo));
        int stats = open("./obj/status",O_RDONLY);
        status status_f;
        char str[100] = {0};
        while(read(stats,&status_f,sizeof(status)) > 0){
            if(status_f.processo == 0){
                break;
            }
            sprintf(str,"%d %s %ld ms\n", status_f.processo, status_f.programa, status_f.time);
            write(1,str,strlen(str));
        }
        close(stats);
        return 0;
    }

    else if(strcmp("status_time",argv[1]) == 0){
        int size = args-2;
        long time_t = 0;
        int* pid_g = malloc(sizeof(int)* size);
        for(int i = 0;i < size; i++){
            pid_g[i] = atoi(argv[i+2]);
        }
        name.processo = -1;
        write(fifo, &name, sizeof(processo));
        int stats = open("./obj/status_time",O_WRONLY);
        int pid = fork();
        if(pid == 0){
            send_request_time(stats,pid_g,size);
            _exit(0);
        }
        close(stats);
        wait(NULL);
        int stats2 = open("./obj/status_time",O_RDONLY);
        read(stats2,&time_t,sizeof(long));
        char answer[50] = {0};
        sprintf(answer,"Total execution time is %ld ms\n",time_t);
        close(stats);
        return 0;
    }


    else{
        return 0;
    }
    name.tv.tv_sec = 0;
    name.tv.tv_usec = 0;
    gettimeofday(&tv2,NULL);
    write(fifo,&name,sizeof(processo));
    char str2[50];
    sprintf(str2,"Program ended in %ld ms\n",((tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000));
    write(1,str2,strlen(str2));
    close(fifo);
}
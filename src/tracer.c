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

#define FILL_TV1 gettimeofday(&tv1,NULL);\
                name.tv = tv1

#define SEND_NAME_PROC write(fifo,&name,sizeof(processo))

#define TIME ((tv2.tv_sec - tv1.tv_sec) * 1000 + (tv2.tv_usec - tv1.tv_usec) / 1000)

#define INIT_STATUS int* pid_g = malloc(sizeof(int)* size);\
            for(;i < size; i++){\
                pid_g[i] = atoi(argv[i+2]);\
            }

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
    for(int i = 0; i <= size;i++){
        int x = pid_g[i];
        write(fd,&x,sizeof(int));
    }
}

void count_n(int n, int final, char** arg){
    while(n < final && arg[n][0] != '|'){
        n++;
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
            FILL_TV1;

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
                        SEND_NAME_PROC;
                    }
                    if(n < final){
                        dup2(fd[current-1][1],STDOUT_FILENO);
                        close(fd[current-1][1]);
                    }
                    execvp(arguments[0], arguments);
                }


                close(fd[current-1][1]);
                if(start != 0){
                    close(fd[current-2][0]);
                }
                wait(NULL);
                current++;
                n++;
            }
        }
        else if((strcmp("execute",argv[1]) == 0) && (strcmp("-u",argv[2]) == 0)){
            FILL_TV1;
            int pid = fork();
            if(pid == -1){
                printf("Fork nao abriu");
                return 2;
            }
            if (pid == 0){
                strcpy(name.programa,arg[0]);
                SEND_NAME_PROC;
                execvp(arg[0],arg);
            }
            else{
                wait(NULL);
            }
        }
        else if(strcmp("status_time",argv[1]) == 0){
            long time_t = 0;
            int size = args-2;
            int i = 0;
            INIT_STATUS;
            name.processo = -1;
            SEND_NAME_PROC;
            int stats = open("./obj/status_time",O_WRONLY);
            int pid = fork();
            if(pid == 0){
                send_request_time(stats,pid_g,size);
                _exit(0);
            }
            close(stats);
            wait(NULL);
            int stats2 = open("./obj/status_time",O_RDONLY);
            if(read(stats2,&time_t,sizeof(long)) > 0){
                char answer[50] = {0};
                sprintf(answer,"Total execution time is %ld ms\n",time_t);
                write(1, answer, strlen(answer));
                close(stats);
                return 0;
            }
        }
        else if(strcmp("status_command",argv[1]) == 0){
            int times = 0;
            int size = args-3;
            name.processo = -2;
            strcpy(name.programa,argv[2]);
            name.tv.tv_sec = size;
            int* pid_g = malloc(sizeof(int)* size);\
            for(int i = 0;i < size; i++){\
                pid_g[i] = atoi(argv[i+3]);
            }
            SEND_NAME_PROC;
            int stats = open("./obj/status_command",O_WRONLY);
            int pid = fork();
            if(pid == 0){
                send_request_time(stats,pid_g,size-1);
                _exit(0);
            }
            close(stats);
            wait(NULL);
            int stats2 = open("./obj/status_command",O_RDONLY);
            if(read(stats2,&times,sizeof(int)) > 0){
                char answer[50] = {0};
                sprintf(answer,"%s was executed %d times\n",argv[2],times);
                write(1, answer, strlen(answer));
                close(stats);
                return 0;
            }
        }
    }
    else if(strcmp("status",argv[1]) == 0){
        name.processo = 0;
        SEND_NAME_PROC;
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
    else{
        return 0;
    }
    name.tv.tv_sec = 0;
    name.tv.tv_usec = 0;
    gettimeofday(&tv2,NULL);
    SEND_NAME_PROC;
    char str2[50];
    sprintf(str2,"Program ended in %ld ms\n",TIME);
    write(1,str2,strlen(str2));
    close(fifo);
}
#include <bits/types/struct_timeval.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "../include/monitor.h"

void remove_proc(processo* arr_procs, int pid, int size, int fd, int args){
    struct timeval tv;
    status name;
    for (int i = 0; i < size;i++) {
        if (arr_procs[i].processo == pid){
            if(args > 1){
                    gettimeofday(&tv, NULL);
                    long time = (tv.tv_sec - arr_procs[i].tv.tv_sec) * 1000 + (tv.tv_usec - arr_procs[i].tv.tv_usec) / 1000;
                    name.processo = pid;
                    strcpy(name.programa, arr_procs[i].programa);
                    name.time = time;
                    write(fd,&name,sizeof(status));
                    close(fd);
                }
            arr_procs[i].processo = 0;
        }
    }
}

int parse_n_count(char* name,char* comp){
    int n = 0;
    char* tok;
    while((tok = strtok_r(name, " ", &name)) != NULL){
        if(strcmp(tok,comp) == 0){
            n++;
        }
    }
    return n;
}

void write_status(int fifo, processo* arr_procs, int size){
    status name;
    for(int i = 0; i < size; i++){
        if(arr_procs[i].processo != 0){
            struct timeval tv;
            gettimeofday(&tv,NULL);
            name.processo = arr_procs[i].processo;
            strcpy(name.programa,arr_procs[i].programa);
            name.time = (tv.tv_sec - arr_procs[i].tv.tv_sec) * 1000 + (tv.tv_usec - arr_procs[i].tv.tv_usec) / 1000;
            write(fifo,&name, sizeof(status));
        }
    }
    name.processo = 0;
    write(fifo,&name,sizeof(status));
}

int main(int args, char* argv[]){
    if(mkfifo("./obj/fifo",0640) < 0){
        printf("Fifo already exists\n");
    };
    mkfifo("./obj/status", 0640);
    int fifo = open("./obj/fifo", O_RDWR);
    int size = 20;
    processo* arr_procs = malloc(sizeof(processo)*20);
    processo name;
    char str[50];
    for (int i = 0; i<size; i++) {
        arr_procs[i].processo = 0;
    }
    while(1){
        if(read(fifo,&name,sizeof(processo))>0){
            if(name.processo == 0){
                int pid = fork();
                if (pid == 0){
                    int fifo_status = open("./obj/status",O_WRONLY);
                    write_status(fifo_status,arr_procs,size);
                    close(fifo_status);
                    _exit(0);
                }
                wait(NULL);
            }
            else if(name.processo == -1){
                status pname;
                int pid = fork();
                if (pid == 0){
                    long time = 0;
                    int x = 0;
                    int fifo_status = open(name.statsp,O_RDONLY);
                    while(read(fifo_status,&x,sizeof(int)) > 0){
                        sprintf(str,"%s/%d",argv[1],x);
                        int file = open(str,O_RDONLY);
                        read(file,&pname,sizeof(status));
                        time += pname.time;
                    }
                    close(fifo_status);
                    int fifo_status2 = open(name.statsp,O_WRONLY);
                    write(fifo_status2,&time,sizeof(long));
                    close(fifo_status2);
                    _exit(0);
                }
                wait(NULL);
            }
            else if(name.processo == -2){
                int pid = fork();
                if (pid == 0){
                    int times = 0;
                    long size = name.tv.tv_sec;
                    int fifo_status = open(name.statsp,O_RDONLY);
                    int n[size];
                    int i = 0;
                    while(read(fifo_status,&(n[i]),sizeof(int)) > 0){
                        i++;
                    }
                    for(int j = 0; j < size; j++){
                        status namae;
                        sprintf(str,"%s/%d",argv[1],n[j]);
                        int file = open(str,O_RDONLY);
                        read(file,&namae,sizeof(namae));
                        times += parse_n_count(namae.programa,name.programa);
                    }
                    close(fifo_status);
                    int fifo_status2 = open(name.statsp,O_WRONLY);
                    write(fifo_status2,&times,sizeof(int));
                    close(fifo_status2);
                    _exit(0);
                }
                wait(NULL);
            }
            else if(name.processo == -3){
                status pname;
                int pid = fork();
                if (pid == 0){
                    int x = 0;
                    int size = 30;
                    char** names = malloc(sizeof(char*) * size); 
                    for(int i = 0; i < size; i++){
                        names[i] = malloc(50);
                        names[i][0] = 0;
                    }
                    int fifo_status = open(name.statsp,O_RDONLY);
                    int iter = 0;
                    while(read(fifo_status,&x,sizeof(int)) > 0){
                        sprintf(str,"%s/%d",argv[1],x);
                        int file = open(str,O_RDONLY);
                        read(file,&pname,sizeof(status));
                        char* nome = pname.programa;
                        char* tok;
                        while((tok = strtok_r(nome, " |", &nome)) != NULL){
                            strcpy(names[iter],tok);
                            iter++;
                        }
                    }
                    close(fifo_status);
                    for(int i = 0;i < size;i++){
                        for(int j = i+1;j < size;j++){
                            if(names[i][0] && names[j][0] &&(strcmp(names[i],names[j])== 0)){
                                names[j][0] = 0;
                            }
                        }
                    }
                    int fifo_status2 = open(name.statsp,O_WRONLY);
                    for(int i = 0; i < size;i++){
                        if(names[i][0]){
                            write(fifo_status2,names[i],50);
                        }
                    }
                    close(fifo_status2);
                    _exit(0);
                }
                wait(NULL);
            }
            else if(name.tv.tv_sec == 0 && name.tv.tv_usec == 0){
                int fd = 0;
                if(args > 1){
                    sprintf(str,"%s/%d",argv[1],name.processo);
                    fd = open(str,O_CREAT | O_TRUNC | O_WRONLY, 0640);
                }
                remove_proc(arr_procs, name.processo,size,fd,args);
            }
            else{
                int i = 0;
                for (; arr_procs[i].processo != 0;i++){
                    if(i == size){
                        arr_procs = realloc(arr_procs, sizeof(processo)*(size*=2));
                        for(int j = i; j < size;j++){
                            arr_procs[i].processo = 0;
                        }

                    }
                }
                arr_procs[i] = name;
            }
        }
    }
}

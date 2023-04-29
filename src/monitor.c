#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "/home/wally/SO_aulas/SO/include/monitor.h"

void remove_proc(processo* arr_procs, int pid, int size){
    for (int i = 0; i < size;i++) {
        if (arr_procs[i].processo == pid){
            arr_procs[i].processo = 0;
        }
    }
}

void write_status(int fifo, processo* arr_procs, int size){
    for(int i = 0; i < size; i++){
        if(arr_procs[i].processo != 0){
            status name;
            struct timeval tv;
            gettimeofday(&tv,NULL);
            name.processo = arr_procs[i].processo;
            strcpy(name.programa,arr_procs[i].programa);
            name.time = (tv.tv_sec - arr_procs[i].tv.tv_sec) * 1000 + (tv.tv_usec - arr_procs[i].tv.tv_usec) / 1000;
            //printf("%d %s\n",name.processo, name.programa);
            write(fifo,&name, sizeof(status));
        }
    }
    close(fifo);
}

int main(int args, char* argv[]){
    if(mkfifo("./obj/fifo",0666) < 0){
        printf("Fifo already exists\n");
    };
    int fifo = open("./obj/fifo", O_RDWR);
    int size = 20;
    processo* arr_procs = malloc(sizeof(processo)*20);
    processo name;
    for (int i = 0; i<size; i++) {
        arr_procs[i].processo = 0;
    }
    while(1){
        if(read(fifo,&name,sizeof(processo))>0){
            if(name.processo == 0){
                int fifo_status = open("./obj/status",O_WRONLY);
                write_status(fifo_status,arr_procs,size);
            }
            else if(name.programa[0] == 0){
                remove_proc(arr_procs, name.processo,size);
            }
            else{
                int i;
                for (i = 0; arr_procs[i].processo != 0 && i < size;i++);
                arr_procs[i] = name;
            }
        }
    }
}
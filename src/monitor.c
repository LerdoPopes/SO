#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "/home/wally/SO_aulas/SO/include/monitor.h"

int main(int args, char* argv[]){
    if(mkfifo("./obj/fifo",0666) < 0){
        printf("Fifo already exists\n");
    };
    int fifo = open("./obj/fifo", O_RDWR);
    int size = 10;
    processo* arr_procs = malloc(sizeof(processo)*10);
    while(1){
        processo name;
        if(read(fifo,&name,sizeof(int))>0){

        }
    }
}
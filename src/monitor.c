#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <sys/stat.h>
#include "/home/wally/SO_aulas/SO/include/monitor.h"

int main(int args, char* argv[]){
    if(mkfifo("./obj/fifo",0666) < 0){
        printf("Fifo already exists\n");
    };
    int fd = open("./obj/fifo", O_RDONLY);
    while(1){
        processo name;
        read(fd,&name,sizeof(processo));
        if(name.processo == 0){
            
        }
    }
}
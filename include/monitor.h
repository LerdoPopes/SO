#include <sys/time.h>
#include <stdlib.h>

typedef struct processo{
    int processo;
    struct timeval tv;
    char programa[50];
} processo;

typedef struct status{
    int processo;
    char programa[20];
    long time;
}status;
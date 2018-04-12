
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
    

// Очень простая программка для удаления очередей по msqid
int main(int argc, char **argv)
{
    if (argc < 2){
        printf("usage: ./prog <msgqid> ...");
    }

    int msqid = atoi(argv[1]);

    msgctl(msqid, IPC_RMID, NULL);
    
    return EXIT_SUCCESS; 
}

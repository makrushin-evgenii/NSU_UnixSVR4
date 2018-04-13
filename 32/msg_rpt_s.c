// ==++==
//
//   Sender
//
// ==--==
/*============================================================
**
** Несколько копий программы отправляют статус (прогресс выполнения)
** мастеру каждую секунду. 
** После завершения цикла отправляет сообщение о завершении работы.
** 
===========================================================*/
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define MAX_SZ 30

int main(int argc, char *argv[])
{
    int mqid;

    struct
    {
        long mtype;
        char mtext[MAX_SZ];
    } buf;

    char *valuep;
    if ((valuep = getenv("MQID")) == NULL)
    {
        fprintf(stderr, "%s: can't getenv MQID\n", argv[0]);
        exit(1);
    }
    else
        sscanf(valuep, "%d", &mqid);

    pid_t pid = getpid();
    buf.mtype = pid;
    srand((unsigned)pid);

    int goal = rand() % 9 + 2;
    for (int progress = 1; progress <= goal; progress++)
    {
        if (progress == goal)
        {
            strcpy(buf.mtext, "DONE");
        }
        else
        {
            sprintf(buf.mtext, "%d/%d", progress, goal);
        }
        if (msgsnd(mqid, &buf, strlen(buf.mtext) + 1, 0) == -1)
        {
            fprintf(stderr, "%s: msgsnd error\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}

// ==++==
//
//   Receiver
//
// ==--==
/*============================================================
**
** Копия программы получает сообщения из очереди MQID, пока не встретит
** сообщение ограничитель. После чего сообщает об этом отправителю и прекращает работу.
**
===========================================================*/
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_SZ 80

int main(int argc, char *argv[])
{
    pid_t pid;
    char *valuep;
    int msgqid;

    struct
    {
        long mtype;
        char mtext[MAX_SZ];
    } buf;

    pid = getpid();

    // получение ид очереди
    if ((valuep = getenv("MQID")) == NULL)
    {
        fprintf(stderr, "%s:  can't  getenv   MQID\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    else
        sscanf(valuep, "%d", &msgqid);

    // получение сообщений, пока не встречен EOF
    for (;;)
    {
        msgrcv(msgqid, &buf, MAX_SZ, pid, MSG_NOERROR);
        printf("%s  received '%s'\n", argv[0], buf.mtext);
        if (!strcmp(buf.mtext, "EOF"))
            break;
    }

    // ответ отправителю, о завершении процесса
    buf.mtype = getppid();
    strcpy(buf.mtext, "DONE");
    if (msgsnd(msgqid, &buf, strlen(buf.mtext), 0) == -1)
    {
        fprintf(stderr, "%s: msgsnd error\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

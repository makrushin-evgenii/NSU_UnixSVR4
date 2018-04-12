#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_SZ 80

main(int argc, char *argv[])
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
        exit(1);
    }
    else
        sscanf(valuep, "%d", &msgqid);

    // получение сообщений, пока не встречен EOF
    while (1)
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
        exit(2);
    }

    return (0);
}

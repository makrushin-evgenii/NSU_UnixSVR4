// ==++==
//
//   Master
//
// ==--==
/*============================================================
**
** Получает статус-сообщения от нескольких процессов.
** Завершается после получения сообщений о завершении 
** от каждого процеса.
**
===========================================================*/
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>

#define MAX_SZ 80
#define NCHILD 3
#define IDLE_CHILD 0

struct msg
{
    long mtype;
    char mtext[MAX_SZ];
};

void set_sigquit_handler();

void set_sigint_handler();

int create_msg_query();

char **create_msg_query_id_envp(int msgqid);

void create_send_forks(char *const *envp, pid_t *r_pid, int count);

int msgqid;

int main()
{
    struct msg buf;
    pid_t r_pid[NCHILD];

    set_sigquit_handler();
    set_sigint_handler();

    // Создание очереди
    msgqid = create_msg_query();

    // Запись id очереди в переменную среды для использования в execle
    char **envp = create_msg_query_id_envp(msgqid);

    // создание копий программы-отправителя
    create_send_forks(envp, r_pid, NCHILD);

    // Получение сообщений и ожидание завершения
    int still_active = NCHILD;
    while (still_active)
    {
        if (msgrcv(msgqid, &buf, MAX_SZ, 0, 0) == -1)
        {
            continue;
        }

        if (!strncmp(buf.mtext, "DONE", 4))
        {
            still_active--;
        }

        printf("%ld: [%s]\n", buf.mtype, buf.mtext);
    }

    // Закрытие очереди
    msgctl(msgqid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}

int create_msg_query()
{
    int msgqid = msgget(IPC_PRIVATE, IPC_CREAT | 0660);
    if (msgqid == -1)
    {
        perror("Master can't make msg queue");
        exit(EXIT_FAILURE);
    }

    return msgqid;
}

char **create_msg_query_id_envp(int msgqid)
{
    static char qid_evar[40];
    static char *envp[2] = {qid_evar};

    sprintf(qid_evar, "MQID=%d", msgqid);

    return envp;
}

void create_send_forks(char *const *envp, pid_t *r_pid, int count)
{
    for (int child = 0; child < count; child++)
    {
        pid_t this_fork = fork();
        if (this_fork == -1)
        {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        else if (this_fork == 0)
        {
            char child_name[20];
            sprintf(child_name, "msg_rpt_s%d", child);
            execle("msg_rpt_s", child_name, (char *)0, envp);
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
        else
            r_pid[child] = this_fork;
    }
}

void close_query_and_quit(int sig)
{
    msgctl(msgqid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void set_sigquit_handler()
{
    if (sigset(SIGQUIT, close_query_and_quit) == SIG_ERR)
    {
        perror("Setting SIGQUIT");
        exit(EXIT_FAILURE);
    }
}

void set_sigint_handler()
{
    if (sigset(SIGINT, close_query_and_quit) == SIG_ERR)
    {
        perror("Setting SIGINT");
        exit(EXIT_FAILURE);
    }
}
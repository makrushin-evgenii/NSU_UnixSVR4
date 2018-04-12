#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>

// gcc msg_wall_s.c -o msg_wall_s -std=gnu99 -Wall

#define MAX_SZ 80
#define NCHILD 3

struct msg_s
{
    long mtype;
    char mtext[MAX_SZ];
};

void set_sigquit_handler();

void set_sigint_handler();

int create_msg_query();

char **create_msg_query_id_envp(int msgqid);

void send_msg_until_eof(int MSGQID, struct msg_s buf, const pid_t *r_pid);

void create_rec_forks(char *const *envp, pid_t *r_pid, int count);

void wait_sole_use(int MSGQID, int still_active, struct msg_s buf);

void string_trim(char * s);

int msgqid;

int main()
{
    struct msg_s buf;
    pid_t r_pid[NCHILD];

    // Создание очереди
    msgqid = create_msg_query();

    // Добавил обработку сигналов на выход, чтобы не плодить новые очереди
    set_sigint_handler();
    set_sigquit_handler();

    // Запись id очереди в переменную среды для использования в execle
    char **envp = create_msg_query_id_envp(msgqid);

    // создание копий программы-получателя
    create_rec_forks(envp, r_pid, NCHILD);

    // отправка сообщений всем получателям
    send_msg_until_eof(msgqid, buf, r_pid);

    // ожидание сообщений от всех получателей
    wait_sole_use(msgqid, NCHILD, buf);

    // Закрытие очереди
    msgctl(msgqid, IPC_RMID, NULL);

    return EXIT_SUCCESS;
}

void create_rec_forks(char *const *envp, pid_t *r_pid, int count)
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
            sprintf(child_name, "mwall_r%d", child);
            execle("msg_wall_r", child_name, (char *)0, envp);
            perror("exec failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            r_pid[child] = this_fork;
        }
    }
}

void send_msg_until_eof(int MSGQID, struct msg_s buf, const pid_t *r_pid)
{
    int eof = 0;
    for (;;)
    {
        fprintf(stdout, "Enter message to be sent to all receivers: ");
        if (fgets(buf.mtext, MAX_SZ, stdin) == NULL)
        {
            eof++;
            fprintf(stdout, "\n");
            strcpy(buf.mtext, "EOF");
        }

        string_trim(buf.mtext);

        int msg_length = (int)(strlen(buf.mtext) + 1);

        for (int child = 0; child < NCHILD; child++)
        {
            buf.mtype = r_pid[child];
            if (msgsnd(MSGQID, &buf, (size_t)msg_length, 0) == -1)
            {
                perror("Producer msgsnd error");
                exit(4);
            }
        }

        if (eof)
        {
            break;
        }

        sleep(1);
    }
}

char **create_msg_query_id_envp(int msgqid)
{
    static char qid_evar[40];
    static char *envp[2] = {qid_evar};

    sprintf(qid_evar, "MQID=%d", msgqid);

    return envp;
}

int create_msg_query()
{
    int msgqid = msgget(IPC_PRIVATE, IPC_CREAT | 0660);
    if (msgqid == -1)
    {
        perror("Sender can't make msg queue");
        exit(EXIT_FAILURE);
    }

    return msgqid;
}

void wait_sole_use(int MSGQID, int still_active, struct msg_s buf)
{
    pid_t s_pid;
    s_pid = getpid();

    while (still_active)
    {
        for (int i = 0; i < NCHILD; i++)
            if (msgrcv(MSGQID, &buf, MAX_SZ, s_pid, IPC_NOWAIT) != -1)
                if (!strncmp(buf.mtext, "DONE", 4))
                    still_active--;

        sleep(2);
    }
}

void string_trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
} 

void close_query_and_quit(int sig)
{
    msgctl(msgqid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}

void set_sigquit_handler()
{
    if (sigset(SIGQUIT, close_query_and_quit) == SIG_ERR) {
        perror("Setting SIGQUIT");
        exit(EXIT_FAILURE);
    }
}

void set_sigint_handler()
{
    if (sigset(SIGINT, close_query_and_quit) == SIG_ERR) {
        perror("Setting SIGINT");
        exit(EXIT_FAILURE);
    }
}

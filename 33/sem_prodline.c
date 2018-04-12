// ИЗГОТОВИТЕЛЬ ВИДЖЕТОВ
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "production.h"

static void endsim();

static int semprodn;

static struct sembuf partc_sub1[2] = {
    {PARTC, -1, SEM_UNDO},
    {SUB1, -1, SEM_UNDO}};

int main(int argc, char *argv[])
{
    char asc_prod_key[20];

    if ((semprodn = semget(IPC_PRIVATE, 4, IPC_CREAT | 0640)) == -1)
    {
        printf("Can't get production line semaphore set\n");
        exit(EXIT_FAILURE);
    }
    sigset(SIGINT, endsim);
    sprintf(asc_prod_key, "%d", semprodn);

    if (fork() == 0)
    {
        execl("prodpart", "a", asc_prod_key, (char *)0);
        perror("a");
        exit(1);
    }

    if (fork() == 0)
    {
        execl("prodpart", "b", asc_prod_key, (char *)0);
        perror("b");
        exit(1);
    }

    if (fork() == 0)
    {
        execl("prodpart", "c", asc_prod_key, (char *)0);
        perror("c");
        exit(1);
    }

    if (fork() == 0)
    {
        execl("prodsub1", "prodsub1", asc_prod_key, (char *)0);
        perror("prodsub1");
        exit(1);
    }

    for (int widget = 1; widget <= NWIDGETS; widget++)
    {
        if (semop(semprodn, partc_sub1, 2) == -1)
        {
            perror(argv[0]);
            exit(EXIT_FAILURE);
        };
        printf("\x1b[32m %s: Виджет готов: [ %d / %d ] \x1b[0m \n", argv[0], widget, NWIDGETS);
    }

    endsim();
}

static void endsim()
{
    semctl(semprodn, IPC_RMID, 0);
    kill(0, SIGTERM);
    exit(EXIT_SUCCESS);
}
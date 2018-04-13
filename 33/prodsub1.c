// ИЗГОТОВИТЕЛЬ МОДУЛЯ 1, prodsub1.c:
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include "production.h"

int main(int argc, char *argv[])
{
    int semprodn;
    int unit = 0;
    static struct sembuf part_ab[2] =
        {
            {PARTA, -1, SEM_UNDO},
            {PARTB, -1, SEM_UNDO}};

    static struct sembuf sub1 = {SUB1, 1, SEM_UNDO};

    semprodn = atoi(argv[1]);
    setbuf(stdout, NULL);

    for (;;)
    {
        if (semop(semprodn, &part_ab[0], 2) == -1)
        {
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }

        printf("%s: Модуль-1 готов: %d\n", argv[0], ++unit);

        if (semop(semprodn, &sub1, 1) == -1)
        {
            fprintf(stderr, "sub1 error\n");
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
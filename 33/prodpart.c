// ИЗГОТОВИТЕЛЬ ДЕТАЛЕЙ
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "production.h"

int main(int argc, char *argv[])
{
    int semprodn;
    int unit = 0;
    static struct sembuf parti = {HOLDER, 1, SEM_UNDO};
    static int prodtimeabc[3] = {2, 3, 4};
    unsigned short index;

    semprodn = atoi(argv[1]);
    index = argv[0][0] - 'a'; /* argv[0] == [abc] */
    parti.sem_num = index;
    setbuf(stdout, NULL);

    for (;;)
    {
        sleep(prodtimeabc[index]);
        printf("%s: Деталь готова: %d\n", argv[0], ++unit);
        semop(semprodn, &parti, 1);
    }

    return EXIT_SUCCESS;
}
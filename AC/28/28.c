#include <stdio.h>  // FILE
#include <libgen.h> // p2open
#include <stdlib.h> // EXIT_*

#define INTS_COUNT 100
#define INTS_PER_LINE 10
#define RANDOM_MAX_VALUE 100
#define READ_PIPE 1
#define WRITE_PIPE 0

// про p2open читать тут:
// http://ccfit.nsu.ru/~deviv/courses/unix/unix/ngb03be.html

// При компиляции не забыть про ключ -lgen:
// cc [flag ...] file ... -lgen [library ...]

void initialize_rand(FILE *fd[]);
void write_rand_ints(FILE *fd);
void print_rand_ints(FILE *fd);

int main(int argc, char **argv)
{
    FILE *fd[2];
    if (p2open("sort -bn", fd) == -1)
    {
        perror("open pipes");
        return EXIT_FAILURE;
    }

    initialize_rand(fd);

    write_rand_ints(fd[WRITE_PIPE]);
    fclose(fd[WRITE_PIPE]);

    print_rand_ints(fd[READ_PIPE]);
    fclose(fd[READ_PIPE]);

    return EXIT_SUCCESS;
}

void initialize_rand(FILE *fd[])
{
    time_t cur_time = time(NULL);
    if (cur_time == -1)
    {
        perror("get time");
        p2close(fd);
        exit(EXIT_FAILURE);
    }
    srand(cur_time);
}

void write_rand_ints(FILE *fd)
{
    for (int i = 0; i < INTS_COUNT; i++)
    {
        fprintf(fd, "%d\n", rand() % RANDOM_MAX_VALUE);
    }
}

void print_rand_ints(FILE *fd)
{
    for (int i = 1; i <= INTS_COUNT; ++i)
    {
        int value;
        fscanf(fd, "%d", &value);
        printf("%4d ", value);

        if (i % INTS_PER_LINE == 0 || i == INTS_COUNT)
        {
            printf("\n");
        }
    }
}

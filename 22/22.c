#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TIME_OUT 3
#define MAXOPEN 20

int files_count;
FILE *fd[MAXOPEN];
char *file_name[MAXOPEN];      
int file_row[MAXOPEN];

int alarm_flag = 0;

void set_alarm_handler();
void alarm_handler(int sig);

void open_files(int argc, char **argv);
void zip_files();

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s filename [filename...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    set_alarm_handler();
    open_files(argc, argv);
    zip_files();

    return EXIT_SUCCESS;
}

void open_files(int argc, char **argv)
{
    files_count = 0;
    while (--argc)
    {
        file_name[files_count] = argv[files_count + 1];
        file_row[files_count] = 1;

        if ((fd[files_count] = fopen(file_name[files_count], "rb")) == NULL)
        {
            fprintf(stderr, "%s: cannot open %s\n", argv[0], file_name[files_count]);
            exit(EXIT_FAILURE);
        }

        ++files_count;
        if (files_count >= 20)
        {
            fprintf(stderr, "%s:too many file names", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

void zip_files()
{
    char line_buf[BUFSIZ];

    int open_files = files_count;
    while (open_files)
    {
        for (int i = 0; i < files_count; ++i)
        {
            if (fd[i] == NULL)
            {   // файл был закрыт
                continue;
            }

            alarm(TIME_OUT);

            if (fgets(line_buf, BUFSIZ, fd[i]) == NULL && !alarm_flag)
            {   // EOF => закрыть файл
                fclose(fd[i]);
                fd[i] = NULL;
                --open_files;
            }
            else if (alarm_flag)
            {   // TIME_OUT => перейти к следующему файлу
                alarm_flag = 0;
                continue;
            }
            else
            {   // OK => вывести строку
                printf("\x1b[32m%s\x1b[0m:%d: ", file_name[i], file_row[i]++);
                fputs(line_buf, stdout);
            }
            
            alarm(0);
        }
    }
}

void set_alarm_handler()
{
    if (signal(SIGALRM, &alarm_handler) == SIG_ERR)
    {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    siginterrupt(SIGALRM, 1);
}

void alarm_handler(int sig)
{
    siginterrupt(SIGALRM, 1);   // разрешает прерывать системные вызовы
    // Если аргумент flag истинен (1) и началась пересылка данных, то системный вызов будет прерван и вернёт действительное количество пересланных данных.
    alarm_flag = 1;
}

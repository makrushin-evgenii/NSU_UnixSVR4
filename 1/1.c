#include <stdlib.h>
#include <stdio.h>

#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ulimit.h>
#include <sys/resource.h>

extern char **environ;

void print_user_and_group_ids();
void set_process_as_group_lider();
void print_processes_ids();
void print_ulimit(int resource);
void set_ulimit(int resource, char *value);
void print_work_directory();
void print_enviroment_vars();
void set_enviroment_var(char *str);

int main(int argc, char *argv[])
{
    // Если первым символом optstring является "-", то каждый элемент argv, не являющийся опцией,
    // обрабатывается так, как если бы он был аргументом опции с символом, имеющим код 1.
    // Она используется программами, которые требуют опции и другие аргументы командной строки.
    char optstring[] = "-ispuU:cC:dvV:";
    int c, invalid = 0;

    // Вызывающая программа может предотвратить вывод сообщения об ошибке, установив нулевое значение opterr.
    opterr = 0;

    printf("Передано аргуметов: %d\n", argc - 1);
    while ((c = getopt(argc, argv, optstring)) != EOF)
    {
        switch (c)
        {
        case 'i':
            print_user_and_group_ids();
            break;
        case 's':
            set_process_as_group_lider();
            break;
        case 'p':
            print_processes_ids();
            break;
        case 'u':
            print_ulimit(RLIMIT_AS);
            break;
        case 'c':
            print_ulimit(RLIMIT_CORE);
            break;
        case 'U':
            set_ulimit(RLIMIT_AS, optarg);
            break;
        case 'C':
            set_ulimit(RLIMIT_CORE, optarg);
            break;
        case 'd':
            print_work_directory();
            break;
        case 'v':
            print_enviroment_vars();
            break;
        case 'V':
            set_enviroment_var(optarg);
            break;
        case '-':
            break;
            // Если getopt() не распознал символ опции, то он выводит на стандартный поток
            // ошибок соответствующее сообщение, заносит символ в optopt и возвращает "?"
            //
            // Если функция getopt() обнаружит в argv символ опции, не найденный в optstring,
            // или если она обнаружит пропущенный аргумент опции, то она возвратит `?'
            // и занесет во внешную переменную optopt действительный символ опции.
        case '?':
            printf("invalid option is %c\n", optopt);
            invalid++;
        }
    }
    return 0;
}

void print_user_and_group_ids()
{
    printf("-i:\n");
    printf("\tuid = %u\n\teuid = %u\n\tgid = %u\n\tegid = %u\n",
           (unsigned int)getuid(),
           (unsigned int)geteuid(),
           (unsigned int)getgid(),
           (unsigned int)getegid());
}

void set_process_as_group_lider()
{
    setpgid(getpid(), getpgid(0));
}

void print_processes_ids()
{
    printf("-p:\n");
    printf("\tpid = %d\n\tppid = %d\n\tpgid = %d\n",
           (unsigned int)getpid(),
           (unsigned int)getppid(),
           (unsigned int)getpgid(0));
}

void print_ulimit(int resource)
{
    printf("-%c:\n", (resource == RLIMIT_AS ? 'u' : 'c'));

    struct rlimit rst;

    if (getrlimit(resource, &rst) == 0)
    {
        printf("\tsoft = %u\n\thard = %u\n",
               (unsigned int)rst.rlim_cur,
               (unsigned int)rst.rlim_max);
    }
    else
    {
        perror("getrlimit");
    }
}

void set_ulimit(int resource, char *value)
{
    printf("-%c:\n", (resource == RLIMIT_AS ? 'U' : 'C'));

    struct rlimit rst;

    if (getrlimit(resource, &rst) == 0)
    {
        long v = atol(value);

        if (v == 0 && strcmp(value, "0") != 0)
        {
            fprintf(stderr, "не число\n");
            return;
        }

        if (v < 0)
        {
            fprintf(stderr, "значение отрицательное\n");
            return;
        }

        if (v > rst.rlim_max)
        {
            fprintf(stderr, "значение слишком большое\n");
            return;
        }

        rst.rlim_cur = atol(value);
        if (setrlimit(resource, &rst) == 0)
        {
            printf("\tизменено на %ld\n", v);
        }
        else
        {
            perror("setrlimit");
            return;
        }
    }
    else
    {
        perror("getrlimit");
        return;
    }
}

void print_work_directory()
{
    printf("-d:\n");

    char path_name[PATH_MAX];

    if (getcwd(path_name, PATH_MAX) == NULL)
        perror("getcwd");
    else
        printf("Текущая директория: %s\n", path_name);
}

void print_enviroment_vars()
{
    printf("-v:\n");
    int i = 0;
    while (environ[i] != NULL)
    {
        printf("\t%s\n", environ[i]);
        i++;
    }
}

void set_enviroment_var(char *str)
{
    putenv(str);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <glob.h>

#define BOLD_TEXT(text) "\033[1m" #text "\033[0m"
#define CALL_TEXT(text) "\e[32;1m" #text "\e[32;0m"

void read_template(char *mask);

void print_matched_files(char *mask);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        char mask[PATH_MAX + 1];
        read_template(mask);
        print_matched_files(mask);
    }
    else if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        printf("usage: %s <globbing pathnames>\n", argv[0]);
        printf(BOLD_TEXT(Wildcard matching) "\n");
        printf("\tA '?' matches any single character.\n");
        printf("\tA '*' matches any string, including the empty string.\n");
    }
    else
    {
        int i;
        for (i = 1; i < argc; ++i)
        {
            print_matched_files(argv[i]);
        }
    }

    return EXIT_SUCCESS;
}

void read_template(char *mask)
{
    printf(CALL_TEXT($) " ");

    int length = 0;

    for (;;)
    {
        if (length > PATH_MAX)
        {
            fprintf(stderr, "Too long template. Max file name length is %d\n", PATH_MAX);
            exit(EXIT_FAILURE);
        }

        char read_symb = (char)fgetc(stdin);

        if (read_symb == EOF || read_symb == '\n')
        {
            break;
        }

        // http://man7.org/linux/man-pages/man3/nftw.3.html ?
        // Программа уже работает с '/'  шаблонах. Поэтому ограничение убрал

        // if (read_symb == '/') {
        //     fprintf(stderr, "'/' must not occur in the template.\n");
        //     exit(EXIT_FAILURE);
        // }

        mask[length++] = read_symb;
    }

    if (length == 0)
    {
        mask[length++] = '*';
    }

    mask[length] = '\0';
}

int errFunc(const char *epath, int errno)
{
    fprintf(stderr, "Glob error. Couldn't check for matching: %s\n", epath);
    return EXIT_FAILURE;
}

void print_matched_files(char *mask)
{
    glob_t glob_results;

    // Параметр flags является нулем или поразрядным ИЛИ нескольких следующих символьных констант, которые меняют ход работы glob():
    // GLOB_NOCHECK - если не найдено совпадений по шаблону, то возвращать в качестве результата заданный шаблон;
    int glob_status = glob(mask, GLOB_NOCHECK, errFunc, &glob_results);

    // При нормальном завершении glob() возврашает ноль.
    // Другие возможные возвращаемые значения: GLOB_NOSPACE, GLOB_ABORTED, GLOB_NOMATCH
    if (glob_status != 0)
    {
        exit(EXIT_FAILURE);
    }

    // int matching_index = 0;

    // while (glob_results.gl_pathv[matching_index] != NULL) {
    //     printf("%s ", glob_results.gl_pathv[matching_index]);
    //     matching_index++;
    // }

    // При успешном завершении работы pglob->gl_pathc содержит количество найденных совпадений имен, а pglob->gl_pathv указывает на список найденных имен.
    // Указатель, который находится непосредственно за последним в списке имен, равен NULL.
    int matching_index = 0;
    for (matching_index = 0; matching_index < glob_results.gl_pathc; ++matching_index)
    {
        printf("%s ", glob_results.gl_pathv[matching_index]);
    }

    if (matching_index)
    {
        printf("\n");
    }

    globfree(&glob_results);
}

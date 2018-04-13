#include <stdio.h>
#include <stdlib.h>
#include <errno.h>     // error codes
#include <sys/types.h> // lseek, open
#include <sys/stat.h>  // open
#include <sys/mman.h>  // mmap
#include <fcntl.h>     // open
#include <unistd.h>    // lseek
#include <signal.h>

#define LINES_COUNT_MAX 100
#define END_LINE_NUMBER 0
#define WAIT_FOR_USER_INPUT_TIME 5

int BuildLinesOffsetsTable(char *src, long *linesOffsets, long *linesLengths);
int RequestingUserInputLoop(char *src, int linesCount, long *linesOffsets, long *linesLengths);
void PrintLine(char *src, int lineOffset, int linesLength);
void PrintLines(char *src);

void BindAlarmHandler();
void AlarmHandler(int sig);

int AlarmTriggered = 0;

int main(int argc, char *argv[])
{
    long linesOffsets[LINES_COUNT_MAX + 1];
    long linesLengths[LINES_COUNT_MAX + 1];

    if (argc != 2)
    {
        fprintf(stderr, "Использование: %s [file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fdin = open(argv[1], O_RDONLY);
    if (fdin < 0)
    {
        perror("невозможно открыть файл для чтения");
        return EXIT_FAILURE;
    }

    struct stat statbuf;
    if (fstat(fdin, &statbuf) < 0) /* определить размер входного файла */
    {
        perror("fstat error");
        return EXIT_FAILURE;
    }

    char *src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0);
    if (src == MAP_FAILED)
    {
        perror("ошибка вызова функции mmap для входного файла");
        return EXIT_FAILURE;
    }

    int linesCount = BuildLinesOffsetsTable(src, linesOffsets, linesLengths);

    BindAlarmHandler();

    RequestingUserInputLoop(src, linesCount, linesOffsets, linesLengths);

    munmap(0, statbuf.st_size);
    free(src);
    close(fdin);

    return EXIT_SUCCESS;
    //      munmapq
    //      free(src)
    // + найти еще одну утечку
}

/**
 * 
 * return: lines count
 */
int BuildLinesOffsetsTable(char *src, long *linesOffsets, long *linesLengths)
{
    char ch;
    int prevOffset = 0;
    int offset = 0;
    linesOffsets[0] = 0;
    int currentLine = 1;
    while ((ch = src[offset]) != '\0')
    {
        if (ch == '\n')
        {
            if (currentLine > LINES_COUNT_MAX + 1)
            {
                fprintf(stderr, "Error: file have more than %d lines\n", LINES_COUNT_MAX);
                exit(EXIT_FAILURE);
            }

            linesLengths[currentLine - 1] = offset - prevOffset;
            linesOffsets[currentLine] = offset + 1;
            prevOffset = linesOffsets[currentLine];
            currentLine++;
        }
        offset++;
    }

    linesLengths[currentLine - 1] = offset - prevOffset;

    return currentLine;
}

/**
 * 
 * return: EXIT_SUCCESS, EXIT_FAILURE if errors occured
 */
int RequestingUserInputLoop(char *src, int linesCount, long *linesOffsets, long *linesLengths)
{
    printf("Lines range: [%d, %d]\n", linesCount > 0, linesCount);
    do
    {
        printf("$ ");

        int lineNumber;
        char ending;

        siginterrupt(SIGALRM, 1);
        alarm(WAIT_FOR_USER_INPUT_TIME);

        int readCount = fscanf(stdin, "%d%c", &lineNumber, &ending);

        alarm(0);

        if (AlarmTriggered != 0)
        {
            printf("\nTIMEOUT. THE ENTIRE TEXT:\n");
            PrintLines(src);
            printf("\n");
            return EXIT_SUCCESS;
        }

        if (readCount != 2 || ending != '\n')
        {
            fprintf(stderr, "Format Error\n");
            return EXIT_FAILURE;
        }

        if (lineNumber == END_LINE_NUMBER)
        {
            return EXIT_SUCCESS;
        }

        if (lineNumber < 0 || lineNumber > linesCount)
        {
            fprintf(stderr, "Out Of Range Error: lineNumber %d out of range [%d, %d]\n", lineNumber, linesCount > 0, linesCount);
            continue;
        }

        PrintLine(src, linesOffsets[lineNumber - 1], linesLengths[lineNumber - 1]);
    } while (1);
}

void PrintLine(char *src, int lineOffset, int lineLength)
{
    for (int j = 0; j < lineLength; j++)
    {
        printf("%c", src[lineOffset + j]);
    }
    printf("\n");
}

void PrintLines(char *src)
{
    fputs(src, stdout);
}

void BindAlarmHandler()
{
    if (signal(SIGALRM, &AlarmHandler) == SIG_ERR)
    {
        perror("Signal set error: ");
        exit(EXIT_FAILURE);
    }
}

void AlarmHandler(int sig)
{
    AlarmTriggered = 1;
}

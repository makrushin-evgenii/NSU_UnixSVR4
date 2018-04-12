#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>

#define TIME_OUT_IN_SECONDS 5

// https://www.opennet.ru/cgi-bin/opennet/man.cgi?topic=select&category=2

int main(int argc, char **argv)
{
        FILE *fd[FOPEN_MAX];
        char buf[BUFSIZ];
        int fd_count = 0;

        for (int i = 1; i < argc; i++)
        {
                fd[fd_count] = fopen(argv[i], "rb");
                if (fd[fd_count] == NULL)
                {
                        perror(argv[i]);
                }
                else if (++fd_count == FOPEN_MAX)
                {
                        break;
                }
        }

        while (fd_count)
        {
                for (int i = 0; i < fd_count; i++)
                {
                        struct timeval timeout;
                        timeout.tv_sec = TIME_OUT_IN_SECONDS;
                        timeout.tv_usec = 0;    

                        fd_set readfds;
                        FD_ZERO(&readfds);
                        FD_SET(fileno(fd[i]), &readfds);
                        if (select(fileno(fd[i]) + 1, &readfds, 0, 0, &timeout) > 0)
                        {
                                if (fgets(buf, BUFSIZ, fd[i]) != 0)
                                {
                                        int bytesRead = strlen(buf);
                                        write(1, buf, bytesRead);
                                }
                                else
                                {
                                        close(fileno(fd[i]));
                                        // тут короче такой трюк:
                                        // i я закрыл и он большей мне не нужен
                                        // вместо него я ставлю последний из еще открытых.
                                        // при этом кол-во открытых сразу уменьшаю уменьшаю
                                        // ну i тоже уменьшаю, чтобы не забыть опросить тот файл
                                        fd[i--] = fd[--fd_count];
                                }
                        }
                }
        }

        return 0;
}

#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int initialize_writing_proc(FILE *reader);
int wait_for_subprocesses();

int main(int argc, char **argv)
{
        FILE *reader = popen("./r", "w");
        if (reader == NULL)
        {
                perror("Making pipe");
                return EXIT_FAILURE;
        }

        initialize_writing_proc(reader);

        pclose(reader);

        wait_for_subprocesses();

        return EXIT_SUCCESS;
}

int initialize_writing_proc(FILE *reader)
{
        int have_input_data = 1;

        int bytes_read;
        char symbol;
        int fputc_status;

        pid_t pid = fork();
        if (pid == -1)
        {
                perror("Making subprocess for writing");
                exit(EXIT_FAILURE);
        }

        if (pid != 0)
        {
                return EXIT_SUCCESS;
        }

        while (have_input_data)
        {
                bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
                if (bytes_read == 0 || symbol == '\04')
                {
                        break;
                }

                if (bytes_read == -1)
                {
                        perror("Reading from stdin");
                        exit(EXIT_FAILURE);
                }

                fputc_status = fputc(symbol, reader);
                if (fputc_status == EOF)
                {
                        perror("Writing to pipe");
                        exit(EXIT_FAILURE);
                }
        }

        pclose(reader);

        exit(EXIT_SUCCESS);
}

int wait_for_subprocesses()
{
        int have_working_subprocess = 1;

        int status;
        int ended_process_id;

        while (have_working_subprocess)
        {
                ended_process_id = wait(&status);

                if (ended_process_id == -1 && errno == EINTR)
                {
                        continue;
                }

                if (ended_process_id == -1 && errno == ECHILD)
                {
                        break;
                }

                if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS || WIFSIGNALED(status))
                {
                        fprintf(stderr, "Subprocess %d ended incorrectly\n", ended_process_id);
                        exit(EXIT_FAILURE);
                }
        }

        return EXIT_SUCCESS;
}

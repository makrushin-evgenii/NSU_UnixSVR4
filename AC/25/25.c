#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define READ 0
#define WRITE 1
#define CHILD_ID 0
#define NO_INPUT_DATA 0


int initialize_writing_proc(int fildes[]);
int initialize_reading_proc(int fildes[]);
void wait_subprocesses();

int main(int argc, char** argv)
{
	int fildes[2];

	int pipe_status = pipe(fildes);
	if (pipe_status == -1) {
		perror("Opening pipe");
		return EXIT_FAILURE;
	}

	initialize_writing_proc(fildes);
	initialize_reading_proc(fildes);

	close(fildes[READ]);
	close(fildes[WRITE]);

	wait_subprocesses();

	return EXIT_SUCCESS;
}


int initialize_writing_proc(int fildes[])
{
	int have_input_data = 1;

	pid_t pid = fork();
	if (pid == -1) {
		perror("Making subprocess for writing");
		exit(EXIT_FAILURE);
	}

	if (pid != 0) {
		return EXIT_SUCCESS;
	}

	close(fildes[READ]);
	
	while (have_input_data) {
		char symbol;
		int bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
		if (bytes_read == NO_INPUT_DATA || symbol == '\4') {
			break;
		}

		if (bytes_read == -1) {
			perror("Reading from stdin");
			exit(EXIT_FAILURE);
		}


		int bytes_wrote = write(fildes[WRITE], &symbol, sizeof(char));
		if (bytes_wrote == -1) {
			perror("Writing to pipe");
			exit(EXIT_FAILURE);
		}  
	}

	close(fildes[WRITE]);
	exit(EXIT_SUCCESS);
}


int initialize_reading_proc(int fildes[])
{
	int have_data_from_pipe = 1;
	int bytes_wrote = 0;

	pid_t pid = fork();
	if (pid == -1) {
		perror("Making subprocess for reading");
		exit(EXIT_FAILURE);
	}

	if (pid != 0) {
		return EXIT_SUCCESS;
	}

	close(fildes[WRITE]);

	while(have_data_from_pipe) {
		char symbol;
		int bytes_read = read(fildes[READ], &symbol, sizeof(char));
		if (bytes_read == NO_INPUT_DATA) {
			break;
		}

		if (bytes_read == -1) {
			perror("Reading from pipe");
			exit(EXIT_FAILURE);
		}

		symbol = (char)toupper(symbol);

		bytes_wrote = write(STDOUT_FILENO, &symbol, sizeof(char));
		if (bytes_wrote == -1) {
			perror("Writing to stdout");
			exit(EXIT_FAILURE);
		} 
	}

	// bytes_wrote = write(STDOUT_FILENO, "\n", sizeof("\n"));
	// if (bytes_wrote == -1) {
	// 	perror("Writing to stdout");
	// 	exit(EXIT_FAILURE);
	// }

	close(fildes[READ]);
	exit(EXIT_SUCCESS);
}

void wait_subprocesses()
{
	int have_working_subprocess = 1;

    while(have_working_subprocess) {
		    int status;
            int ended_process_id = wait(&status);
            if (ended_process_id == -1 && errno == EINTR) {
                    continue;
            }
            if (ended_process_id == -1 && errno == ECHILD) {
                    break;
            }
            if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS || WIFSIGNALED(status)) {
                    fprintf(stderr, "Subprocess %d corrupt an error\n", ended_process_id);
                    exit(EXIT_FAILURE);
            }
    }
}

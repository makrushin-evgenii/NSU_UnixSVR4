#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>      // wait
#include <sys/wait.h>       // wait
#include <unistd.h>			// execl
#include <errno.h>

#define CHILD_ID 0

int main (int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr,"Usage: %s file_name\n", argv[0]);
        return EINVAL;
    }

	pid_t process_id = fork();

	if (process_id == -1) {
		perror("Fork error: ");
		return EXIT_FAILURE;
	}

	if (process_id == CHILD_ID){
		execl("/bin/cat", "cat", argv[1], NULL);
		return EXIT_SUCCESS;
	}
	
    // Возвращает идентификатор дочернего процесса, который завершил выполнение
    // 0 означает ожидание любого дочернего процесса, идентификатор группы процессов которого равен идентификатору текущего процесса
	pid_t ended_process_id = wait(0); 

	if (ended_process_id == -1) {
		perror("TIMEOUT ERROR: ");
		return EXIT_FAILURE;
	}

	printf ("\nPARENT MSG\n");

	return EXIT_SUCCESS;
}

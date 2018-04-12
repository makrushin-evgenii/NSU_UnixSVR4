#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <unistd.h>     // execl
#include <errno.h>

#define CHILD_ID 0

// Функция exec() (execute) загружает и запускает другую программу. 
// Таким образом, новая программа полностью замещает текущий процесс. 
// Новая программа начинает свое выполнение с функции main. 
// Все файлы вызывающей программы остаются открытыми. 
// Они также являются доступными новой программе.

// Суффикс l определяет формат и объем аргументов. 
// l (список) - количество аргументов известно

// fork() creates a new process by duplicating the calling process.  The
//        new process is referred to as the child process.  The calling process
//        is referred to as the parent process.

//        The child process and the parent process run in separate memory
//        spaces.  **At the time of fork() both memory spaces have the same
//        content.**
//              - грубо говоря, каждый процесс имеет тот же самый исходный код, 
//              выполнение которого для нового процесса начинается после вызова fork()

int main (int argc, char* argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s file_name\n", argv[0]);
		return EINVAL;
	}

	pid_t process_id = fork();
	
    // При неудаче родительскому процессу возвращается -1, процесс-потомок не создается
    // возможные значения errno: 
    // EAGAIN - не может скопировать таблицы страниц родителя и выдерить структуру описания потомка
    // ENOMEM - ...
	if (process_id == -1) {
		perror("Fork error: ");
		return EXIT_FAILURE;
	}

    // При успешном завершении родителю возвращается PID процесса-потомка, а процессу-потомку возвращается 0
	if (process_id == CHILD_ID){
		execl("/bin/cat", "cat", argv[1], NULL);
		return EXIT_SUCCESS;
	}

	printf ("\nPARENT MSG\n\n");

	return EXIT_SUCCESS;
}

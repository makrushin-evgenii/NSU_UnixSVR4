#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define FOPEN_FAIL 2

int main(int argc, char *argv[])
{
    // Платиновые ответы:

    // Значение UID ставится в соответствие пользователю в файле /etc/passwd

    // Операционная система различает пользователей именно по UID (а не, например, по логину). 
    // Во многих системах существует возможность создать две записи пользователя с разными логинами, 
    // но одинаковыми UID; в результате оба логина будут иметь одинаковые права, 
    // так как с точки зрения системы они неотличимы (так как имеют одинаковый UID). 

    // Суперпользователь всегда должен иметь UID, равный нулю (0)

    // UIDы с 1 по 100 по соглашению резервируются под системные нужды

    // Почему не могу использовать скрипт для демонстрации:
    // Due to the increased likelihood of security flaws, many operating systems ignore the setuid attribute when applied to executable shell scripts.

    FILE *file;

    if (argc != 2)
	{
    	fprintf(stderr,"Usage: %s [file]\n", argv[0]);
    	return EINVAL;
    }

    for(int i = 0; i < 2; ++i) 
    {
        printf("uid=%d, euid=%d\n", getuid(), geteuid());

        if ((file = fopen(argv[1], "r")) == NULL)
        {
            printf("%d try: access denied\n", i + 1);
            perror(argv[0]);
            return FOPEN_FAIL;
        }
        printf("%d try: accessed\n", i + 1);
        fclose(file);

        if (setuid(geteuid()) < 0)
        {
            fprintf(stderr,"cant change euid\n");
            perror(argv[0]);
            return EINVAL;
        }
    }

	// ERRNO - глобальная переменная. Хранит код последней ошибки 
	// perror - использует системную функцию strerr() чтобы перевести код 
	// ERRNO в соответсвующую строку.
	
	// Как переводится? Смотрим в исходниках:
	// Есть два массива, один интовый - в него залазим и по номеру ERRNO берем число
	// второй масив - массив чаров с кучей \0 и по сути мы просто берем строку от индекса, 
	// который мы получили в прошлом массиве, до следующего сивола \0

	return EXIT_SUCCESS;
}


#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_FILE_NAME "file"

//Open file wrapper
FILE* open_file(const char* file_name);

int main(int argc, char* argv[]) {
    // Платиновые ответы:

    // Значение UID ставится в соответствие пользователю в файле /etc/passwd

    // Операционная система различает пользователей именно по UID (а не,
    // например, по логину).
    // Во многих системах существует возможность создать две записи пользователя
    // с разными логинами,
    // но одинаковыми UID; в результате оба логина будут иметь одинаковые права,
    // так как с точки зрения системы они неотличимы (так как имеют одинаковый
    // UID).

    // Суперпользователь всегда должен иметь UID, равный нулю (0)

    // UIDы с 1 по 100 по соглашению резервируются под системные нужды

    // Почему не могу использовать скрипт для демонстрации:
    // Due to the increased likelihood of security flaws, many operating systems
    // ignore the setuid attribute when applied to executable shell scripts.

    FILE* input_file;

    // If the input file name wasn't entered, use default file name.
    const char* file_name = (argc == 2) ? argv[1] : DEFAULT_FILE_NAME;

    for (int index = 0; index < 2; ++index) {
        fprintf(stdout, "Real UID:%u\nEffective UID:%u\n", getuid(), geteuid());
        input_file = open_file(file_name);
        if (input_file == NULL) {
            return (EXIT_FAILURE);
        }
        if (setuid(getuid()) < 0) {
            fputs("Can't change euid\n", stderr);
            perror(argv[0]);
            return (EXIT_FAILURE);
        }

        if (fclose(input_file) == EOF) {
            fprintf(stderr, "Can't close %s\n", file_name);
            perror(argv[0]);
            return (EXIT_FAILURE);
        }
    }

    // ERRNO - глобальная переменная. Хранит код последней ошибки
    // perror - использует системную функцию strerr() чтобы перевести код
    // ERRNO в соответсвующую строку.

    // Как переводится? Смотрим в исходниках:
    // Есть два массива, один интовый - в него залазим и по номеру ERRNO берем
    // число
    // второй масив - массив чаров с кучей \0 и по сути мы просто берем строку
    // от индекса,
    // который мы получили в прошлом массиве, до следующего сивола \0

    return (EXIT_SUCCESS);
}

FILE* open_file(const char* file_name) {
    FILE* file_for_return = fopen(file_name, "r");
    if (file_for_return == NULL) {
        fprintf(stderr, "Can't open %s file.\n", file_name);
        perror("fopen");
    }
    return file_for_return;
}

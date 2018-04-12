#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

#define LINE_LENGTH_LIMITER 40
#define SYMB_NLINE "\n"
#define SYMB_BELL "\7"
#define SYMB_ERASE "\b \b"

void get_terminal_attrs(int terminal, struct termios *attrs);

void set_terminal_attrs(int terminal, struct termios attrs);

void set_terminal_attrs_to_task_mode(int terminal);

int execute(int term);

int handle_symb(int term, char symb, char *line, int *line_length);

int print_symb(int term, char symb, char *line, int *line_length);

int erase_symb(int term, int *line_length);

int erase_line(int term, int *line_length);

int erase_word(int term, char *line, int *line_length);

int play_sound(int term);

int break_cur_line(int term, char *line, int *line_length);

int print_new_line(int term, int *line_length);

void check_terminal_io_err(ssize_t return_value, char *err_msg,
                           int term, struct termios attrs_backup);

static struct termios attrs_backup;

int main(int argc, char **argv) {
    int terminal = open("/dev/tty", O_RDWR);
    if (terminal == -1) {
        perror("Opening /dev/tty");
        return EXIT_FAILURE;
    }

    if (!isatty(terminal)) {
        perror("/dev/tty is not a terminal");
        return EXIT_FAILURE;
    }

    get_terminal_attrs(terminal, &attrs_backup);
    set_terminal_attrs_to_task_mode(terminal);

    execute(terminal);

    set_terminal_attrs(terminal, attrs_backup);

    return EXIT_SUCCESS;
}

void get_terminal_attrs(int terminal, struct termios *attrs) {
    if (tcgetattr(terminal, attrs) == -1) {
        perror("SAVING ATTRS ERROR");
        exit(EXIT_FAILURE);
    }
}

void set_terminal_attrs(int terminal, struct termios attrs) {
    if (tcsetattr(terminal, TCSANOW, &attrs) == -1) {
        perror("UPDATE ATTRS ERROR");
        exit(EXIT_FAILURE);
    }
}

void set_terminal_attrs_to_task_mode(int terminal) {
    struct termios old_attrs;
    if (tcgetattr(terminal, &old_attrs) == -1) {
        perror("GET ATTRS ERROR");
        exit(EXIT_FAILURE);
    }

    struct termios new_attrs;
    new_attrs = old_attrs;
    new_attrs.c_lflag &= ~(ICANON | ECHO | ISIG);
    new_attrs.c_cc[VMIN] = 1;

    set_terminal_attrs(terminal, new_attrs);
}

int execute(int term) {
    int current_line_length = 0;
    char current_line[LINE_LENGTH_LIMITER + 1] = {0};

    for (;;) {
        char read_symbol;
        check_terminal_io_err((int) read(term, &read_symbol, 1), "READ ERROR", term, attrs_backup);

        if (read_symbol == CEOF && current_line_length == 0) {
            break;
        }

        handle_symb(term, read_symbol, current_line, &current_line_length);
    }

    return EXIT_SUCCESS;
}

int handle_symb(int term, char symb, char *line, int *line_length) {
    switch (symb) {
        case CWERASE:
            erase_word(term, line, line_length);
            break;
        case '\n':
            print_new_line(term, line_length);
            break;
        case CKILL:
            erase_line(term, line_length);
            break;
        case CERASE:
            erase_symb(term, line_length);
            break;
        default:
            if (!isprint(symb)) {
                play_sound(term);
                break;
            }
            print_symb(term, symb, line, line_length);
    }

    return EXIT_SUCCESS;
}

int print_symb(int term, char symb, char *line, int *line_length) {
    line[*line_length] = symb;

    if (*line_length == LINE_LENGTH_LIMITER) {
        break_cur_line(term, line, line_length);
        return EXIT_SUCCESS;
    }

    (*line_length)++;

    check_terminal_io_err(write(term, &symb, 1), "WRITE ERROR", term, attrs_backup);

    return EXIT_SUCCESS;
}

int erase_symb(int term, int *line_length) {
    if (*line_length == 0) {
        return EXIT_SUCCESS;
    }

    check_terminal_io_err(write(term, SYMB_ERASE, strlen(SYMB_ERASE)), "WRITE ERROR", term, attrs_backup);

    (*line_length)--;

    return EXIT_SUCCESS;
}

int erase_line(int term, int *line_length) {
    while (*line_length > 0) {
        erase_symb(term, line_length);
    }

    return EXIT_SUCCESS;
}

int erase_word(int term, char *line, int *line_length) {
    while (isspace(line[(*line_length) - 1]) && *line_length > 0) {
        erase_symb(term, line_length);
    }

    while (!isspace(line[(*line_length) - 1]) && *line_length > 0) {
        erase_symb(term, line_length);
    }

    return EXIT_SUCCESS;
}

int play_sound(int term) {
    check_terminal_io_err(write(term, SYMB_BELL, strlen(SYMB_BELL)), "WRITE ERROR", term, attrs_backup);

    return EXIT_SUCCESS;
}

int break_cur_line(int term, char *line, int *line_length) {
    int index_in_line = *line_length;
    while (!isspace(line[index_in_line]) && index_in_line != 0) {
        index_in_line--;
    }

    if (index_in_line == 0) {
        print_new_line(term, line_length);

        check_terminal_io_err(write(term, &line[LINE_LENGTH_LIMITER], 1), "WRITE ERROR", term, attrs_backup);

        (*line_length)++;
        return EXIT_SUCCESS;
    }

    while (*line_length != index_in_line) {
        erase_symb(term, line_length);
    }

    print_new_line(term, line_length);

    while (index_in_line != LINE_LENGTH_LIMITER) {
        index_in_line++;

        check_terminal_io_err(write(term, &line[index_in_line], 1), "WRITE ERROR", term, attrs_backup);

        line[*line_length] = line[index_in_line];
        (*line_length)++;
    }

    return EXIT_SUCCESS;
}

int print_new_line(int term, int *line_length) {
    *line_length = 0;

    check_terminal_io_err(write(term, SYMB_NLINE, strlen(SYMB_NLINE)), "WRITE ERROR", term, attrs_backup);

    return EXIT_SUCCESS;
}

void check_terminal_io_err(ssize_t return_value, char *err_msg, int term, struct termios attrs_backup) {
    if (return_value == -1) {
        set_terminal_attrs(term, attrs_backup);
        perror(err_msg);
        exit(EXIT_FAILURE);
    }
}
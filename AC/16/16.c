#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include <unistd.h>
#include <termios.h>

#define ANSWER_LENGTH 1

void save_terminal_attrs(int terminal, struct termios *old_attrs);
void set_terminal_attrs(int terminal, struct termios attrs);
void set_terminal_attrs_to_confirm_mode(int terminal);

// Все довольно неплохо разжеванно вот здесь:
// https://ejudge.ru/study/3sem/term.pdf

/**
 * @return: 0 => no, 1 => yes, -1 => undefined 
 */
int get_permission(int terminal, char *quest);

int main(int argc, char** argv)
{
	int terminal = open("/dev/tty", O_RDWR);

	if (terminal == -1) {
		perror("CANT OPEN /dev/tty: ");
		return EXIT_FAILURE;
	}
	
	if (!isatty(terminal)) {
		perror("/dev/tty IS NOT TERMINAL: ");
		return EXIT_FAILURE;
	}

	switch (get_permission(terminal, "[Y/n]")) {
		case 1:
			printf("\nY\n");
			break;
		case 0:
			printf("\nN\n");
			break;
		default:
			printf("\nUNDEFINED\n");
			break;
	}

	return EXIT_SUCCESS;
}

int get_permission(int terminal, char *quest) {
	struct termios old_attrs;
	save_terminal_attrs(terminal, &old_attrs);
	set_terminal_attrs_to_confirm_mode(terminal);
	
	int bytes_wrote = write(terminal, quest, strlen(quest));
	if (bytes_wrote == -1) {
		set_terminal_attrs(terminal, old_attrs);
		perror("WRITE ERROR: ");
		exit(EXIT_FAILURE);
	}

	char answer;
	int bytes_read = read(terminal, &answer, ANSWER_LENGTH);
	if (bytes_read == -1) {
		set_terminal_attrs(terminal, old_attrs);
		perror("READ ERROR: ");
		exit(EXIT_FAILURE);
	}

	set_terminal_attrs(terminal, old_attrs);

	switch (answer) {
		case 'y':
		case 'Y':
		case '\n':
			return 1;
		case 'n':
		case 'N':
			return 0;
		default:
			return -1;
	}
}

void save_terminal_attrs(int terminal, struct termios *attrs) {
	if (tcgetattr(terminal, attrs) == -1) {
		perror("SAVING ATTRS ERROR: ");	
		exit(EXIT_FAILURE);
	}
}

void set_terminal_attrs(int terminal, struct termios attrs) {
	if (tcsetattr(terminal, TCSANOW, &attrs) == -1) {
		perror("UPDATE ATTRS ERROR: ");
		exit(EXIT_FAILURE);
	}
}

void set_terminal_attrs_to_confirm_mode(int terminal) {
	struct termios old_attrs;

	if (tcgetattr(terminal, &old_attrs) == -1) {
		perror("GET ATTRS ERROR: ");	
		exit(EXIT_FAILURE);
	}

	struct termios new_attrs;
	new_attrs = old_attrs;
	new_attrs.c_lflag &= ~(ICANON|ECHO);
	new_attrs.c_cc[VMIN] = ANSWER_LENGTH; 
	
	set_terminal_attrs(terminal, new_attrs);
}

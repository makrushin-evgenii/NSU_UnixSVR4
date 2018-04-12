#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/types.h>
#include <signal.h>

#define BELL_CHARACTER "\7"

void set_sigquit_handler();
void set_sigint_handler();
void restore_terminal_attrs_and_quit(int sig);
void ring_bell(int sig);

void save_terminal_attrs(int terminal, struct termios *attrs);
void set_terminal_attrs(int terminal, struct termios attrs);
void set_terminal_attrs_to_task_mode(int terminal);

int terminal;
struct termios old_attrs;
int bells_count = 0;

int main()
{
    int sigquit_isnt_catched = 1;

    terminal = open("/dev/tty", O_RDWR);
    if (terminal == -1) {
        perror("Opening /dev/tty/");
        return EXIT_FAILURE;
    }

    if (!isatty(terminal)) {
        fprintf(stderr, "/dev/tty isn't a terminal");
        return EXIT_FAILURE;
    }

    set_terminal_attrs_to_task_mode(terminal);
    set_sigint_handler();
    set_sigquit_handler();

    while(sigquit_isnt_catched) {
        pause();
    }

    return EXIT_SUCCESS;
}

void set_sigquit_handler()
{
    if (sigset(SIGQUIT, restore_terminal_attrs_and_quit) == SIG_ERR) {
        set_terminal_attrs(terminal, old_attrs);
        perror("Setting SIGQUIT");
        exit(EXIT_FAILURE);
    }
}

void set_sigint_handler()
{
    if (sigset(SIGINT, ring_bell) == SIG_ERR) {
        set_terminal_attrs(terminal, old_attrs);
        perror("Setting SIGINT");
        exit(EXIT_FAILURE);
    }
}

void restore_terminal_attrs_and_quit(int sig)
{
    set_terminal_attrs(terminal, old_attrs);
    printf("%d times\n", bells_count);
    exit(EXIT_SUCCESS);
}

void ring_bell(int sig)
{
    int bytes_wrote = write(terminal, BELL_CHARACTER, strlen(BELL_CHARACTER));
    if (bytes_wrote == -1) {
        perror("Ringing");
        set_terminal_attrs(terminal, old_attrs);
        exit(EXIT_FAILURE);
    }

    ++bells_count;
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

void set_terminal_attrs_to_task_mode(int terminal) {
    save_terminal_attrs(terminal, &old_attrs);

    struct termios new_attrs;
    new_attrs = old_attrs;
    new_attrs.c_lflag &= ~(ECHO);

    set_terminal_attrs(terminal, new_attrs);
}



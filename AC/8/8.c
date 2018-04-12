#include <sys/types.h>  // flock
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void lockFile(int fd, struct flock* lock);
void unlockFile(int fd, struct flock* lock);

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "usage: %s [file]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    struct flock lock;
    int fd;
    char command[100];

    if ((fd = open(argv[1], O_RDWR)) == -1) {
      	perror(argv[1]);
    	exit(EXIT_FAILURE);
    }

	lockFile(fd, &lock);
 
	sprintf(command, "vi %s\n", argv[1]);
	
	if (system(command) == -1) {
		perror("Can't create child proccess: ");
		exit(EXIT_FAILURE);
	}

	unlockFile(fd, &lock);

	return EXIT_SUCCESS;
}

void lockFile(int fd, struct flock* lock)
{
      lock->l_type = F_RDLCK;
      lock->l_whence = SEEK_SET;
      lock->l_start = 0;
      lock->l_len = 0; 
      if (fcntl(fd, F_SETLK, lock) == -1) {
        perror("Can't lock file now: ");
        exit(EXIT_FAILURE);
      }
}

void unlockFile(int fd, struct flock* lock)
{
    lock->l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, lock) == -1) {
        perror("File unlock error: ");
        exit(EXIT_FAILURE);
    }
}


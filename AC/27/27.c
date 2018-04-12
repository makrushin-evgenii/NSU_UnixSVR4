#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int count_empty_lines(int fd, FILE* pipe_handler);

int main(int argc, char *argv[]) {
	if (argc < 2) {
	fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
    	return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
    	perror("Opening file");
    	return EXIT_FAILURE;
    }

    FILE *handler = popen("wc -l", "w");
    if(handler == NULL) {
    	perror("Making pipe");
    	close(fd);
    	return EXIT_FAILURE;
    }

	count_empty_lines(fd, handler);

	pclose(handler);
	close(fd);

    return EXIT_SUCCESS;
}


int count_empty_lines(int fd, FILE* pipe_handler)
{
	int have_data_from_file = 1;
    char prev_symbol = '\n';

	while (have_data_from_file) 
	{
        char cur_symbol;
        int bytes_read = read(fd, &cur_symbol, 1);
		if (bytes_read == -1) {
			perror("Reading from file");
			close(fd);
			pclose(pipe_handler);
			exit(EXIT_FAILURE);
		}

		if (bytes_read == 0) 
		{
			break;
		}		
 
        if (cur_symbol == '\n' && prev_symbol == '\n') 
		{
             fprintf(pipe_handler, "%c", cur_symbol);
        }

        prev_symbol = cur_symbol;
    }

	return EXIT_SUCCESS;
}

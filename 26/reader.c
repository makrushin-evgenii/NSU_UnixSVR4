#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

int main(int argc, char **argv)
{
	int have_input_data = 1;
	while (have_input_data)
	{
		char symbol;
		int bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
		if (bytes_read == -1)
		{
			perror("Reading from stdin");
			return EXIT_FAILURE;
		}

		if (bytes_read == 0)
		{
			break;
		}

		symbol = (char)toupper(symbol);

		int bytes_wrote = write(STDOUT_FILENO, &symbol, sizeof(char));
		if (bytes_wrote == -1)
		{
			perror("Writing to stdout");
			return EXIT_FAILURE;
		}
	}

	write(STDOUT_FILENO, "\n", sizeof(char));

	return EXIT_SUCCESS;
}

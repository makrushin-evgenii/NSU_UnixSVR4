#include <stdio.h>
#include <stdlib.h>
#include <errno.h>      // error codes
#include <sys/types.h>  // lseek, open
#include <sys/stat.h>   // open
#include <fcntl.h>      // open
#include <unistd.h>     // lseek
#include <signal.h>

#define LINES_COUNT_MAX 100
#define END_LINE_NUMBER 0
#define WAIT_FOR_USER_INPUT_TIME 5

// INFO: использовать STDIN_FILENO, STDOUT_FILENO и STDERR_FILENO вместо магических 0, 1, 2
int BuildLinesOffsetsTable(int fileDescriptor, long *linesOffsets, int *linesLengths);
int RequestingUserInputLoop(int fileDescriptor, int linesCount, long* linesOffsets, int* linesLengths);
void PrintLine(int fileDescriptor, int lineOffset, int linesLength);
void PrintLines(int fileDescriptor);

void BindAlarmHandler();
void AlarmHandler(int sig);

int AlarmTriggered = 0;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr,"usage: %s [file]\n", argv[0]);
        return EINVAL;
    }

    long linesOffsets[LINES_COUNT_MAX + 1];
    int  linesLengths[LINES_COUNT_MAX + 1];

    int fileDescriptor = open(argv[1], O_RDONLY | O_NDELAY);  

    if (fileDescriptor == -1) {
		perror(argv[0]);
		return EIO;
    }

    int linesCount = BuildLinesOffsetsTable(fileDescriptor, linesOffsets, linesLengths);

    BindAlarmHandler();

    return RequestingUserInputLoop(fileDescriptor, linesCount, linesOffsets, linesLengths);
}

/**
 * 
 * return: lines count
 */
int BuildLinesOffsetsTable(int fileDescriptor, long *linesOffsets, int *linesLengths) {
    int currentLine = 1;
    int currentPosInLine = 0;

    linesOffsets[1] = 0L;

    while(1) {
        char currentChar;
        int readReturnCode = read(fileDescriptor, &currentChar, 1);
        
        // On error, -1 is returned, and errno is set appropriately.  In this case, it is left unspecified whether the file position (if any) changes.
        if (readReturnCode == -1) {
            perror("Reading Error: ");
            exit(EXIT_FAILURE);
        }

        // On  success, the number of bytes read is returned (zero indicates end of file), and the file position is advanced by this number.
	    if (readReturnCode == 0) {  // EOF
		    break;
        }
	    
        if (currentLine > LINES_COUNT_MAX + 1) {
            fprintf(stderr, "Error: file have more than %d lines\n", LINES_COUNT_MAX);
            exit(EXIT_FAILURE);
        }

        currentPosInLine++;

        if (currentChar != '\n') {
            continue;
        }

        linesLengths[currentLine] = currentPosInLine;
        ++currentLine;

        // Upon successful completion, lseek() returns the resulting offset
        // location as measured in bytes from the beginning of the file.
        int currentLineOffset = lseek(fileDescriptor, 0L, 1);

        if (currentLineOffset == -1) { // the value (off_t) -1 is returned and errno is set to indicate the error.
                perror("Lseek Error: ");
                exit(EXIT_FAILURE);
        }

        linesOffsets[currentLine] = currentLineOffset;
        currentPosInLine = 0;
    } 

    return currentLine - 1;
}

/**
 * 
 * return: EXIT_SUCCESS, EXIT_FAILURE if errors occured
 */
int RequestingUserInputLoop(int fileDescriptor, int linesCount, long* linesOffsets, int* linesLengths) {
    printf("Lines range: [%d, %d]\n", linesCount > 0, linesCount);
    do {
        printf("$ ");

        int lineNumber;
	    char ending;

        siginterrupt(SIGALRM, 1);
        alarm(WAIT_FOR_USER_INPUT_TIME);

        int readCount = scanf("%d%c", &lineNumber, &ending);
        
        alarm(0);
        
        if (AlarmTriggered != 0) {
            printf("\nTIMEOUT. THE ENTIRE TEXT:\n");
            PrintLines(fileDescriptor);    
        }

        if (readCount != 2 || ending != '\n') {
            fprintf(stderr, "Format Error\n");
            return EXIT_FAILURE;
        }

        if (lineNumber == END_LINE_NUMBER) {
            return EXIT_SUCCESS;
        }

        if (lineNumber < 0 || lineNumber > linesCount) {
            fprintf(stderr, "Out Of Range Error: lineNumber %d out of range [%d, %d]\n", lineNumber, linesCount > 0, linesCount);
            continue;
        }
        
        PrintLine(fileDescriptor, linesOffsets[lineNumber], linesLengths[lineNumber]);
    } while(1);
}

void PrintLine(int fileDescriptor, int lineOffset, int linesLength) {
    lseek(fileDescriptor, lineOffset, 0);   // move to requested line

    for (int i = 0; i < linesLength; ++i) {
    	char currentChar;	
        if (read(fileDescriptor, &currentChar, 1) == -1) {
              perror("Reading Error: ");
              exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, &currentChar, 1) == -1) {
              perror("Writing Error: ");
              exit(EXIT_FAILURE);
        }
    }
}

void PrintLines(int fileDescriptor)
{
	char buffer[128];

    if (lseek(fileDescriptor, 0L, 0) == -1) {
		perror("File move error");
		exit(EXIT_FAILURE);
	}

    while (1) {
		int readReturnCode = read(fileDescriptor, buffer, 10);

		if (readReturnCode == -1) {
			perror("Reading Error: ");
			exit(EXIT_FAILURE);
		}
		
        if (readReturnCode == 0) {
			break;
        }
	
		int writeReturnCode = write(STDOUT_FILENO, buffer, readReturnCode);

        if (writeReturnCode == -1) {
        	perror("Writing Error: ");
        	exit(EXIT_FAILURE);
        }
	}

	exit(EXIT_SUCCESS);
}

void BindAlarmHandler() {
	if (signal(SIGALRM, &AlarmHandler) == SIG_ERR) {
        perror("Signal set error: ");
        exit(EXIT_FAILURE); 
    }
}

void AlarmHandler(int sig)
{
    AlarmTriggered = 1;
}

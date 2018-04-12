#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


typedef struct LinkedList
{
	char *value;
	struct LinkedList *next;
} LinkedList;


LinkedList *MakeNode(char *value)
{
	LinkedList *newNode;
	if ((newNode = (LinkedList*)malloc(sizeof(LinkedList))) == NULL)
	{
		return NULL;
	}

	if((newNode->value = (char*)malloc(strlen(value) + 1)) == NULL)
	{
		return NULL;
	}

	strcpy(newNode->value, value);
	newNode->next = NULL;

	return newNode;
}

/**
	@returns: 0 - OK, -1 - An error has occured during the reading lines (Error NO MEMory)
*/
int ReadLinesToLinkedList(LinkedList *head)
{
	char buffer[BUFSIZ] = "";		// http://all-ht.ru/inf/prog/c/func/setbuf.html
	LinkedList *move = head;

	do {
		printf("> ");
		scanf("%s", buffer);

		if ((move->next = MakeNode(buffer)) == NULL)
		{
			return -1;
		}

		move = move->next;
	} while(buffer[0] != '.');

	return 0;
}

void PrintLines(LinkedList *head){
	LinkedList *move  = NULL;

	for (LinkedList *move = head->next; move != NULL; move = move->next)
	{
		puts(move->value);
	}
}


int main(int argc, char **argv)
{
	LinkedList *head = NULL;

	if ((head = (LinkedList*)malloc(sizeof(LinkedList))) == NULL)
	{
		perror(argv[0]);
		return ENOMEM;		// ENOMEM stands for Error NO MEMory
	}

	head->next = NULL;

	if (ReadLinesToLinkedList(head) == -1)
	{
		perror(argv[0]);
		return ENOMEM;
	}

	PrintLines(head);

	return EXIT_SUCCESS;
}

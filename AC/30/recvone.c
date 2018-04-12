#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <string.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 

#define MSG_TYPE_ALL 0
#define MSG_TYPE_DAT 1
#define MSG_TYPE_END 2
#define MSG_TYPE_ANS 3
#define MSG_SIZE 100

void send_ans(int queue);
void get_msgs(int queue);

struct msg_s {
	long msg_type;
	char msg_text[MSG_SIZE];
};


int main(int argc, char** argv) {
    	int queue = msgget(getuid(), 0);
    	if(queue == -1) {
        	perror("msgget");
        	return EXIT_FAILURE;
    	}

    	printf("Начато чтение из очереди сообещний\n");
    	get_msgs(queue);
    	printf("Все сообщения прочитаны\n");
    	send_ans(queue);
    	printf("Ответ отправлен\n");
            
    	return EXIT_SUCCESS;
}

void send_ans(int queue)
{
        struct msg_s msg = {MSG_TYPE_ANS, ""};
        if(msgsnd(queue, &msg, 0, 0) == -1) {
                perror("msgsnd");
                exit(EXIT_FAILURE);
        }
}

void get_msgs(int queue)
{
        int receiving_messages = 1;

        while (receiving_messages) {
                struct msg_s msg;
                int recv_status = msgrcv(queue, &msg, MSG_SIZE, MSG_TYPE_ALL, 0);
                if (recv_status == -1) {
			perror("msgrcv");
			exit(EXIT_FAILURE);
                }
                if (msg.msg_type == MSG_TYPE_END) {
                        break;
                }

                printf("rtn=%d type=%ld text=%s\n", (int)recv_status, msg.msg_type, msg.msg_text);
        }
}


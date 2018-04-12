#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

#define MSG_SIZE 100
#define MSG_TYPE_DAT 1
#define MSG_TYPE_END 2
#define MSG_TYPE_ANS 3
#define QUEUE_RIGHTS 0600

struct msg_s {
	long msg_type;
	char msg_text[MSG_SIZE];
};

void send_msgs(int queue);
void wait_ans(int queue);
void close_queue(int queue);

int main(int argc, char** argv) {
    	int queue = msgget(getuid(), IPC_CREAT | QUEUE_RIGHTS);
    	if (queue == -1) {
        	perror("msgget");
        	exit(EXIT_FAILURE);
    	}

    	printf("Максимальная длина сообщения %d символов, Ctrl+D - завершение ввода)\n", MSG_SIZE - 1);

	send_msgs(queue);
        printf("Ожидание ответа\n");
	wait_ans (queue);
        printf("Получен ответ\n");
	close_queue(queue);
    	printf("Очередь закрыта\n");
    	return EXIT_SUCCESS;
}


void close_queue(int queue) {
        int close_status = msgctl(queue, IPC_RMID, NULL);
        if(close_status == -1) {
                perror("msgctl");
                exit(EXIT_FAILURE);
        }
}

int null_terminate(char *msg, int length) {
        if (length <= 0) {
                return 0;
        }

        if (msg[length - 1] == '\n') {
                msg[length - 1] = '\0';
                return length;
        }

        if (msg[length - 1] == '\0') {
                return length;
        }

        msg[length++] = '\0';
        return length;
}

void send_msgs(int queue) {
        int have_data_to_send = 1;

        do {
                struct msg_s msg;
                msg.msg_type = MSG_TYPE_DAT;

                int bytes_read = read(STDIN_FILENO, msg.msg_text, MSG_SIZE - 1);
                if (bytes_read == -1) {
                        perror("read");
			close_queue(queue);
                        exit(EXIT_FAILURE);
                }

                int length = null_terminate(msg.msg_text, bytes_read);
                if (length == 0) {
                        msg.msg_type = MSG_TYPE_END;
                        have_data_to_send = 0;
                }

                int send_status = msgsnd(queue, &msg, length, 0);
                if (send_status == -1) {
                        perror("msgsnd");
			close_queue(queue);
                        exit(EXIT_FAILURE);;
                }
        }
        while (have_data_to_send);
}

void wait_ans (int queue) {
        struct msg_s msg;
        int recv_status = msgrcv(queue, &msg, MSG_SIZE, MSG_TYPE_ANS, 0);
        if (recv_status == -1) {
                perror("msgrcv");
		close_queue(queue);
                exit(EXIT_FAILURE);
        }
}

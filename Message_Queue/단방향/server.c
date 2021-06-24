#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 128

typedef struct msgbuf {
    long type;
    char text[SIZE];
} MsgBuf;

int main(void) {
    MsgBuf msg;
    int msgid, len;
    key_t key = 1234;

	
    if((msgid = msgget(key, IPC_CREAT|0644)) < 0) {
        perror("msgget");
        exit(1);
    }
	while(1){
        len = msgrcv(msgid, &msg, SIZE, 0, 0);
        printf("Received Message is: %s", msg.text);
    }
	return 0;
}
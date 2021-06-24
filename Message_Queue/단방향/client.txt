#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>

#define SIZE 128

typedef struct msgbuf {
    long type;
    char text[SIZE];
} MsgBuf;

int main(void) {
    int msgid, len;
    MsgBuf msg;
    key_t key = 1234; 
    msgid = msgget(key, IPC_CREAT|0644); 
    char buffer[SIZE];
	
    if(msgid == -1) {
        perror("msgget");
        exit(1);
    }
    msg.type = 1;
	
	while(1){
	    printf("Input ====> ");
	    fgets(buffer, SIZE, stdin);
	
        strcpy(msg.text, buffer); 
        if(msgsnd(msgid, (void *)&msg, SIZE, IPC_NOWAIT) == -1) { 
            perror("msgsnd");
            exit(1);
        }
	}
    return 0;
}
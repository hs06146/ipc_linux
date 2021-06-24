#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define FIFO "./myfifo"
#define FIFO1 "./myfifo1"
#define MAXMSG 1024

int main(){
	int fd[2], fd1[2];
	pid_t = pid;
	
	pipe(fd), pipe(fd1);
    char msg[MAXMSG], msg1[MAXMSG];

    umask(0000);
//recieve msg & print
    if(access(FIFO,F_OK) == 0){
            unlink(FIFO);
    }

    if(mkfifo(FIFO,0666)!=0){
            perror("mkfifo failure");
            exit(0);
    }
    if((fd[1] = open(FIFO,O_RDWR))<0){
            perror("open error:");
            exit(0);
    }
	
	if((fd1 = open(FIFO1, O_WRONLY)) < 0) {
            perror("open error: ");
            exit(0);
    }
	
    while(1){
            if(read(fd,msg,MAXMSG)<0){
                    perror("fifo read failed");
                    exit(1);
            }
			fgets(msg1, MAXMSG, stdin);
            write(fd1[0], msg1, MAXMSG);
			printf("%s", msg);
    }

    exit(1);
}

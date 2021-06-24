#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/wait.h>

#define MAXLINE 1024

void sig_handler();

char *escapechar = "quit"; // 종료 문자열

int main()
{
    char line[MAXLINE], sendmsg[MAXLINE], recvmsg[MAXLINE+1];
    int n, size, comp, addr_size;
    pid_t fork_ret;
    
    static int s;
    static struct sockaddr_in serv_addr;
    struct sigaction act;
    act.sa_handler = sig_handler;
    
    
    //TCP 연결지향형이고 ipv4 도메인을 위한 소켓 형성
    if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket error:");
        exit(0);
    }
    // 소켓 주소 구조체에 접속할 서버 주소 세팅
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(2345);
     
    sigaction(SIGCHLD, &act, 0);
    // 서버에 연결 요청
    if(connect(s, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connect error:");
        exit(0);
    }
    fork_ret = fork();
    if(fork_ret > 0)
    {
        printf("Input Message : \n");        
		// 부모 프로세스는 키보드 입력을 서버로 송신
        while(fgets(sendmsg, MAXLINE, stdin) != NULL)
        {
            size = strlen(sendmsg);
            if(write(s, sendmsg, strlen(sendmsg)) != size)
            {
                printf("Error in write. \n");
            }
            if(strstr(sendmsg, escapechar) != NULL) // 종료 문자열
            {
                close(s);
                while(1);    //자식프로세서가 죽을때까지 블로킹
            }
        }
    }
    else if(fork_ret == 0)
    {
        // 자식 프로세스는 서버로부터 수신된 메시지를 화면에 출력
        while(1)
        {
            if((size = read(s, recvmsg, MAXLINE)) < 0)
            {
                printf("Error if read. \n");
                close(s);
                return 0;
            }
            recvmsg[size] = '\0';
            if(strstr(recvmsg, escapechar)!=NULL) // 입력 받은 문자열이 'quit'이면 종료
            {
                write(s, escapechar, strlen(escapechar));
                break;
            }
            printf("Message from Server : %s", recvmsg); // 화면 출력
        }
    }
    close(s);
    return 0;
}
void sig_handler()
{
    int state;
    waitpid(-1, &state, WNOHANG);
    exit(0);
    
    return ;
}

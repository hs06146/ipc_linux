#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/wait.h>

#define MAXLINE 512

void sig_handler();// 시그널 처리 함수

char *escapechar = "quit"; // 종료 문자열

int main()
{
    int serv_sock;
    int clnt_sock;
    int clntlen;
    char sendmsg[MAXLINE];
    char recvmsg[MAXLINE];
    int size;
    pid_t fork_ret;
    struct sockaddr_in client_addr;
    struct sockaddr_in serv_addr;
    int state;
    struct sigaction act;
    act.sa_handler = sig_handler;
    
    
    //TCP 연결지향형이고 ipv4 도메인을 위한 소켓 형성
    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) <0)
    {
      printf("Socket error:");
      exit(0);
    }
    // 소켓 주소 구조체에 접속할 서버 주소 세팅
    bzero((char *)&serv_addr, sizeof(serv_addr)); // 소켓 주소 구조체 초기화
    serv_addr.sin_family = AF_INET;  // 타입 : ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // ip주소
    serv_addr.sin_port = htons(2345); // port
    
    
    sigaction(SIGCHLD, &act, 0);
    
    // 소켓에 서버 주소 바인딩
    if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Bind error:");
        exit(0);
    }
    
    listen(serv_sock, 1);
    
    // 클라이언트로부터 요청이 오면 연결 수락
    clntlen = sizeof(client_addr);
    if((clnt_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &clntlen)) <0)
    {
        printf("Accept error:");
        exit(0);
    }
    if((fork_ret = fork()) > 0)
    {
        printf("Input Message : \n");
		// 부모 프로세스는 키보드 입력을 클라이언트로 송신
        while(fgets(sendmsg, MAXLINE, stdin)!=NULL)
        {
            size = strlen(sendmsg);
            if(write(clnt_sock, sendmsg, strlen(sendmsg)) != size)
            {
                printf("Error in write. \n");        
            }
            if(strstr(sendmsg, escapechar) != NULL) // 입력 받은 문자열이 'quit'이면 종료
            {
                close(clnt_sock);
                while(1);    //자식프로세서가 죽을때까지 블로킹 
            }
        }
    }
    else if(fork_ret == 0)
    {
        // 자식 프로세스는 클라이언트로부터 수신된 메시지를 화면에 출력
        while(1)
        {
            if((size = read(clnt_sock, recvmsg, MAXLINE)) < 0)
            {
                printf("Error if read. \n");
                close(clnt_sock);
                exit(0);
            }
            recvmsg[size] = '\0';
            if(strstr(recvmsg, escapechar) != NULL) // 종료 문자열 입력시 처리
            {
                write(clnt_sock, escapechar, strlen(escapechar));
                break;
            }
            printf("Message from Client : %s", recvmsg); // 화면 출력
        }
    }
    close(serv_sock);
    close(clnt_sock);
    
    return 0;
}
void sig_handler()
{
    int state;
    waitpid(-1, &state, WNOHANG);
    exit(0);
    
    return ;
}

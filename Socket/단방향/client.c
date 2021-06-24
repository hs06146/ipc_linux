#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char * message);

#define SIZE 512

int main(){
    int clnt_sock;
    struct sockaddr_in serv_addr;
    char message[SIZE] = {0x00, }; // 초기화
    
    //TCP연결지향형이고 ipv4 도메인을 위한 소켓 생성
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
    if(clnt_sock < 0) {
        error_handling("socket error");
    }
    
    //인자로 받은 서버 주소 정보를 저장
    memset(&serv_addr, 0, sizeof(serv_addr));
    //서버 주소는 ipv4
    serv_addr.sin_family = AF_INET;
    //서버주소 IP저장해주기
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    //서버주소 포트번호 인자로 받은거 저장
    serv_addr.sin_port = htons(1234);
    
    //클라이언트 소켓부분에 서버 연결
    if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        error_handling("connect error");
    }
    
    while(1){
        //연결 되면 데이터 받기
        if(read(clnt_sock, message, sizeof(message)-1) < 0){
        	error_handling("read error");
        }
        //입력 받은 문자열이 'quit'이면 종료
        if(!strncmp(message, "quit", 4)) {
            break;	           
        } else {
            printf("Message from server : %s", message); 
        }
    }
    //소켓 닫기
    close(clnt_sock);
    return 0;
}


void error_handling(char* message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
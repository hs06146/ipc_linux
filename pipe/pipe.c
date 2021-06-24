#include <stdio.h>
#include <unishd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds1[2], fds2[2]; //파이프 생성을 위한 정수형 배열 선언
	char buf[BUF_SIZE];
	pid_t pid; // 자식 프로세스의 pid를 저장할 변수
	
	// 파이프 2개를 생성
	// fds1[0] = 출구, fds1[1] = 입구
	// fds2[0] = 출구, fds2[1] = 입구
	pipe(fds1), pipe(fds2);
	pid = fork();
	while(1){
		if(pid==0){
			printf("To Parent: ");
			fgets(buf, BUF_SIZE, stdin);
			// 자식 프로새스는 fds1[1]로 읽어 들어온 메시지를 보냄
			write(fds1[1], buf, sizeof(buf));
			// fds2[0] 으로부터 값을 읽어 buf에 저장
			read(fds2[0], buf, BUF_SIZE);
			// buf 출력
			printf("%s\n", buf);
		} else {
			// fds1[0] 으로부터 값을 읽어 buf에 저장
			read(fds1[0], buf, BUF_SIZE);
			// buf의 내용 출력
			printf("%s\n", buf);
			printf("To Child: ");
			fgets(buf, BUF_SIZE, stdin);
			//부모 프로새스는 fds2[1]로 읽어 들어온 메시지를 보냄
			write(fds2[1], buf, sizeof(buf));
		}
	}
	return 0;
}
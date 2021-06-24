#include <stdio.h>

#include <string.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <stdlib.h>

#include <sys/shm.h>

#define SIZE 1024

 

main()

{

    struct check_data {

       int check;

       char data[SIZE];

    } *shared_data;
	
	struct check_data1 {

       int check;

       char data[SIZE];

    } *shared_data1;

    int shmid, shmid1;

    void *shmaddr, *shmaddr1;

    char buffer[SIZE], buffer1[SIZE];

	struct shmid_ds shm_stat;
	
	struct shmid_ds shm_stat1;

    // 1234 키의 공유메모리 있으면 접근해서 식별자 얻음

    if((shmid=shmget((key_t)1234, SIZE, IPC_CREAT|0666)) == -1) {

       perror("shmid failed");

       exit(1);

    }

     if((shmid1=shmget((key_t)2345, SIZE, IPC_CREAT|0666)) == -1) {

       perror("shmid failed");

       exit(1);

    }

    // shmid 공유메모리를 호출 프로세스 메모리 영역으로 첨부 */

    if((shmaddr=shmat(shmid, (void *)0, 0)) == (void *)-1) {

       perror("shmat failed");

       exit(1);

    }

	if((shmaddr1=shmat(shmid1, (void *)0, 0)) == (void *)-1) {

       perror("shmat failed");

       exit(1);

    }


    shared_data = (struct check_data *)shmaddr;
	shared_data1 = (struct check_data1 *)shmaddr1;
 


    printf("input data ==> ");

    fgets(buffer, SIZE, stdin);

 

    strncpy(shared_data->data, buffer, SIZE); // 공유메모리에 데이터 쓰기

    shared_data->check = 1;
	   
    while(1) {
			   
		while(shared_data->check && shared_data1->check==0) {

			sleep(1);

			//printf("waiting...\n");

		} 	   
	   
      if(shared_data1->check) {

          printf("data read from shared memory : %s", shared_data1->data);

          sleep(1);

          shared_data1->check = 0;
		  
		  printf("input data ==> ");

		  fgets(buffer1, SIZE, stdin);

 

		  strncpy(shared_data->data, buffer1, SIZE); // 공유메모리에 데이터 쓰기

		  shared_data->check = 1;

    		  

          /* 읽은 데이터가 ‘quit’이면 */

          if(!strncmp(shared_data1->data, "quit", 4)) {

             /* shmid 공유메모리 정보를 얻어 shm_stat에 저장 */

             if(shmctl(shmid1, IPC_STAT, &shm_stat1) == -1) {

                perror("shmctl failed");

                exit(1);

             }

             /* 공유메모리를 이용하는 프로세스가 나 혼자면 while 문 벗어남 */

             if(shm_stat.shm_nattch == 1)

                break;
				
			
          }
		  
		  if(!strncmp(shared_data1->data, "quit", 4)) {
			   if(shmctl(shmid1, IPC_STAT, &shm_stat1) == -1) {
				perror("shmctl failed");
				exit(1);
			 }
			 
			   if(shm_stat1.shm_nattch == 1)
				 break;

		  }

       }	   
			sleep(1);
    }
	

    // 공유메모리를 호출 프로세스의 메모리 영역에서 분리

    if(shmdt(shmaddr) == -1) {

       perror("shmdt failed");

       exit(1);

    }
	
	if(shmdt(shmaddr1) == -1) {
		perror("shmdt failed");
		exit(1);
	}

    // shmid 공유메모리 삭제

    if(shmctl(shmid, IPC_RMID, 0) == -1) {

       perror("shmctl failed");

       exit(1);

    }
	
	if(shmctl(shmid1, IPC_RMID, 0) == -1) {
		perror("shmct1 failed");
		exit(1);
	}

    exit(0);

}
#include "smartsock.h"
#include <unistd.h>

int main(int iRtn, char *cpCMD[])
{
  int iSock;   //소켓 함수의 반환 값을 받는 변수
  int icSock[MAX_USER+1];   //accept의 반환 값을 받는 변수
  struct sockaddr_in stAddr;
  socklen_t uiSockLen=sizeof(struct sockaddr);
  char cBuf[BUF_SIZE];
  const char * cP;
  fd_set fdRead;
  unsigned int uiUser;
  unsigned int uiCnt, uiCnt2;
  int iMSock;  //store greatest number in file descriptors
  char cNick[MAX_USER][NIC_NAME_SIZE];
  char cMSG[MSG_SIZE];
  unsigned short usPORT=PORT;  //PORT == 3456

  if(iRtn == 2)
  {
    iSock  = atoi(cpCMD[1]);
    if(1024 < iSock)
    {
      if(65535 > iSock)
      {
        usPORT = iSock;
        printf("PORT no. %d\n", usPORT);
      }
    }
  }

  iSock = socket(AF_INET, SOCK_STREAM, 0);    //AF_INET = 2, 
  if(0 > iSock)
  {
    perror("socket : ");
    return -1;
  }
  // stAddr구조체에 socket연결을 위한 필수 정보 입력  setting
  bzero(&stAddr, sizeof(stAddr));            //구조체 비우기
  stAddr.sin_family = AF_INET;               //#define AF_INET 2 /* IP protocol family. */
  stAddr.sin_addr.s_addr = inet_addr(IP);    //IP와 PORT값은 헤더파일에 정의되어 있다.
  stAddr.sin_port = htons(usPORT);

  iRtn = bind(iSock, (struct sockaddr *)&stAddr,sizeof(stAddr));
  if(iRtn < 0)
  {
    perror("bind : ");
    close(iSock);

    return -2;
  }
  iRtn = listen(iSock, 5);
  if(iRtn != 0)
  {
    perror("listen : ");
    close(iSock);

    return -3;
  }
  uiUser = 0;
  while(1) 
  {
    // setting fd_set
    FD_ZERO(&fdRead);
    FD_SET(0, &fdRead);
    FD_SET(iSock, &fdRead);
    iMSock = iSock;    

    for(uiCnt = 0; uiCnt < uiUser; ++uiCnt)  //사용자가 접속하면 해당 Fd를 set해주고 가장 높은 fd값을 저장
    {
      FD_SET(icSock[uiCnt], &fdRead);
      //FD_SET(iSock, &fdRead);
      if(iMSock < icSock[uiCnt])
      {
        iMSock = icSock[uiCnt];
      }      
    }
    select((iMSock+1), &fdRead, 0, 0, 0);//select 함수를 사용하여 감시해준다.(입력이 있을때까지 무한대기)
    if( 0 != FD_ISSET(iSock, &fdRead) ) //지정된 소켓 번호가 있으면 대화 소켓 번호를 받아저장하고 user+
    {
      icSock[uiUser] = accept(iSock, (struct sockaddr *)&stAddr, &uiSockLen); //접속자의 정보가 stAddr에 입력된다.
      if(icSock[uiUser] < 0)
      {
        perror("Accept : ");
        continue;
      }
      if(uiUser >= MAX_USER)
      {
        read(icSock[uiUser], cBuf, sizeof(cBuf));
        sprintf(cMSG, "Server is not vacant");
        write(icSock[uiUser], cMSG, sizeof(cMSG));
        write(icSock[uiUser], MSG_END, sizeof(MSG_END));
        close(icSock[uiUser]);

        printf("Server is not vacant [%s]\n", cBuf);
        printf("Client iP :%s\n", inet_ntoa(stAddr.sin_addr));
        continue;
      }
      read(icSock[uiUser], cNick[uiUser], NIC_NAME_SIZE);
      printf("Incoming Client :[%s] \n", cNick[uiUser]);
      printf("Client IP :%s\n", inet_ntoa(stAddr.sin_addr));
      write(icSock[uiUser], "Welcome :)", sizeof("Welcome :)"));
      sprintf(cMSG, "[%s] has logged in.",cNick[uiUser] );
      for(uiCnt2 = 0; uiCnt2 < uiUser; ++uiCnt2)
      {
        write(icSock[uiCnt2] ,cMSG, MSG_SIZE);
      }
      ++uiUser;

    }
    if(0 != FD_ISSET(0, &fdRead))    //서버에서 키보드 입력 받은 내용 클라이언트에게 보내기
    {
      iRtn = read(0, cBuf, BUF_SIZE);
      cBuf[iRtn-1] = 0;  // \n \r 제거
      if(iRtn == 0)
      {
        for(uiCnt = 0; uiCnt < uiUser; ++uiCnt)
        {
          sprintf(cMSG, "Server is aborted");
          write(icSock[uiCnt], cMSG, sizeof(cMSG));
          write(icSock[uiCnt], MSG_END, sizeof(MSG_END));
        }
        break;
      }
      sprintf(cMSG, "[Notice]: [%s]", cBuf);
      for(uiCnt = 0; uiCnt < uiUser; ++uiCnt)
      {
        write(icSock[uiCnt], cMSG, MSG_SIZE);//모든 사용자에게 보낸다.
      }
    }
    for(uiCnt = 0; uiCnt < uiUser; ++uiCnt)
    {  
      if( 0 != FD_ISSET(icSock[uiCnt], &fdRead))
      {
        iRtn = read(icSock[uiCnt], cMSG, MSG_SIZE);
        
        if(
        ( 0 == strncmp(cMSG, MSG_END, sizeof(MSG_END) )) 
        ||
        0 == iRtn)
        {
          sprintf(cMSG, "[%s] has logged out.", cNick[uiCnt]);
          close(icSock[uiCnt]);
          --uiUser;
          icSock[uiCnt] = icSock[uiUser];          
          memcpy(cNick[uiCnt], cNick[uiUser], sizeof(NIC_NAME_SIZE));  
        }
        for(uiCnt2 = 0; uiCnt2 < uiUser; ++uiCnt2)
        {
          write(icSock[uiCnt2] ,cMSG, MSG_SIZE);
        }
      }
    }

  }
  for(uiCnt = 0; uiCnt < uiUser; ++uiCnt)
  {
    close(icSock[uiCnt]);  
  }
  close(iSock);
  return 0;
}


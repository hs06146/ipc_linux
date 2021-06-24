#include "smartsock.h"

int main(int iRtn, char *cpCMD[])
{
  int iFd;
  struct sockaddr_in stAddr;
  int iLen;
  char cBuf[BUF_SIZE];  
  fd_set fdRead;
  char cMSG[MSG_SIZE];
  char cNick[NIC_NAME_SIZE];
  unsigned short usPORT=PORT; //PORT == 3456

  if(iRtn == 2)
  {
    iFd  = atoi(cpCMD[1]);
    if(1024 < iFd)
    {
      if(65535 > iFd)
      {
        usPORT=iFd;        
        printf("PORT no. %d\n", usPORT);
      }
    }
  }


  /*** Nick Name in ***/
  while(1)
  {
    printf("Please Input Nick Name\n");
    fflush(stdout);
    iRtn = read(0, cNick, NIC_NAME_SIZE);
    if(iRtn < 2)//그냥 엔터키를 눌렀을 경우. **ctl + d 누르면 iRtn = 0
      continue;

    cNick[iRtn-1] = 0;
    break;

  }
  /*** socket ***/

  iFd = socket(AF_INET, SOCK_STREAM, 0);
  if(-1 == iFd)
  {
    perror("socket:");
    return 100;
  }

  /*** structure setting ***/
  stAddr.sin_family = AF_INET;
  stAddr.sin_addr.s_addr = inet_addr(IP);
  stAddr.sin_port = htons(usPORT);

  iLen = sizeof(struct sockaddr_in);

  /*** connect ***/
  iRtn = connect(iFd, (struct sockaddr *)&stAddr, iLen);
  if(-1 == iRtn)
  {
    perror("connect:");
    close(iFd);
    return 200;
  }

  write(iFd, cNick, NIC_NAME_SIZE);

  while(1)
  {
    FD_ZERO(&fdRead);  
    FD_SET(0, &fdRead);
    FD_SET(iFd, &fdRead);      
    select(iFd+1,&fdRead, 0, 0, 0);

    if(0 != (FD_ISSET(0, &fdRead) ) )
    {
      iRtn = read(0, cBuf, BUF_SIZE);      
      if(iRtn == 0)
      {
        write(iFd, MSG_END, sizeof(MSG_END));
        break;
      }
      cBuf[iRtn - 1] = 0;
      sprintf(cMSG, "[%s]: %s ", cNick, cBuf);
      write(iFd, cMSG, MSG_SIZE);

      //printf("Me: [%s]\n", cBuf);    
    }
    if(0 != (FD_ISSET(iFd, &fdRead) ))
    {
      iRtn = read(iFd, cMSG, sizeof(cMSG));      
      if(iRtn == 0)
      {
        printf("Server does not respond\n");
        break;
      }
      if( 0 == strncmp(cMSG, MSG_END, sizeof(MSG_END) ))
      {
        break;

      }
      printf("[%s]\n", cMSG);    
    }
  }
}

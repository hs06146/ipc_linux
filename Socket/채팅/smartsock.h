#ifndef __SMARTSOCK_H__
#include <sys/select.h>
#define __SMARTSOCK_H__

#include <stdio.h>
#include <string.h>
#include <strings.h>
// socket & bind & listen & accept & connect
#include <sys/types.h>
#include <sys/socket.h>

// sockaddr_in
#include <netinet/in.h>

// read & write
#include <unistd.h>

// htonl
#include <arpa/inet.h>

// errno, perror
#include <errno.h>

// open
#include <fcntl.h>
#include <sys/stat.h>

//select
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT      3456
#define IP        "127.0.0.1"

#define MAX_USER     5
#define NIC_NAME_SIZE   20
#define NIC_NAME_MSG   (20+2)

#define BUF_SIZE  512              //only message
#define MSG_SIZE  (BUF_SIZE+1+NIC_NAME_MSG)  //message + Nick Name
#define MSG_END    "\x01\x02\x03"        

#endif /* __SMARTSOCK_H__ */
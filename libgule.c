#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "libgule.h"

int lgSendData(char *data, unsigned long size)
{
  return write(lg_sockfd, data, size);
}

int lgSendHeader(unsigned long base, unsigned long size)
{
  struct header hdr;
  
  hdr.base = base;
  hdr.size = size;
  return write(lg_sockfd, &hdr, sizeof(struct header));
}

int lgSendNullHeader()
{
  struct header hdr;

  hdr.base = 0;
  hdr.size = 0;
  return write(lg_sockfd, &hdr, sizeof(struct header));
}

int lgSendStack(unsigned long stackbase, unsigned long entry, 
                unsigned long base,      unsigned long phnum,
                unsigned long pagesz)
{
  struct stack stk; 
  memset(&stk, '\0', sizeof(struct stack));

  stk.argc       = 1;
  stk.argv       = stackbase - 4;//stackbase - (((long)&stk.argvP1S) - ((long)&stk.argvP1)) - 4;

  stk.argvP1     = stackbase - 4;
  stk.argvP2     = 0x0;
  stk.argvP1S    = 0x41424300;
  
  stk.envp       = stackbase - (&stk.argvP1S - &stk.envp - 8);
  
  stk.atEntryVal = entry;
  stk.atEntryId  = 0x9;
  stk.atFlagsVal = 0x0;
  stk.atFlagsId  = 0x8;
  stk.atBaseVal  = base;
  stk.atBaseId   = 0x7;
  stk.atPhnumVal = phnum;
  stk.atPhnumId  = 0x5;
  stk.atPhdrVal  = 0x0;
  stk.atPhdrId   = 0x3;
  stk.atPageszVal= 0x4096;
  stk.atPageszId = 0x6;

  return write(lg_sockfd, &stk, sizeof(struct stack));
}

int lgConnect(char *host, int port)
{
    struct hostent      *he;
    struct sockaddr_in   their_addr;
    
    if((he = gethostbyname(host)) == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }

    if((lg_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port   = htons(port);
    their_addr.sin_addr   = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8);

    if(connect(lg_sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(1);
    }
    
    return lg_sockfd;
}

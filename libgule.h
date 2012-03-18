int lg_sockfd;

struct header
{
  unsigned long base;
  unsigned long size;
};

struct stack
{
  unsigned long argc;
  unsigned long argv;
  unsigned long nul1;
  unsigned long envp;
  unsigned long nul2;
  
  unsigned long atEntryVal;
  unsigned long atEntryId;
  unsigned long atFlagsVal;
  unsigned long atFlagsId;
  unsigned long atBaseVal;
  unsigned long atBaseId;
  unsigned long atPhnumVal;
  unsigned long atPhnumId;
  unsigned long atPhdrVal;
  unsigned long atPhdrId;
  unsigned long atPageszVal;
  unsigned long atPageszId;
  unsigned long atNullVal;
  unsigned long atNullId;

  unsigned long argvP1;
  unsigned long argvP2;
  unsigned long argvP1S;
};

int lgSendData(char *, unsigned long);
int lgSendHeader(unsigned long base,     unsigned long size);
int lgSendNullHeader();
int lgSendStack(unsigned long stackbase, unsigned long entry, 
                unsigned long base,      unsigned long phnum,
                unsigned long pagesz);
int lgConnect(char *host,                int port);

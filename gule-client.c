#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <elf.h>
#include <alloca.h>
#include "libgule.h"

#define VERBOSE

struct stackinfo *sendElf(char *);
struct stackinfo *sendElfData(char *);

struct stackinfo
{
  unsigned long base;
  unsigned long phnum;
  unsigned long entry;
};

#define ALIGN(k,v) (((k)+((v)-1))&(~((v)-1)))

void info()
{
}

void usage(char *program)
{
  printf("usage:\n%s static-elf hostname port\n", program);
  exit(0);
}

void succes()
{
  printf("executed..\n");
  exit(0);
}

void nosuchfile(char *fname)
{
  printf("file: %s not found\n", fname);
  exit(0);
}

int main(int argc, char **argv)
{
  struct stackinfo *si;

  info();

  if(argc != 4)
    usage(argv[0]);
 
  lgConnect(argv[2], atoi(argv[3]));
  
  si = sendElf(argv[1]);

  lgSendNullHeader();

  lgSendStack(0xbfffff00,
              si->entry,
              si->base,
              si->phnum,
              4096);

  #ifdef VERBOSE
  printf("entry -> 0x%08x\n", (unsigned int)si->entry);
  #endif
  
  free(si);

  succes();
  
  return 0;
}

struct stackinfo *sendElf(char *fname)
{
  Elf32_Ehdr      *e;
  Elf32_Phdr      *p, *ptab;
  struct stat      st;
  int              fd, i;
  char            *elf;
  struct stackinfo *si;

  #ifdef VERBOSE  
  printf("sending elf section -> %s\n", fname);
  #endif
  stat(fname, &st); 
  elf = malloc(st.st_size);
  if((fd = open(fname, O_RDONLY)) == -1)
      nosuchfile(fname);
  #ifdef VERBOSE
  printf("  read %i of %i bytes\n", (int)read(fd, elf, st.st_size), (int)st.st_size);
  #endif
  e    = (Elf32_Ehdr *) elf;
  ptab = (Elf32_Phdr *) (elf + e->e_phoff);
  
  si = sendElfData(elf);
  
  for(i = 0, p = ptab; i < e->e_phnum; i++, p++)
  {
    #ifdef VERBOSE
    printf("%i %i\n", PT_INTERP, p->p_type);
    #endif
    if(p->p_type == PT_INTERP)
      free(sendElf(elf + p->p_offset));
  }
  
  free(elf);
  
  return si;
}

struct stackinfo *sendElfData(char *elf)
{
  Elf32_Ehdr       *e;
  Elf32_Phdr       *p, *ptab;
  int               i, tlen = 0;
  char             *ptr, *base;
  struct stackinfo *si = malloc(sizeof(struct stackinfo));

  e    = (Elf32_Ehdr *)elf;
  ptab = (Elf32_Phdr *)(elf + e->e_phoff);
 
  for(i = 0, p = ptab; i < e->e_phnum; i++, p++)
  {
    #ifdef VERBOSE
    printf("found phdr -> type %i\n", p->p_type);
    #endif
    if(p->p_type  != PT_LOAD)
      continue;
    if(p->p_flags == (PF_X|PF_R))
      base = (char *)p->p_vaddr;
    tlen += ALIGN(p->p_memsz, p->p_align);
  }

  ptr = malloc(tlen);

  for(i = 0, p = ptab; i < e->e_phnum; i++, p++)
  { 
    if(p->p_type != PT_LOAD)
      continue;
    memcpy((ptr+((char *)p->p_vaddr - base)), (elf + p->p_offset), p->p_filesz);
  }
  #ifdef VERBOSE
  printf("send elf data: base 0x%08x tlen %i\n", (unsigned int)base, (int)tlen);
  #endif
  lgSendHeader((long)base, tlen);
  lgSendData(ptr, tlen);
  
  si->base  = (long)base;
  si->phnum = e->e_phnum;
  si->entry = e->e_entry;
  
  free(ptr);

  return si;
}

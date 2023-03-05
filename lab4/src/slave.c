#include "../../common/sub.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef DEBUG
static int count = 0;
#endif

static char *raw = 0;

void term() { exit(0); }

void reader() {
  //  signal(SIGUSR1, reader);

  msync(raw, 500 * sizeof(char), MS_SYNC | MS_INVALIDATE);

  struct line line = GetLineI(raw);
  if (line.size == 0) {
    exit(0);
  }
  printf("OK ON WRITE %d,\n", ++count);
  printf("%s\n", line.str);
  ReverseLine(&line);
  printf("%s\n", line.str);
  DestroyLine(&line);
}

int main(int argc, char **argv) {
  char id = argv[1][0];

  int fd_for_input = 0;

  if (id == 1) {
    fd_for_input = open("pipe1.txt", O_RDWR);
  } else {
    fd_for_input = open("pipe2.txt", O_RDWR);
  }

  struct stat sb;
  fstat(fd_for_input, &sb);

  char *file_mem = mmap(NULL, 500 * sizeof(char), PROT_WRITE | PROT_READ,
                        MAP_SHARED, fd_for_input, 0);
  raw = file_mem;

  signal(SIGUSR1, reader);
  signal(SIGUSR2, term);
  while (1)
    ;
  return 0;
}
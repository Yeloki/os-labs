// var 21
#include "../../common/sub.h"

#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef DEBUG
static int file_id = 0;
#endif

int GetFilenameFromSTDI() {

#ifdef DEBUG
  struct line filename;
  if (file_id == 0) {
    InItLine(&filename, "test-first-thread.txt", 21);
  } else if (file_id == 1) {
    InItLine(&filename, "test-second-thread.txt", 22);
  }
  file_id += 1;
#else
  struct line filename = GetLine(false);
#endif
  int descriptor = open(filename.str, O_CREAT | O_WRONLY | O_TRUNC, 0666);
  DestroyLine(&filename);

  if (descriptor == -1) {
    perror("Error while opening file");
  }
  assert(descriptor != -1);
  return descriptor;
}

int main() {
  int file1, file2;
  remove("pipe1.txt");
  remove("pipe2.txt");
  file1 = GetFilenameFromSTDI();
  file2 = GetFilenameFromSTDI();

  int fd_for_input = open("pipe1.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
  struct stat sb;
  ftruncate(fd_for_input, 500 * sizeof(char)); // cut the file
  fstat(fd_for_input, &sb);
  char *file_mem = mmap(NULL, sb.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
                        fd_for_input, 0);

  int fd_for_input2 = open("pipe2.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
  struct stat sb2;
  ftruncate(fd_for_input2, 500 * sizeof(char)); // cut the file
  fstat(fd_for_input2, &sb2);
  char *file_mem2 = mmap(NULL, sb2.st_size, PROT_WRITE | PROT_READ, MAP_SHARED,
                         fd_for_input2, 0);

  pid_t id = fork();
  if (id == 0) {
    char *arg = "\001";
    dup2(file1, fileno(stdout));
    execl("./lab-4-slave", "lab-4-slave", arg, NULL);
    perror("execl");
  }

  pid_t id2 = fork();
  if (id2 == 0) {
    char *arg = "\002";
    dup2(file2, fileno(stdout));
    execl("./lab-4-slave", "lab-4-slave", arg, NULL);
    perror("execl");
  }

  struct line line = GetLine(true);
  bool is_2_thread = true;
  while (line.size > 1) {

    if (!is_2_thread) {
      for (int i = 0; i < line.size; ++i)
        file_mem[i] = line.str[i];
      msync(file_mem, sb.st_size, MS_SYNC | MS_INVALIDATE);
      kill(id, SIGUSR1);

    } else {
      for (int i = 0; i < line.size; ++i)
        file_mem2[i] = line.str[i];
      msync(file_mem2, sb2.st_size, MS_SYNC | MS_INVALIDATE);
      kill(id2, SIGUSR1);
    }

    DestroyLine(&line);
    line = GetLine(true);
    is_2_thread = !is_2_thread;
  }

  kill(id, SIGUSR2);
  kill(id2, SIGUSR2);
  close(file1);
  close(file2);
}
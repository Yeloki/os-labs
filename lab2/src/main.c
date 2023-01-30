// var 21
#define DEBUG
#define _GNU_SOURCE
#include "../../common/sub.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <stdlib.h>

enum {
  READ = 0,
  WRITE = 1
};

int GetFilenameFromSTDI() {
  struct line filename = GetLine();
  int descriptor = open(filename.str, O_CREAT | O_WRONLY | O_TRUNC, 0666);
  if (descriptor == -1) perror("Error while opening file");
  return descriptor;
}

int *CreatePipe() {
  int *pipe_ = malloc(2 * sizeof(int));
  int code = pipe2(pipe_, O_CLOEXEC);
  if (code == -1) perror("Error occurred while creating pipe");
  return pipe_;
}

int main() {
  int file1, file2;
  int *pipe1, *pipe2;

  file1 = GetFilenameFromSTDI();
  file2 = GetFilenameFromSTDI();

  pipe1 = CreatePipe();
  pipe2 = CreatePipe();

  if (file1 == -1 || file2 == -1) return -1;

  pid_t id = fork();
  if (id == 0) {
    close(pipe1[WRITE]);
    dup2(pipe1[READ], fileno(stdin));
    dup2(file1, fileno(stdout));
    execl("./lab-2-slave", "lab-2-slave", NULL);
    perror("execl");
  }

  pid_t id2 = fork();
  if (id2 == 0) {
    close(pipe2[WRITE]);
    dup2(pipe2[READ], fileno(stdin));
    dup2(file2, fileno(stdout));
    execl("./lab-2-slave", "lab-2-slave", NULL);
    perror("execl");
  }

  struct line line = GetLine();
  bool is_2_thread = true;

  while (line.size > 1) {
    line.str = realloc(line.str, ++line.size * sizeof(char));
    line.str[line.size - 1] = '\n';

    if (!is_2_thread) {
      write(pipe1[WRITE], line.str, line.size * sizeof(char));
    } else {
      write(pipe2[WRITE], line.str, line.size * sizeof(char));
    }

    line = GetLine();
    is_2_thread = !is_2_thread;
  }

  close(pipe1[WRITE]);
  close(pipe2[WRITE]);
  wait(NULL);

  close(file1);
  close(file2);
}
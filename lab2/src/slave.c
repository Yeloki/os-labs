#include <stdio.h>
#include<unistd.h>

int main() {

  char c;
  while (read(fileno(stdin), &c, sizeof(char)) != 0) {
    write(fileno(stdout), &c, sizeof(char));
  }
  return 0;
}
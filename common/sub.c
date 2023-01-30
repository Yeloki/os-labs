#include "sub.h"

#include <stdio.h>
#include <malloc.h>

struct line GetLine() {
  struct line line;
  size_t size = 0, capacity = 4;

  char *str = (char *) malloc(sizeof(char) * capacity);

  int tmp_c = getchar();
  while (tmp_c != '\n') {

    str[size++] = (char) tmp_c;
    if (size - 1 >= capacity) {
      capacity = capacity * 2;
      str = realloc(str, capacity);
    }

    tmp_c = getchar();
  }

  str = realloc(str, size + 1);
  line.str = str;
  line.str[size] = '\000';
  line.size = size + 1;
  return line;
}
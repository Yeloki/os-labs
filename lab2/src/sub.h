//
// Created by Yeloki on 16.12.2022.
//

#ifndef LAB_1_LAB2_SRC_SUB_H_
#define LAB_1_LAB2_SRC_SUB_H_

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

struct line {
  size_t size;
  char *str;
};

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

  str = realloc(str, size);
  line.str = str;
  line.size = size;
  return line;
}

#endif //LAB_1_LAB2_SRC_SUB_H_

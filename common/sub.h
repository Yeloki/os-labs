//
// Created by Yeloki on 16.12.2022.
//

#ifndef SUB_H_
#define SUB_H_

#include <stddef.h>

struct line {
  size_t size;
  char *str;
};

struct line GetLine();

#endif //SUB_H_

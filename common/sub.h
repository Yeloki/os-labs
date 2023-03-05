//
// Created by Yeloki on 16.12.2022.
//

#ifndef SUB_H_
#define SUB_H_
#include "defines.h"
#include <stdbool.h>
#include <stddef.h>

struct line {
  size_t size;
  size_t capacity;
  char *str;
};

struct line GetLine(bool with_end);

struct line GetLineI(const char *raw_data);

void ReverseLine(struct line *line);

void TestReverseLine();

void InItLine(struct line *line, const char *init_list, size_t char_count);

void DestroyLine(struct line *line);

#endif // SUB_H_

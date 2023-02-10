#include "sub.h"

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <unistd.h>

struct line GetLine(bool with_end) {
  struct line line;
  line.size = 0;
  line.capacity = 4;
  line.str = (char *) malloc(sizeof(char) * line.capacity);
  char tmp_c;

  read(fileno(stdin), &tmp_c, sizeof(char));
  while (tmp_c != '\n' && tmp_c != 0) {

    line.str[line.size++] = (char) tmp_c;
    if (line.size - 1 >= line.capacity) {
      line.capacity = line.capacity * 2;
      line.str = realloc(line.str, line.capacity);
    }

    read(fileno(stdin), &tmp_c, sizeof(char));
  }
  if (with_end) {
    line.str = realloc(line.str, line.size + 2);
    line.capacity = line.size + 2;
    line.str[line.size] = '\n';
    line.str[line.size + 1] = '\000';
    line.size = line.size + 1;
  } else {
    line.str = realloc(line.str, line.size + 1);
    line.capacity = line.size + 1;
    line.str[line.size] = '\000';
  }
  return line;
}

void InItLine(struct line *line, const char *init_list, size_t char_count) {
  line->str = malloc(char_count + 1);
  line->size = char_count;
  line->capacity = char_count + 1;
  for (int i = 0; i < char_count; ++i) {
    line->str[i] = init_list[i];
  }
  line->str[char_count] = '\000';
}

void ReverseLine(struct line *line) {
  size_t delta_size = line->size;
  for (int i = 0; i < delta_size / 2; ++i) {
    char buf = line->str[delta_size - i - 1];
    line->str[delta_size - i - 1] = line->str[i];
    line->str[i] = buf;
  }
}

void DestroyLine(struct line *line) {
  free(line->str);
  line->capacity = 0;
  line->size = 0;
}

void TestReverseLine() {
  {
    struct line line1, line2;
    InItLine(&line1, "123456", 6);
    InItLine(&line2, "654321", 6);

    ReverseLine(&line1);
    for (int i = 0; i < line1.size; ++i) {
      assert(line1.str[i] == line2.str[i]);
    }
    DestroyLine(&line1);
    DestroyLine(&line2);
    printf("TestReverseLine [ 1  | OK] - 6 chars\n");
  }
  {
    struct line line1, line2;
    InItLine(&line1, "12345", 5);
    InItLine(&line2, "54321", 5);

    ReverseLine(&line1);
    for (int i = 0; i < line1.size; ++i) {
      assert(line1.str[i] == line2.str[i]);
    }
    DestroyLine(&line1);
    DestroyLine(&line2);
    printf("TestReverseLine [ 2  | OK] - 5 chars\n");
  }
  {
    struct line line1, line2;
    InItLine(&line1, "123", 3);
    InItLine(&line2, "321", 3);

    ReverseLine(&line1);
    for (int i = 0; i < line1.size; ++i) {
      assert(line1.str[i] == line2.str[i]);
    }
    DestroyLine(&line1);
    DestroyLine(&line2);
    printf("TestReverseLine [ 3  | OK] - 3 chars\n");
  }
  printf("TestReverseLine [ALL | OK]\n");
}
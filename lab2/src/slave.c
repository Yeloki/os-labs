#include "../../common/sub.h"

#include <stdio.h>
#include<unistd.h>

#ifdef DEBUG
static int count = 0;
#endif

int main() {

  struct line line = GetLine(false);

#ifdef DEBUG
  while (line.size > 0) {
    printf("OK ON WRITE %d,\n", ++count);
    printf("%s\n", line.str);
    ReverseLine(&line);
    printf("%s\n", line.str);
    DestroyLine(&line);
    line = GetLine(false);
  }
  printf("OK ON CLOSE WRITING\n");
#else
  while (line.size > 0) {
    ReverseLine(&line);
    printf("%s\n", line.str);
    DestroyLine(&line);
    line = GetLine(false);
  }
#endif

  return 0;
}
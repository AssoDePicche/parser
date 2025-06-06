#include <stdio.h>

#include "parser.h"

int main(const int argc, const char **argv) {
  if (2 != argc) {
    fprintf(stderr, "Error: You must provide only the grammar file\n");

    return 1;
  }

  FILE *stream = fopen(argv[1], "r");

  if (NULL == stream) {
    fprintf(stderr, "Error: The grammar file '%s' could not be read\n",
            argv[1]);

    return 1;
  }

  printf("%s\n", ParseStream(stream) ? "Correct" : "Incorrect");

  if (EOF == fclose(stream)) {
    fprintf(stderr, "Error: I/O error when closing the '%s' stream\n", argv[1]);

    return 1;
  }

  return 0;
}

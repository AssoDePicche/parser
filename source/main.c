#include <stdio.h>
#include <stdlib.h>

#include "parser.h"

int main(int argc, char **argv) {
  if (2 != argc) {
    fprintf(stderr, "%s", "You must provide the grammar file\n");

    return 1;
  }

  FILE *stream = fopen(argv[1], "r");

  if (NULL == stream) {
    perror("The grammar file could not be read");

    return 1;
  }

  Parser *parser = CreateParserFromStream(stream);

  free(parser);

  if (EOF == fclose(stream)) {
    perror("I/O error when closing the stream");

    return 1;
  }

  return 0;
}

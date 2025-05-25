#include "parser.h"

#include <stdlib.h>
#include <string.h>

Parser *CreateParserFromStream(FILE *stream) {
  Parser *this = (Parser *)malloc(sizeof(Parser));

  this->root = '\0';

  this->exprSize = 0;

  fseek(stream, 0, SEEK_END);

  unsigned bufferSize = ftell(stream);

  rewind(stream);

  char *buffer = (char *)malloc(bufferSize + 1);

  fread(buffer, 1, bufferSize, stream);

  buffer[bufferSize - 1] = '\0';

  unsigned i = 0;

  unsigned j = 0;

  while ('\0' != buffer[i]) {
    if (' ' != buffer[i]) {
      buffer[j++] = buffer[i];
    }

    ++i;
  }

  buffer[j] = '\0';

  buffer = strtok(buffer, "-");

  do {
    char *token = strchr(buffer, '>');

    *token = '\0';

    if ('\0' == this->root) {
      this->root = *(token - 1);
    }

    Expr expr;

    expr.root = *(token - 1);

    strcpy(expr.symbols, token + 1);

    this->expr[this->exprSize] = expr;

    ++this->exprSize;
  } while ((buffer = strtok(NULL, "-")));

  return this;
}

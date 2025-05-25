#include "parser.h"

#include <stdlib.h>
#include <string.h>

#define CONCATSYMBOL "-"
#define NULLSYMBOL '\0'
#define SPACESYMBOL ' '
#define SPLITSYMBOL '>'

Parser *CreateParserFromStream(FILE *stream) {
  Parser *this = (Parser *)malloc(sizeof(Parser));

  this->root = NULLSYMBOL;

  this->exprSize = 0;

  fseek(stream, 0, SEEK_END);

  unsigned size = ftell(stream);

  rewind(stream);

  char *buffer = (char *)malloc(size + 1);

  fread(buffer, 1, size, stream);

  buffer[size - 1] = NULLSYMBOL;

  unsigned i = 0;

  unsigned j = 0;

  while (NULLSYMBOL != buffer[i]) {
    if (SPACESYMBOL != buffer[i]) {
      buffer[j++] = buffer[i];
    }

    ++i;
  }

  buffer[j] = NULLSYMBOL;

  buffer = strtok(buffer, CONCATSYMBOL);

  do {
    Symbol *symbol = strchr(buffer, SPLITSYMBOL);

    *symbol = NULLSYMBOL;

    Symbol root = *(symbol - 1);

    Symbol *symbols = symbol + 1;

    if (NULLSYMBOL == this->root) {
      this->root = root;
    }

    this->expr[this->exprSize] = (Expr){
        .root = root,
    };

    strcpy(this->expr[this->exprSize].symbols, symbols);

    ++this->exprSize;
  } while ((buffer = strtok(NULL, CONCATSYMBOL)));

  return this;
}

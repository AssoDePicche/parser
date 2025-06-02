#include "parser.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define NULLSYMBOL '\0'
#define SPACESYMBOL ' '
#define CONCATSYMBOL "-"
#define SPLITSYMBOL '>'
#define NILSYMBOL '$'

typedef struct {
  char root;
  char *symbols;
} Expr;

typedef struct {
  unsigned size;
  unsigned capacity;
  char *buffer;
} Array;

static Array *CreateArray(const unsigned capacity) {
  Array *this = (Array *)malloc(sizeof(Array));

  this->size = 0;

  this->capacity = capacity;

  this->buffer = (char *)calloc(sizeof(char), capacity);

  return this;
}

static void Push(Array *this, const char value) {
  if (this->size == this->capacity) {
    return;
  }

  for (unsigned index = 0; index < this->size; ++index) {
    if (value == this->buffer[index]) {
      return;
    }
  }

  this->buffer[this->size] = value;

  ++this->size;
}

bool ParseStream(FILE *stream) {
  fseek(stream, 0, SEEK_END);

  const unsigned streamSize = ftell(stream);

  rewind(stream);

  char *buffer = (char *)malloc(streamSize + 1);

  fread(buffer, 1, streamSize, stream);

  buffer[streamSize - 1] = NULLSYMBOL;

  unsigned current = 0;

  unsigned previous = 0;

  while (NULLSYMBOL != buffer[current]) {
    if (SPACESYMBOL != buffer[current]) {
      buffer[previous++] = buffer[current];
    }

    ++current;
  }

  buffer[previous] = NULLSYMBOL;

  const char ROOT = buffer[0];

  if (NULL == strchr(buffer, NILSYMBOL)) {
    return false;
  }

  buffer = strtok(buffer, CONCATSYMBOL);

  Expr exprBuffer[1024];

  unsigned exprSize = 0;

  Array *nonterminal = CreateArray(25);

  Array *terminal = CreateArray(25);

  do {
    char *expr = strchr(buffer, SPLITSYMBOL);

    *expr = NULLSYMBOL;

    char root = *(expr - 1);

    if (!isupper(root)) {
      return false;
    }

    Push(nonterminal, root);

    char *symbols = expr + 1;

    exprBuffer[exprSize] = (Expr){
        .root = root,
    };

    exprBuffer[exprSize].symbols = (char *)malloc(strlen(symbols));

    strcpy(exprBuffer[exprSize].symbols, symbols);

    ++exprSize;

    for (unsigned index = 0; index < strlen(symbols); ++index) {
      if (isupper(symbols[index]) || NILSYMBOL == symbols[index]) {
        continue;
      }

      Push(terminal, symbols[index]);
    }
  } while ((buffer = strtok(NULL, CONCATSYMBOL)));

  printf("G = ({");

  for (unsigned index = 0; index < nonterminal->size - 1; ++index) {
    printf("%c,", nonterminal->buffer[index]);
  }

  printf("%c}, {", nonterminal->buffer[nonterminal->size - 1]);

  for (unsigned index = 0; index < terminal->size - 1; ++index) {
    printf("%c,", terminal->buffer[index]);
  }

  printf("%c}, P, %c)\n", terminal->buffer[terminal->size - 1], ROOT);

  printf("P = {");

  for (unsigned index = 0; index < exprSize - 1; ++index) {
    printf("%c -> %s, ", exprBuffer[index].root, exprBuffer[index].symbols);
  }

  printf("%c -> %s}\n", exprBuffer[exprSize - 1].root,
         exprBuffer[exprSize - 1].symbols);

  return true;
}

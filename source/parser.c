#include "parser.h"

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

static bool isSpecialChar(const char character) {
  return 36 == character || 45 == character || 62 == character;
}

static bool isNonterminal(const char character) {
  return 65 <= character && character <= 90;
}

static bool isTerminal(const char character) {
  return 97 <= character && character <= 122;
}

static bool inTheAlphabet(const char character) {
  return isNonterminal(character) || isTerminal(character) ||
         isSpecialChar(character);
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

  if (NULL == strchr(buffer, NILSYMBOL)) {
    fprintf(stderr, "Error: '$' not found on stream\n");

    return false;
  }

  char ROOT = NULLSYMBOL;

  for (unsigned index = 1; index < strlen(buffer) - 1; ++index) {
    const char previous = buffer[index - 1];

    const char current = buffer[index];

    const char next = buffer[index + 1];

    if (!inTheAlphabet(previous)) {
      fprintf(stderr, "Error: '%c' character not in the alphabet\n", previous);

      return false;
    }

    if (!inTheAlphabet(current)) {
      fprintf(stderr, "Error: '%c' character not in the alphabet\n", current);

      return false;
    }

    if (!inTheAlphabet(next)) {
      fprintf(stderr, "Error: '%c' character not in the alphabet\n", next);

      return false;
    }

    if (NULLSYMBOL == ROOT && isNonterminal(previous)) {
      ROOT = previous;
    }

    if (!isSpecialChar(current) ||
        (SPLITSYMBOL == current && NILSYMBOL == next)) {
      continue;
    }

    if (isSpecialChar(previous) || isSpecialChar(next)) {
      fprintf(
          stderr,
          "Error: Cannot have '%c' or '%c' characters before or after '%c'\n",
          buffer[index - 1], buffer[index + 1], buffer[index]);

      return false;
    }
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

    if (!isNonterminal(root)) {
      fprintf(stderr, "Error: Root must be an upper case character\n");

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
      if (isNonterminal(symbols[index]) || NILSYMBOL == symbols[index]) {
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

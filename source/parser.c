#include "parser.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NULLSYMBOL '\0'
#define SPACESYMBOL ' '
#define CONCATSYMBOL "-"
#define SPLITSYMBOL '>'
#define NILSYMBOL '$'
#define SPLIT(buffer) strtok(buffer, CONCATSYMBOL)

typedef struct {
  char *root;
  char *symbols;
} Expr;

typedef struct {
  uint64_t size;
  uint64_t capacity;
  char *buffer;
} Array;

static Array *CreateArray(const uint64_t capacity) {
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

  for (uint64_t index = 0; index < this->size; ++index) {
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

static char *StreamAsString(FILE *stream) {
  fseek(stream, 0, SEEK_END);

  const uint64_t streamSize = ftell(stream);

  rewind(stream);

  char *buffer = (char *)malloc(streamSize + 1);

  fread(buffer, 1, streamSize, stream);

  buffer[streamSize - 1] = NULLSYMBOL;

  uint64_t current = 0;

  uint64_t previous = 0;

  while (NULLSYMBOL != buffer[current]) {
    if (SPACESYMBOL != buffer[current]) {
      buffer[previous++] = buffer[current];
    }

    ++current;
  }

  buffer[previous] = NULLSYMBOL;

  return buffer;
}

bool ParseStream(FILE *stream) {
  char *buffer = StreamAsString(stream);

  if (NULL == strchr(buffer, NILSYMBOL)) {
    fprintf(stderr, "Error: '$' not found on stream\n");

    return false;
  }

  char ROOT = NULLSYMBOL;

  Array *nonterminal = CreateArray(25);

  Array *terminal = CreateArray(25);

  const uint64_t BUFFERSIZE = strlen(buffer);

  for (uint64_t index = 0; index < BUFFERSIZE; ++index) {
    const char current = buffer[index];

    const char previous = index > 0 ? buffer[index - 1] : current;

    const char next = index < BUFFERSIZE - 1 ? buffer[index + 1] : current;

    if (!inTheAlphabet(current)) {
      fprintf(stderr, "Error: '%c' character not in the alphabet\n", current);

      return false;
    }

    if (isNonterminal(current)) {
      Push(nonterminal, current);
    }

    if (isTerminal(current)) {
      Push(terminal, current);
    }

    if (NULLSYMBOL == ROOT && isNonterminal(current)) {
      ROOT = current;
    }

    if (BUFFERSIZE - 1 == index) {
      break;
    }

    if (!isSpecialChar(current) ||
        (SPLITSYMBOL == current && NILSYMBOL == next)) {
      continue;
    }

    if (isSpecialChar(previous) || isSpecialChar(next)) {
      fprintf(
          stderr,
          "Error: Cannot have '%c' or '%c' characters before or after '%c'\n",
          previous, next, current);

      return false;
    }
  }

  Expr exprBuffer[1024];

  uint64_t exprSize = 0;

  for (buffer = SPLIT(buffer); NULL != buffer; buffer = SPLIT(NULL)) {
    char *expression = strchr(buffer, SPLITSYMBOL);

    if (NULL == expression) {
      fprintf(stderr, "Error: Cannot parse expression\n");
    }

    *expression = NULLSYMBOL;

    uint64_t nonterminalSymbols = 0;

    for (uint64_t index = 0; index < strlen(buffer); ++index) {
      if (isNonterminal(buffer[index])) {
        ++nonterminalSymbols;
      }
    }

    if (0 == nonterminalSymbols) {
      fprintf(stderr,
              "Error: Root must contains at least one nonterminal character\n");

      return false;
    }

    exprBuffer[exprSize].root = strdup(buffer);

    exprBuffer[exprSize].symbols = strdup(expression + 1);

    ++exprSize;
  }

  printf("G = ({");

  for (uint64_t index = 0; index < nonterminal->size - 1; ++index) {
    printf("%c,", nonterminal->buffer[index]);
  }

  printf("%c}, {", nonterminal->buffer[nonterminal->size - 1]);

  for (uint64_t index = 0; index < terminal->size; ++index) {
    if (terminal->size - 1 != index) {
      printf("%c,", terminal->buffer[index]);

      continue;
    }

    printf("%c}, P, %c)\nP = {", terminal->buffer[index], ROOT);
  }

  for (uint64_t index = 0; index < exprSize; ++index) {
    if (exprSize - 1 != index) {
      printf("%s -> %s, ", exprBuffer[index].root, exprBuffer[index].symbols);

      continue;
    }

    printf("%s -> %s}\n", exprBuffer[index].root, exprBuffer[index].symbols);
  }

  return true;
}

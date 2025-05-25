#ifndef __PARSER__
#define __PARSER__

#include <stdio.h>

#define MAX_EXPR 100
#define MAX_SYMBOLS 100

typedef struct {
  char root;
  char symbols[MAX_SYMBOLS];
} Expr;

typedef struct {
  Expr expr[MAX_EXPR];
  unsigned exprSize;
  char root;
} Parser;

Parser *CreateParserFromStream(FILE *);

#endif

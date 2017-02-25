#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#define BUFFSIZE 500

char * rettoshell = " && sysscript";

enum token_type {
  call_op = 1,
  sys_call = 2,
  bash_op = 3,
  none = 4
};

struct token {
  char * token;
  enum token_type type;
  struct token * next;
};

int get_token(char ** input, char ** token)
{
  if (**input == '\n') {
    return 0;
  }
  int i = 0;
  char * copyto = (char*)malloc(BUFFSIZE*sizeof(char));
  if (**input == '$') {
    while ((int)**input > 31) {
      copyto[i++] = **input;
      *input += sizeof(char);
    }
    for (int j = 0; j < strlen(rettoshell); j++) {
      copyto[i++] = rettoshell[j];
    }
  } else {
    while ((int)**input > 32) {
      copyto[i++] = **input;
      *input += sizeof(char);
    }
  }
  copyto[++i] = '\0';
  *token = copyto;
  if (**input == ' ')
    *input += sizeof(char);
  return 1;
}

int lex_analyze(char input[], struct token * tokens)
{
  int error = 0;
  int call;

  struct token * currtoken = tokens;
  char * tokstr = (char*)malloc(BUFFSIZE*sizeof(char));
  while (get_token(&input, &tokstr) && error != -1) {

    currtoken->token = tokstr;
    if (strcmp(currtoken->token, "call") == 0) {
      currtoken->type = call_op;
      continue;
    }
    if (currtoken->token[0] == '0' && currtoken->token[1] == 'x') {
      currtoken->type = sys_call;
      continue;
    }
    if (currtoken->token[0] == '$') {
      currtoken->type = bash_op;
      continue;
    }
    printf("Invalid token\n");
    error = -1;

    struct token * newtok = (struct token *)malloc(sizeof(struct token));
    currtoken->next = newtok;
    currtoken = newtok;
  }

  // cut off the tail
  currtoken->next = NULL;

  return error;
}

int syntax_analyze(struct token * tokens)
{
  struct token * currtok = tokens;
  enum token_type first_tok = currtok->type;
  enum token_type lookingfor = none;
  if (first_tok == call_op) {
    lookingfor = sys_call;
  }
  if (first_tok == bash_op) {
    lookingfor = none;
  }
  if (currtok->next == NULL) return 0;
  currtok = currtok->next;
  while (currtok->type == lookingfor) {
    if (currtok->next == NULL) return 0;
    currtok = currtok->next;
  }
  printf("Invalid syntax\n");
  return -1;
}

void parse(struct token * tokens) {
  struct token * currtok = tokens;
  if (currtok->type == bash_op) {
    execl("/bin/bash", "bash", "-c", currtok->token + sizeof(char));
  } else if (currtok->type == call_op) {
    printf("a\n");
    currtok = currtok->next;
    execl("/bin/bash", "bash", "-c", "echo > int.s && sysscript");
    printf("b\n");
    FILE * f = fopen("int.s", "w");
    printf("c\n");
    fprintf(f, "__start:\n\n\tint\t%s\n\t", currtok->token);
    printf("d\n");
    fclose(f);
    printf("e\n");
    execl("/bin/bash", "bash", "-c", "gcc -s int.s -o int && ./int && sysscript");
    printf("f\n");
  }
}

int main(void)
{
  for(;;)
  {
    int error;

    // The input is a line of code... this is an interpreted lang so each line will be analyzed, parsed, and run one at a time
    char * input = (char*)malloc(BUFFSIZE*sizeof(char));
    printf(">>> "); fflush(stdout);
    fgets(input, BUFFSIZE, stdin);

    struct token * tokens = (struct token *)malloc(sizeof(struct token));
    error = lex_analyze(input, tokens);
    if (error == -1) {
      continue;
    }

    error = syntax_analyze(tokens);
    if (error == -1) {
      continue;
    }

    parse(tokens);

  }
  return 0;
}

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>

#define TOK_DELIM " \t\r\n"
#define RED  "\x1B[31m"
#define BLUE  "\x1B[34m"
#define RESET "\e[0m"

char *read_line();
char **split_line(char *);
int gears_exit(char **);
int gears_execute(char **);

int gears_execute(char **args) {
  pid_t cpid;
  int status;

  if (strcmp(args[0], "exit") == 0) {
    return gears_exit(args);
  }

  cpid = fork();

  if (cpid == 0) {
    if (execvp(args[0], args) < 0)
      printf("gears: command not found: %s\n", args[0]);
    exit(EXIT_FAILURE);

  } else if (cpid < 0)
    printf(RED "Error forking"
      RESET "\n");
  else {
    waitpid(cpid, & status, WUNTRACED);
  }
  return 1;
}

int gears_exit(char **args) {
  return 0;
}

char **split_line(char * line) {
  int buffsize = BUFSIZ, position = 0;
  char **tokens = malloc(buffsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "%gears: Allocation error%s\n", RED, RESET);
    exit(EXIT_FAILURE);
  }
  token = strtok(line, TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= buffsize) {
      buffsize += BUFSIZ;
      tokens = realloc(tokens, buffsize * sizeof(char * ));

      if (!tokens) {
        fprintf(stderr, "%gears: Allocation error%s\n", RED, RESET);
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, TOK_DELIM);
  }
  tokens[position] = NULL;

  return tokens;
}

char *read_line() {
  int buffsize = 1024;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "%sgears: Allocation error%s\n", RED, RESET);
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    if (position >= buffsize) {
      buffsize += 1024;
      buffer = realloc(buffer, buffsize);

      if (!buffer) {
        fprintf(stderr, "gears: Allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

void loop() {
  char *line;
  char **args;
  int status = 1;

  do {
    printf("%sμ ~>", BLUE);
    line = read_line();
    args = split_line(line);
    status = gears_execute(args);
    free(line);
    free(args);
  } while (status);
}

int main() {
  loop();
  return 0;
}

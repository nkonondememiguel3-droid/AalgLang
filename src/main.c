#include <stdio.h>
#include <stdlib.h>

void run_file(const char *script);
void run_prompt();

int main(int argc, char **argv) {

  if (argc > 2) {
    fprintf(stderr, "Usage: alglang <script.al>\n");
    exit(EXIT_FAILURE);
  }

  if (argc == 2) run_file(argv[1]);
  else run_prompt();

  return 0;
}

void run_file(const char *script) {
  printf("Execute script : %s\n", script);
}
void run_prompt() {
  printf("Run in REPL\n");
}

#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc != 2 || argv[1][0] == '\0') {
    fprintf(2, "Usage: sleep ticks\n");
    exit(1);
  }

  for (int i = 0; argv[1][i] != '\0'; i++) {
    if (argv[1][i] < '0' || argv[1][i] > '9') {
      fprintf(2, "sleep: invalid number of ticks\n");
      exit(1);
    }
  }

  char *number = argv[1];
  while (*number == '0')
    number++;
  if (strlen(number) > 10 ||
      (strlen(number) == 10 && strcmp(number, "2147483647") > 0)) {
    fprintf(2, "sleep: tick count is too large\n");
    exit(1);
  }

  int ticks = atoi(number);
  if (pause(ticks) < 0) {
    fprintf(2, "sleep: pause failed\n");
    exit(1);
  }
  exit(0);
}

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
sixfive(int fd)
{
  int number = 0;
  int has_digits = 0;
  int valid = 1;
  int too_large = 0;
  char c;

  while (1) {
    int n = read(fd, &c, 1);
    if (n < 0) {
      fprintf(2, "sixfive: read failed\n");
      return -1;
    }

    if (n == 0 || strchr(" -\r\t\n./,", c) != 0) {
      if (has_digits && valid) {
        if (too_large) {
          fprintf(2, "sixfive: number is too large\n");
          return -1;
        }
        if (number % 5 == 0 || number % 6 == 0)
          printf("%d\n", number);
      }
      number = 0;
      has_digits = 0;
      valid = 1;
      too_large = 0;
      if (n == 0)
        break;
    } else if (c >= '0' && c <= '9') {
      has_digits = 1;
      if (valid && !too_large) {
        int digit = c - '0';
        // Check that the next digit fits in an int.
        if (number > 214748364 || (number == 214748364 && digit > 7))
          too_large = 1;
        else
          number = number * 10 + digit;
      }
    } else {
      valid = 0;
    }
  }
  return 0;
}

int
main(int argc, char *argv[])
{
  if (argc == 1)
    exit(sixfive(0) < 0);

  int failed = 0;
  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    if (fd < 0) {
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      failed = 1;
      continue;
    }
    if (sixfive(fd) < 0)
      failed = 1;
    if (close(fd) < 0) {
      fprintf(2, "sixfive: close failed\n");
      failed = 1;
    }
  }
  exit(failed);
}

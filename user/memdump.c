#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{
  for (int j = 0; fmt[j] != '\0'; j++) {
    char format = fmt[j];
    if (format == 'S') {
      while (len > 0 && *data != '\0') {
        printf("%c", *data);
        data++;
        len--;
      }
      printf("\n");
      return;
    }
    int required;

    if (format == 'i') {
      required = 4;
    } else if (format == 'p' || format == 's') {
      required = 8;
    } else if (format == 'h') {
      required = 2;
    } else if (format == 'c') {
      required = 1;
    } else {
      printf("memdump: not enough data for '%c'\n", format);
      return;
    }
    if (len < required) {
      printf("memdump: unsupported format '%c'\n", format);
      return;
    }
    if (format == 'i') {
      int value;
      memmove(&value, data, required);
      printf("%d\n", value);
    } else if (format == 'p') {
      uint64 value;
      memmove(&value, data, required);
      printf("%lx \n", value);
    } else if (format == 'h') {
      short value;
      memmove(&value, data, required);
      printf("%d\n", (int)value);
    } else if (format == 'c') {
      printf("%c\n", *data);
    } else if (format == 's') {
      char *string;
      memmove(&string, data, required);
      printf("%s\n", string);
    }

    data += required;
    len -= required;
  }
}

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
  int index = 0;
  int offset = 0;

  while (fmt[index] != '\0')
  {
    char character = fmt[index];

    switch (character)
    {
    case 'i':
    {
      // Kiem tra du 4 byte
      if (offset > len || len - offset < 4)
      {
        printf("memdump: not enough data for '%c'\n", character);
        return;
      }

      int value;
      memmove(&value, data + offset, sizeof(int));

      printf("%d\n", value);
      offset += 4;
      break;
    }

    case 'h':
    {
      // Kiem tra du 2 byte
      if (offset > len || len - offset < 2)
      {
        printf("memdump: not enough data for '%c'\n", character);
        return;
      }

      short value;
      memmove(&value, data + offset, sizeof(short));

      printf("%d\n", value);
      offset += 2;
      break;
    }

    case 'c':
    {
      // Kiem tra du 1 byte
      if (offset > len || len - offset < 1)
      {
        printf("memdump: not enough data for '%c'\n", character);
        return;
      }

      char val;
      memmove(&val, data + offset, sizeof(char));

      printf("%c\n", val);
      offset += 1;
      break;
    }

    case 'p':
    {
      if (offset > len || len - offset < 8)
      {
        printf("memdump: not enough data for '%c'\n", character);
        return;
      }

      uint64 value;
      memmove(&value, data + offset, sizeof(uint64));

      printf("%lx\n", value);
      offset += 8;
      break;
    }

    case 's':
    {
      if (offset > len || len - offset < 8)
      {
        printf("memdump: not enough data for '%c'\n", character);
        return;
      }

      uint64 address;
      memmove(&address, data + offset, sizeof(address));
      printf("%s\n", (char *)address);
      offset += 8;
      break;
    }

    case 'S':
    {
      while (offset < len && data[offset] != '\0')
      {
        printf("%c", data[offset]);
        offset++;
      }
      printf("\n");
      break;
    }

    default:
      printf("memdump: unknown format '%c'\n", character);
      return;
    }

    index++;
  }
}

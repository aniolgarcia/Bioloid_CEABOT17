#include <stdio.h>

typedef unsigned char uint8_t;

static uint8_t startswith(const char* str, const char* substr)
{
  int i = 0;
  uint8_t eq = 0x1;
  while (str[i] && substr[i] && eq)
  {
    eq = str[i] == substr[i];
    ++i;
  }
  return !substr[i] && eq;
}

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s str strcmp\n", argv[0]);
    return -1;
  }

  if (startswith(argv[1],argv[2]))
    printf("%s IS a prefix of %s\n",argv[2],argv[1]);
  else 
    printf("%s is NOT a prefix of %s\n",argv[2],argv[1]);

  return 0;
}

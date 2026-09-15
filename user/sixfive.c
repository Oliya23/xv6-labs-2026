#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSZ 512

char *seps = " -\r\t\n./,";

int
is_sep(char c)
{
  for (char *p = seps; *p; p++) {
    if (c == *p)
      return 1;
  }
  return 0;
}

void
process(int fd)
{
  char buf[BUFSZ];
  int n;
  long num = 0;
  int in_number = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (int i = 0; i < n; i++) {
      char c = buf[i];
      if (c >= '0' && c <= '9') {
        num = num * 10 + (c - '0');
        in_number = 1;
      } else if (is_sep(c)) {
        if (in_number) {
          if (num % 5 == 0 || num % 6 == 0)
            printf("%ld\n", num);
          num = 0;
          in_number = 0;
        }
      } else {
        // non-digit, non-separator character: reset (not part of a number)
        num = 0;
        in_number = 0;
      }
    }
  }

  // end of file is an implicit separator
  if (in_number) {
    if (num % 5 == 0 || num % 6 == 0)
      printf("%ld\n", num);
  }
}

int
main(int argc, char *argv[])
{
  int fd;

  if (argc <= 1) {
    process(0); // stdin
  } else {
    for (int i = 1; i < argc; i++) {
      if ((fd = open(argv[i], 0)) < 0) {
        printf("sixfive: cannot open %s\n", argv[i]);
        continue;
      }
      process(fd);
      close(fd);
    }
  }

  exit(0);
}

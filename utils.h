#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
  char* path;
} ClipConfig;

void rtrim(char *str)
{
  char *s;
  s = str + strlen(str);
  while (--s >= str) {
    if (!isspace(*s)) break;
    *s = 0;
  }
}

void ltrim(char *str)
{
  size_t n;
  n = 0;
  while (str[n] != '\0' && isspace((unsigned char)str[n])) {
    n++;
  }
  memmove(str, str + n, strlen(str) - n + 1);
}

void trim(char *str)
{
  rtrim(str);
  ltrim(str);
}

void storeData(char *data, char *path)
{
  FILE *file = fopen(path, "a");
  if (file == NULL) {
    perror("Could not open file");
  } else {
    fprintf(file, "%s\n", data);
  }
  fclose(file);
}

ClipConfig parseArgs(int argc, char **argv)
{
  ClipConfig config = {};

  int c;

  while ((c = getopt(argc, argv, "p:")) != -1) {
    switch(c) {
      case 'p':
        config.path = optarg;
        break;
      default:
        // NOOP
        ;
    }
  }

  if (config.path == NULL) {
    printf("You need to define a path.\n");
    exit(EXIT_FAILURE);
  }

  return config;
}

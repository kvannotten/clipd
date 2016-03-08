#include <ctype.h>
#include <string.h>

void rtrim(char* str)
{
  char *s;
  s = str + strlen(str);
  while (--s >= str) {
    if (!isspace(*s)) break;
    *s = 0;
  }
}

void ltrim(char* str)
{
  size_t n;
  n = 0;
  while (str[n] != '\0' && isspace((unsigned char)str[n])) {
    n++;
  }
  memmove(str, str + n, strlen(str) - n + 1);
}

void trim(char* str)
{
  rtrim(str);
  ltrim(str);
}

void storeData(char* data, char* path)
{
  printf("%s\n", data);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>

// This is the number of bytes read and sisplayed at a time - 16 is good for an 80 column diplay.
#define READ_BLOCK_SIZE 16

int main(int argc, char *argv[])
{
  FILE *file = NULL;
  size_t bytes_got = 0;
  unsigned char buff[READ_BLOCK_SIZE];
  unsigned int bytes_offset = 0;
  struct stat file_stat;
  int offset_width = 0;

  if (argc == 1 || !strcmp(argv[1], "-help")) {
    printf("Usage:\n%s {options} file ...\n", argv[0]);
    printf("Options:\n");
    printf("  -help  Show this information:\n");
    exit(0);
  }

  for (int i = 1; i < argc; i++) 
  {
    // get the file stat info and try to open it
    if ((stat(argv[i], &file_stat)) || ((file = fopen(argv[i], "r")) == NULL))
    {
      perror(argv[i]);
      continue;
    }

    // determine how wide the offset needs to be
    //   maybe add an option to force something specific?
    sprintf(buff, "%llx", file_stat.st_size);
    offset_width = strlen(buff);
    if (offset_width < 6)
      offset_width = 6;
    else
      offset_width = (((offset_width + 1) / 2) * 2);

    if (i > 1)
      printf("\n");
    printf("%s\n", argv[i]);
    for (int j = 0; j < strlen(argv[i]); j++)
      printf("=");
    printf("\n");
    printf("%-*s  %-*s   Characters\n", offset_width, "Offset", (READ_BLOCK_SIZE*3)+1, "Bytes");

    while ((bytes_got = fread(buff, 1, READ_BLOCK_SIZE, file) ) > 0) {
      printf("%0*x ", offset_width, bytes_offset);
      for (int j = 0; j < bytes_got; j++) {
        printf(" %02x", buff[j]);
        if (((j+1) % 8) == 0) printf(" ");
      }
      for (int j = bytes_got; j < READ_BLOCK_SIZE; j++) {
        printf("   ");
        if (((j+1) % 8) == 0) printf(" ");
      }
      printf("   ");
      for (int j = 0; j < bytes_got; j++) {
        printf("%c", isprint(buff[j]) ? buff[j] : '.');
      }
      printf("\n");
      bytes_offset += READ_BLOCK_SIZE;
    }
  }
}

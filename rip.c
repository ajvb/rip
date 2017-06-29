#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void usage() {
  printf("rip - like cut, but more like what I actually want cut to do\n");
  printf("------------------------------------------------------------\n");
  printf("USAGE: rip [OPTIONS] [FILE]\n");
  printf("\n");
  printf("Options:\n");
  printf("  -f - Field you want to parse (leave empty to see all field options)\n");
  printf("  -d - Your delimiter. Default is '|'. You can use multi-character delimiters as well.\n");
  printf("  -h - See this usage message.\n");
  printf("\n");
  printf("[FILE]: You can leave this empty or use '-' for stdin. You can also list out as many files as you want space sperated.\n");
  printf("\n");
}

void process_line(char *instr, char *delim, int field) {
  char *val;
  int i = 0;

  while ((val = strsep(&instr, delim)) != NULL) {
    // Test if it is empty
    if (!strcmp(val, "")) {
      continue;
    }

    // TODO: Decide if I want this. (stripping out the newline if it exists)
    // Remove trailing newline if it exists
    if (val[strlen(val) - 1] == '\n') {
      val[strlen(val) - 1] = '\0';
    }

    if (field >= 0) {
      if (i == field) {
        printf("%s\n", val);
      }
    } else {
      printf("[%d] %s\n", i, val);
    }

    i++;
  }
}

void process_file(char *filename, char *delim, int field) {
  FILE *fp;
  char *line = NULL;
  size_t size;

  if(!strcmp(filename, "-")) {
    fp = stdin;
  } else {
    fp = fopen(filename, "r");
    if (fp == NULL) {
      printf("Failed to open file: %s\n", filename);
      abort();
    }
  }

  while (getline(&line, &size, fp) != -1) {
    process_line(line, delim, field);
  }
}

int main(int argc, char *argv[]) {
  char *delim = "|";
  int field = -1;
  int c;
  char *end;
  int index;

  while ((c = getopt(argc, argv, "hd:f:")) != -1) {
    switch(c) {
      case 'd':
        delim = optarg;
        break;
      case 'f':
        field = strtol(optarg, &end, 10);
        break;
      case 'h':
        usage();
        return 0;
      case '?': // TODO: Does this work as expected?
        if (optopt == 'd' || optopt == 'f') {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        usage();
        return 1;
      default:
        abort();
    }
  }

  // TODO: Support more than one field
  if (optind < argc) {
    for (index = optind; index < argc; index++) {
      process_file(argv[index], delim, field);
    }
  } else {
    char *f = "-";
    process_file(f, delim, field);
  }

  return 0;
}

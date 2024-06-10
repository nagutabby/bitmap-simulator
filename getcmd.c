#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

//
// Read a line of the form "<one letter command> [<optional integer argment>]".
// The command letter is stored in *cmd.
// The optional argument, if any, is stored in *arg.
// getcmd() returns:
// 2 or 3 if the command and the argument were read,
// 1 if only the command was read,
// 0 if EOF was detected, or
// -1 upon error.

int getcmd(char *pCmd, int *pArg1, int *pArg2) {
  char line[80];
  char cmd;
  char *bp;
  int optarg1, optarg2;
  int ac = 0;

  if ((bp = fgets(line, 80, stdin)) == NULL) {
    return 0;
  }
  if (*bp) {
    cmd = *bp++;
    ac++;
  }

  for (; *bp; bp++) {
    if (!isspace(*bp)) {
      int ic;
      ic = sscanf(bp, "%d %d", &optarg1, &optarg2);
      if (ic < 1) {
        fprintf(stderr, "Bad param: <%s>\n", bp);
        return -1;
      }
      if (pArg1) {
        *pArg1 = optarg1;
      }
      if (ic == 2 && pArg2) {
        *pArg2 = optarg2;
      }
      ac += ic;
      break;
    }
  }

  *pCmd = cmd;
  return ac;
}

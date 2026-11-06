#include <termios.h>                                                    /* required for termios */

struct stty_value{
  char *name;                                                           /* how flag displayed */
  tcflag_t key;                                                         /* constant of flag int value */
  int (*value_handler)(struct termios *, tcflag_t, int, char *[], int); /* a function pointer on a function handler */
};

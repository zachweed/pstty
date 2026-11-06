#include <termios.h> /* terminal io settings */

#ifndef FLAGS_H /* prevent duplicate includes */
#define FLAGS_H

/* identifies what group the flag is of, so we can programatically access flag in termios */
enum flag_group { LFLAG, IFLAG, CFLAG, CCFLAG };

struct stty {
  int flag_group_length; /* how many flag groups (4, so far) */
  int length; /* length of all values */
  int lengths[4]; /* positional lengths */
  int cflag_length; /* how many cflags */
  int ccflag_length; /* control character flag length */
  int iflag_length; /* iflag length */
  int lflag_length; /* lflag length */

  struct stty_value *cflags; /* cflags struct */
  struct stty_value *ccflags; /* control chars struct */
  struct stty_value *iflags; /* iflags struct */
  struct stty_value *lflags; /* lflags struct */

  struct stty_value *all_flags[4]; /* a container of all structs */
};

struct stty_value{
  char *name; /* how flag displayed */
  tcflag_t key; /* constant of flag int value */
  enum flag_group group; /* group of flags present in */
  int (*value_handler)( /* points to address of function that assigns/flips values */
    struct termios *,
    tcflag_t,
    int,
    char *[],
    int,
    int,
    enum flag_group
  ); /* a function pointer on a function handler */
};

/* writes array of lflags into from pointer */
int lflags(struct stty_value **where_flags_are);
/* handles iflags*/
int iflags(struct stty_value **where_flags_are);
/* cflags */
int cflags(struct stty_value **where_flags_are);
/* and control characters */
int cchar_flags(struct stty_value **where_flags_are);

#endif
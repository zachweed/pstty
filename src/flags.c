#include "flag_handlers.h" /* flag_processing header */

/* lflags - returns lflag data
 *   params:
 *     a) where_flags_are: pointer to array of structs
 *   output: none
 *   errs: none
 *   rets: number of flags
 */
int lflags(struct stty_value **where_flags_are) {
  static struct stty_value flags[] = {
    { .name = "echo", .key = ECHO, .group = LFLAG, .value_handler = flip_flag }, /* show typing with flip handler */
    { .name = "echok", .key = ECHOK, .group = LFLAG, .value_handler = flip_flag }, /* prints new line on kill */
    { .name = "icanon", .key = ICANON, .group = LFLAG, .value_handler = flip_flag }, /* process line-by-line? */
    { .name = "isig", .key = ISIG, .group = LFLAG, .value_handler = flip_flag } /* converts characters (&!) as signals */
  };
  *where_flags_are = flags; /* assigns flags */
  return 4;
}

/* iflags - returns iflag data
 *   params:
 *     a) where_flags_are: pointer to array of structs
 *   output: none
 *   errs: none
 *   rets: number of flags
 */
int iflags(struct stty_value **where_flags_are) {
  static struct stty_value flags[] = {
    { .name = "igncr", .key = IGNCR, .group = IFLAG, .value_handler = flip_flag } /* ignore return with flip handler */
  };
  *where_flags_are = flags; /* sets status of flags */
  return 1;
}

/* cflags - returns cflag data
 *   params:
 *     a) where_flags_are: pointer to array of structs
 *   output: none
 *   errs: none
 *   rets: number of flags
 */
int cflags(struct stty_value **where_flags_are) {
  static struct stty_value flags[] = {
    { .name = "parodd", .key = PARODD, .group = CFLAG, .value_handler = flip_flag } /* parodd with flip handler */
  };
  *where_flags_are = flags; /* sets flags */
  return 1;
}

/* cchars - writes flags into memory pointer to by pointer
 *   params:
 *     a) where_flags_are: pointer to array of structs
 *   output: none
 *   errs: none
 *   rets: number of flags
 */
int cchar_flags(struct stty_value **where_flags_are) {
  static struct stty_value flags[] = {
    { .name = "intr", .key = VINTR, .group = CCFLAG, .value_handler = assign_cchar }, /* interrupt control character */
    { .name = "min", .key = VMIN, .group = CCFLAG, .value_handler = assign_int }, /* minimum characters in icanon mode */
    { .name = "time", .key = VTIME, .group = CCFLAG, .value_handler = assign_int }, /* non-canonical read threshold */
    { .name = "erase", .key = VERASE, .group = CCFLAG, .value_handler = assign_cchar }, /* deletes character on left */
    { .name = "kill", .key = VKILL, .group = CCFLAG, .value_handler = assign_cchar } /* deletes whole line */
  };
  *where_flags_are = flags;
  return 5;
}
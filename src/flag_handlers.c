#include <string.h> /* required for string operations */
#include <stdlib.h> /* handles atoi */
#include "flag_handlers.h"

/* build_stty_config - builds a struct with the config
 *   params: none
 *   output: none
 *   errs: none
 *   rets: stty_config struct of all flags and their states
 */
struct stty build_stty_config() {
  struct stty_value *cf; /* c flags */
  struct stty_value *ccf; /* control characters */
  struct stty_value *ifla; /* iflags */
  struct stty_value *lf; /* lflags */

  int lflags_length = lflags(&lf); /* length of lflags */
  int iflags_length = iflags(&ifla); /* iflag length */
  int ccflags_length = cchar_flags(&ccf); /* control character length */
  int cflags_length = cflags(&cf); /* cflag length */

  struct stty stty_config = {
    .flag_group_length = 4, /* how many flag groups (cflag, cchar, lflag, iflag) */
    .length = 10, /* length of all members of each group */
    .lflag_length = lflags_length, /* lflag length */
    .lengths[0] = lflags_length, /* positional lflag length */
    .lflags = lf, /* lflags */
    .lengths[1] = iflags_length, /* iflag positional length */
    .iflag_length = iflags_length, /* iflag length */
    .iflags = ifla, /* iflags */
    .lengths[2] = ccflags_length, /* control character positional */
    .ccflag_length = ccflags_length, /* control character length */
    .ccflags = ccf, /*all control characters */
    .lengths[3] = cflags_length, /* positional lenght of cflags */
    .cflag_length = cflags_length, /* cflag length */
    .cflags = cf, /* all cflags */
    .all_flags = { lf, ifla, ccf, cf } /* array of all flags */
  };

  return stty_config;
}

/* flag_matchesP - returns if flag name matches query
 *   params:
 *     a) field: field to match flag to,
 *     b) flag_name: flag to match field,
 *     c)
 *   output: none
 *   input: none
 *   errs: none
 *   rets: if argument name matches
 */
int flag_matchesP(char *field, char *flag_name) {
  return ( strcmp(field, flag_name) == 0 ); /* matches if field and flag name match */
}

/* flag_enabledP - returns state of flag
 *   params:
 *     a) flag_name: what flag,
 *     b) sconfig: stty struct configuration,
 *     c) terminal_io: terminal information,
 *     d) flag_type: what type of flag,
 *     e) key: name of flag
 *   errs: none
 *   output: none
 *   input: none
 *   rets: 1 if flag is enabled and 0 if not
 */
int flag_enabledP(struct termios terminal_io, char *flag_type, tcflag_t key) {
  if ( strcmp( flag_type, "lflag") == 0 ) { /* if lflag */
    return terminal_io.c_lflag & key; /* on ? */
  } else if ( strcmp( flag_type, "iflag") == 0 ) { /* if iflag */
    return terminal_io.c_iflag & key; /* is enabled? */
  } else if ( strcmp( flag_type, "cflag") == 0 ) { /* if cflag */
    return terminal_io.c_cflag & key; /* enabled? */
  }
  return 0;
}

/* set_control_character - sets terminal driver control character
 *   params:
 *      a) terminal_io: termios struct of terminal status,
 *      b) flag: what flag control character is being set on,
 *      c) value: the control character(s) to set
 *   output: none
 *   rets: none
 */
void set_control_character(struct termios *terminal_io, tcflag_t flag, char *value) {
  terminal_io->c_cc[flag] = value[0]; /* purely sets the flag to a value */
}

/* integer_argument_suppliedP - returns if integer supplied with flag
 *   params:
 *     a) argc: argument count
 *     b) argi: current argument index
 *   errs: none
 *   output: none
 *   input: none
 *   rets: 1 if argument is an integer, 0 if not
 */
int integer_argument_suppliedP(int argc, char *argv[], int argi) {
  if (argi + 1 >= argc) { /* if index beyond argument count */
    return 0;
  }

  char *sub_argument = argv[argi + 1]; /* sub argument (eg kill n) */
  char *initial_non_int; /* shall store the initial non integer character */

  strtol(sub_argument, &initial_non_int, 10); /* convert argument to integer */

  if (initial_non_int == sub_argument) { /* if empty argument */
    return 0;
  }
  if (*initial_non_int != '\0') {/* if a character was found */
    return 0;
  }

  return 1;
}

/* assign_control_character - assigns control character
 *   params:
 *      a) terminal_io: termios struct of terminal status,
 *      b) flag: what flag it is being set on,
 *      c) argc: argument count,
 *      d) argv: argument vector,
 *      e) i: iterator of current argument
 */
int assign_cchar(struct termios *terminal_io, tcflag_t flag, int argc, char *argv[], int i, int flipped, enum flag_group group) {
  (void)flipped; /* argument exists because it is a function pointer */
  (void)group; /* also an intentionally unused argument */

  if ( i + 1 >= argc ) { /* if not a sub-argument */
    return 1;
  }
  char *cchar_value = argv[i + 1]; /* else grab sub-argument */
  if (strlen(cchar_value) != 1) { /* if sub-argument multi-character */
    return 1;
  }
  set_control_character(terminal_io, flag, cchar_value);
  return 0;
}

/* assign_int - assigns integer value
 *   params:
 *      a) terminal_io: termios struct of terminal status,
 *      b) flag: what flag it is being set on,
 *      c) argc: argument count,
 *      d) argv: argument vector,
 *      e) i: iterator of current argument
 *   output: none
 *   errs: none
 *   rets: 0 if successful, 1 if not
 */
int assign_int(struct termios *terminal_io, tcflag_t flag, int argc, char *argv[], int i, int flipped, enum flag_group group) {
  (void)flipped; /* argument exists because it is a function pointer */
  (void)group; /* also an intentionally unused argument */

  if ( integer_argument_suppliedP(argc, argv, i) ) { /* if integer argument provided */
    int int_arg = atoi(argv[++i]); /* convert to integer */
    if ( int_arg < 0 || int_arg >= 256 ) { /* if large */
      return 1; /* return error code */
    } else { /* if not */
      terminal_io->c_cc[flag] = int_arg; /*then set it */
    }
  } else {
    return 1;
  }
  return 0;
}

/* flip_flag - flips flag to opposite value
 *   params:
 *      a) terminal_io: termios struct of terminal status,
 *      b) flag: what flag it is being set on,
 *      c) argc: argument count,
 *      d) argv: argument vector,
 *      e) i: iterator of current argument
 *   output: none
 *   errs: none
 *   rets: 0 if successful
 */
int flip_flag(struct termios *terminal_io, tcflag_t flag, int argc, char *argv[], int i, int flipped, enum flag_group group) {
  (void)argc; /* intentionally unused */
  (void)argv; /* also intentionally unused */
  (void)i; /* intentionally unused */

  tcflag_t *g;

  switch(group) { /* find which group to switch flags with */
    case LFLAG:
      g = &terminal_io->c_lflag; /* address of what lflags points to */
      break;
    case IFLAG:
      g = &terminal_io->c_iflag; /* iflags */
      break;
    case CFLAG:
      g = &terminal_io->c_cflag; /* cflags */
      break;
    default:
      return 1; /* return 1 if invalid flag */
  }

  if (flipped) { /* if -flag_name provided */
    *g &= ~flag; /* disable flag at what at address */
  } else {
    *g |= flag; /* enable flag */
  }

  return 0;
}
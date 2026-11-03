/* stty
 *
 *   purpose: sets options via a device driver on a terminal device
 *   plan:
 *      incorporate error handling
 *      incorporate flags
 *      make sure it works in dash
 */

#include <stdio.h>     /* so we can handle i/o */
#include <termios.h>   /* required for termios */
#include <unistd.h>    /* unix standard */
#include <string.h>    /* required for string operations */
#include <sys/ioctl.h> /* so we can grab window size */
#include <stdlib.h>    /* handles atoi */

struct stty_value{
  char *name; /* how flag displayed */
  tcflag_t key; /* constant of flag int value */
  int (*value_handler)(struct termios *, tcflag_t, int, char *[], int); /* a function pointer on a function handler */
};

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

int get_flag(char *, struct stty);
int turn_on_flag(char *, struct stty);
int turn_off_flag(char *, struct stty);
char *get_char_flag(char *);
int set_char_flag(char *);
int flag_matchesP(char *, char *);

/* integer_argument_suppliedP - returns if integer supplied with flag
 *   params:
 *     a) argc: argument count
 *     b) argi: current argument index
 *   errs: none
 *   output: none
 *   input: none
 *   rets: if argument is an integer
 */
int integer_argument_suppliedP(int argc, int argi) {
  return ( argi + 1 < argc ); /* returns if argument has another value with it */
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

/* get_flag - returns state of flag
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
int flag_enabledP(char *flag_name, struct stty sconfig, struct termios terminal_io, char *flag_type, tcflag_t key) {
  if ( strcmp( flag_type, "lflag") == 0 ) { /* if lflag */
    return terminal_io.c_lflag & key; /* on ? */
  } else if ( strcmp( flag_type, "iflag") == 0 ) { /* if iflag */
    return terminal_io.c_iflag & key; /* is enabled? */
  } else if ( strcmp( flag_type, "cflag") == 0 ) { /* if cflag */
    return terminal_io.c_cflag & key; /* enabled? */
  }
  return 0;
}

/* print_lflags - prints states of lflags
 *   params:
 *     a) sconfig,
 *     b) terminal_io: termios struct of terminal status,
 *     c) winsize: window size of terminal
 *   output: status of lflags
 *   input: none
 *   errs: none
 *   rets: none
 */
void print_lflags(struct stty sconfig, struct termios terminal_io) {
  printf("lflags: ");
  for(int i = 0; i < sconfig.lflag_length; i++) { /* for each lflag */
    if ( 
      flag_enabledP( /* if flag is already enabled */
        sconfig.lflags[i].name, /* how flag is displayed */
        sconfig, /* stty config */
        terminal_io, /* terminal config */
        "lflag", /* flag type */
        sconfig.iflags[i].key /* constant of flag */
      ) 
    ) {
      printf("%s ", sconfig.lflags[i].name); /* print flag as enabled */
    } else {
      printf("-%s ", sconfig.lflags[i].name);  /* print flags as not enabled */
    }
  }
  printf("\n");
}

/* print_iflags - prints states of iflags
 *   params:
 *     a) sconfig,
 *     b) terminal_io: termios struct of terminal status,
 *     c) winsize: window size of terminal
 *  output: status of iflags
 *  input: none
 *  errs: none
 *  rets: none
 */
void print_iflags(struct stty sconfig, struct termios terminal_io) {
  printf("iflags: ");
  for(int i = 0; i < sconfig.iflag_length; i++) {
    if ( 
      flag_enabledP( /* if flags is enabled */
        sconfig.iflags[i].name, /* how flag displayed */
        sconfig, /* stty_config */
        terminal_io, /* terminal information */
        "iflag",  /* flag type */
        sconfig.iflags[i].key /* constant of flag */
      ) 
    ) {
      printf("%s ", sconfig.iflags[i].name);  /* print as enabled */
    } else {
      printf("-%s ", sconfig.iflags[i].name);  /* print as not-enabled */
    }
  }
  printf("\n");
}

/* print_cflags - prints states of cflags
 *   params:
 *     a) sconfig,
 *     b) terminal_io: termios struct of terminal status,
 *     c) winsize: window size of terminal
 *   output: status of cflags
 *   input: none
 *   errs: none
 *   rets: none
 */
void print_cflags(struct stty sconfig, struct termios terminal_io) {
  printf("cflags: ");
  for(int i = 0; i < sconfig.cflag_length; i++) { /* for each cflag */
    printf("%s ", sconfig.cflags[i].name); /* print flag information */
  }
  printf("\n");
}

/* print_cchars - prints states of control-characters
 *   params:
 *     a) sconfig,
 *     b) terminal_io: termios struct of terminal status,
 *     c) winsize: window size of terminal
 *   output: control character values
 *   rets: none
 */
void print_cchars(struct stty sconfig, struct termios terminal_io) {
  printf("cchars: ");
  for(int i = 0; i < sconfig.ccflag_length; i++) { /* for each control character */
    printf(
      "%s = ^%c ", 
      sconfig.ccflags[i].name, /* print display name of flag */
      terminal_io.c_cc[sconfig.ccflags[i].key] - 1 + 'A' /* print offset of character from beginning */
    );
  }
  printf("\n");
}

/* print_header - prints header of terminal output
 *   params:
 *     a) sconfig,
 *     b) terminal_io: termios struct of terminal status,
 *     c) winsize: window size of terminal
 *   output: dimensions along with speed of terminal
 *   rets: none
 */
void print_header(struct stty sconfig, struct termios terminal_io, struct winsize ws) {
  printf(
    "speed %lu baud; %d rows; %d columns;\n", /* prints speed, rows, and columns */
    (unsigned long)cfgetospeed(&terminal_io), /* terminal processing speed */
    ws.ws_row, /* rows of window */
    ws.ws_col /* columns of window */
  );
}

/* print_flags - prints pstty output
 *   params:
 *     a) terminal_io: termios struct of terminal status
 *   output: stty header information and lflag, iflag, cflag, and cchar states
 *   rets: none
 */
void print_output(struct stty sconfig, struct termios terminal_io, struct winsize ws) {
  print_header(sconfig, terminal_io, ws); /* prints speed, columns, and rows */
  print_lflags(sconfig, terminal_io); /* prints lflags */
  print_iflags(sconfig, terminal_io); /* prints iflags */
  print_cflags(sconfig, terminal_io); /* prints cflags */
  print_cchars(sconfig, terminal_io); /* prints control characters */
}

/* set_control_character - sets terminal driver control character
 *   params:
 *      a) terminal_io: termios struct of terminal status,
 *      b) flag: what flag control character is being set on,
 *      c) c: the control character to set 
 *   output: none
 *   rets: none
 */
void set_control_character(struct termios *terminal_io, tcflag_t flag, cc_t c) {
  terminal_io->c_cc[flag] = c - 'A' + 1; /* set character as offset from beginning */
}

/* assign_control_character - assigns control character
 *   params:
 *      a) terminal_io: termios struct of terminal status,
 *      b) flag: what flag it is being set on,
 *      c) argc: argument count,
 *      d) argv: argument vector,
 *      e) i: iterator of current argument
 */
int assign_cchar(struct termios *terminal_io, tcflag_t flag, int argc, char *argv[], int i) {
  if ( i + 1 < argc ) { /* if there is another argument */
    char *value = argv[i + 1]; /* define value of argument */
    if (value[0] == '^' && value[1] != '\0' && value[2] == '\0') {
      if ( strcmp(argv[i], "erase") == 0 ) { /* if erase control character configure */
        set_control_character(terminal_io, VERASE, value[1]); /* set control character then */
      } else if ( strcmp(argv[i], "intr") == 0 ) { /* if interrupt control character set, then */
        set_control_character(terminal_io, VINTR, value[1]);  /* set character */
      } else if ( strcmp( argv[i], "kill") == 0 ) { /* kill character set? */
        set_control_character(terminal_io, VKILL, value[1]); /* set properly */
      }
    }  
  }
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
int assign_int(struct termios *terminal_io, tcflag_t flag, int argc, char *argv[], int i) {
  if ( integer_argument_suppliedP(argc, i) ) { /* if integer argument provided */
    int int_arg = atoi(argv[++i]); /* convert to integer */
    if ( int_arg >= 256 ) { /* if too large */
      return 1; /* return error code */
    } else { /* if not */
      terminal_io->c_cc[VMIN] = int_arg; /*then set it */
    }
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
int flip_flag(struct termios *terminal_io, tcflag_t flag, int argc, char *argv[], int i) {
  terminal_io->c_lflag ^= flag; /* flip status of flag */
  return 0;
}

/* lflags - returns lflag data
 *   params:
 *     a) where_flags_are: pointer to array of structs
 *   output: none
 *   errs: none
 *   rets: number of flags
 */
int lflags(struct stty_value **where_flags_are) {
  static struct stty_value flags[] = {
    { .name = "echo", .key = ECHO, .value_handler = flip_flag }, /* show typing with flip handler */
    { .name = "isig", .key = ISIG, .value_handler = flip_flag } /* converts characters (&!) as signals */
  };
  *where_flags_are = flags; /* assigns flags */
  return 2;
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
    { .name = "igncr", .key = IGNCR, .value_handler = flip_flag } /* ignore return with flip handler */
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
    { .name = "parodd", .key = PARODD, .value_handler = flip_flag } /* parodd with flip handler */
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
    { .name = "intr", .key = VINTR, .value_handler = assign_cchar }, /* interrupt control character */
    { .name = "min", .key = VMIN, .value_handler = assign_int }, /* minimum characters in icanon mode */
    { .name = "time", .key = VTIME, .value_handler = assign_int }, /* non-canonical read threshold */
    { .name = "erase", .key = VERASE, .value_handler = assign_cchar }, /* deletes character on left */
    { .name = "kill", .key = VKILL, .value_handler = assign_cchar } /* deletes whole line */
  };
  *where_flags_are = flags;
  return 5;
}

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

int main(int argc, char *argv[]) {
  struct winsize window_size; /* terminal window size information */
  struct stty stty_config = build_stty_config(); /* stty_config struct */
  struct termios terminal_io; /* terminal io information */

  if ( ioctl(STDIN_FILENO, TIOCGWINSZ, &window_size) == -1 ) { /* if error grabbing window size */
    perror("error grabbing io control"); /* print error */
    return 1; /* return error code */
  }
  if ( tcgetattr(STDIN_FILENO, &terminal_io) == -1 ) { /* if error getting terminal settings */
    perror("error grabbing terminal input output"); /* print error */
    return 1; /* print error code */
  }
  for (int h = 0; h < argc; h++) { /* for each argument */
    if ( strcmp(argv[h], "-a") == 0 ) { /* if -a flag supplied */
      print_output(stty_config, terminal_io, window_size); /* print all flag statuses */
    }
    for (int i = 0; i < stty_config.flag_group_length; i++) { /* for each flag group */
      for (int j = 0; j < stty_config.lengths[j]; j++) { /* for each flag in group */
        if (strcmp(argv[h], stty_config.all_flags[i][j].name) == 0 ) { /* if argument equals flag */
          stty_config.all_flags[i][j].value_handler( /* call function pointer of handler */
            &terminal_io,  /* terminal io config */
            stty_config.all_flags[i][j].key, /* stty config of given key */
            argc, /* arg count */
            argv, /* arg vector */
            h /* current flag */
          );
        }
      }
    }
  }
  if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_io) == -1) {
    perror("tcsetattr");
    return -1;
  }
  return 0;
}

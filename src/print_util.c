#include <stdio.h> /* io */
#include "print_util.h" /* util header */
#include "flag_handlers.h" /* handles flags */

/* print_lflags - prints states of lflags
 *   params:
 *     a) sconfig,
 *     b) terminal_io: termios struct of terminal status,
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
        terminal_io, /* terminal config */
        "lflag", /* flag type */
        sconfig.lflags[i].key /* constant of flag */
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
 *   output: status of cflags
 *   input: none
 *   errs: none
 *   rets: none
 */
void print_cflags(struct stty sconfig, struct termios terminal_io) {
  printf("cflags: ");
  for(int i = 0; i < sconfig.cflag_length; i++) {
    if (
      flag_enabledP( /* if flags is enabled */
        terminal_io, /* terminal information */
        "cflag",  /* flag type */
        sconfig.cflags[i].key /* constant of flag */
      )
    ) {
      printf("%s ", sconfig.cflags[i].name);  /* print as enabled */
    } else {
      printf("-%s ", sconfig.cflags[i].name);  /* print as not-enabled */
    }
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
    if ( sconfig.ccflags[i].key == VMIN || sconfig.ccflags[i].key == VTIME ) {
      printf(
        "%s = %d; ",
        sconfig.ccflags[i].name, /* print display name of flag */
        terminal_io.c_cc[sconfig.ccflags[i].key] /* print numerical value of control character (VTIME and VMIN) */
      );
    } else if ( terminal_io.c_cc[sconfig.ccflags[i].key] == 127 ) { /* if delete character */
      printf(
        "%s = ^?; ",
        sconfig.ccflags[i].name
      );
    } else if ( terminal_io.c_cc[sconfig.ccflags[i].key] < 32 ) { /* elsif control character */
      printf(
        "%s = ^%c; ",
        sconfig.ccflags[i].name, /* print display name of flag */
        terminal_io.c_cc[sconfig.ccflags[i].key] - 1 + 'A' /* print offset of character from beginning */
      );
    } else { /* some printable character between alphabet & control characters and DEL */
      printf(
        "%s = %c; ",
        sconfig.ccflags[i].name,
        terminal_io.c_cc[sconfig.ccflags[i].key]
      );
    }
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
void print_header(struct termios terminal_io, struct winsize ws) {
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
  print_header(terminal_io, ws); /* prints speed, columns, and rows */
  print_lflags(sconfig, terminal_io); /* prints lflags */
  print_iflags(sconfig, terminal_io); /* prints iflags */
  print_cflags(sconfig, terminal_io); /* prints cflags */
  print_cchars(sconfig, terminal_io); /* prints control characters */
}
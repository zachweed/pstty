/* ptty
 *  author: Zach Weed
 *  purpose: sets and displays options via a device driver on a terminal device
 */

#include <string.h>    /* required for string operations */
#include "flag_handlers.h" /* handles config of flags */
#include "flags.h"     /* custom struct implements */
#include <stdio.h>     /* so we can handle i/o */
#include <unistd.h>    /* unix standard */
#include <sys/ioctl.h> /* so we can grab window size */
#include "print_util.h" /* handles printing flag information */

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
  if (argc == 1) { /* if no argument supplied */
    print_output(stty_config, terminal_io, window_size); /* print all flag statuses */
    return 0;
  }
  for (int h = 1; h < argc; h++) { /* for each argument, excluding program name */
    int flag_matched = 0; /* if we found a flag */
    if ( strcmp(argv[h], "-a") == 0 ) { /* if -a flag supplied */
      print_output(stty_config, terminal_io, window_size); /* print all flag statuses */
      continue; /* continues processing next flags */
    }
    for (int i = 0; i < stty_config.flag_group_length && !flag_matched; i++) { /* for each flag group */
      for (int j = 0; j < stty_config.lengths[i]; j++) { /* for each flag in group */
        char *flag = argv[h]; /* pull flag into variable in case it is enabling/disabling */
        int flag_flipped = 0; /* if enabling/disabling then set a sub-flag */

        if (flag[0] == '-') { /* if flag being disabled */
          flag_flipped = 1; /* indicate flag was flipped */
          flag++; /* advance pointer beyond '-' character */
        }

        if (strcmp(flag, stty_config.all_flags[i][j].name) == 0 ) { /* if argument equals flag */
          /* if a flag is attempting to be flipped and is not a CCFLAG */
          if (flag_flipped && stty_config.all_flags[i][j].group == CCFLAG) {
            fprintf(stderr, "invalid argument: %s\n", argv[h]);
            return 1;
          }
          int handle_call = stty_config.all_flags[i][j].value_handler( /* call function pointer of handler */
            &terminal_io,  /* terminal io config */
            stty_config.all_flags[i][j].key, /* stty config of given key */
            argc, /* arg count */
            argv, /* arg vector */
            h, /* current flag */
            flag_flipped,
            stty_config.all_flags[i][j].group /* flag group */
          );

          if (handle_call != 0) { /* if non-success code */
            fprintf(stderr, "invalid value: %s\n", argv[h]);
            return 1;
          }
          /* if a flag that has a sub-argument, skip next argument in loop */
          if (stty_config.all_flags[i][j].group == CCFLAG) {
            h++;
          }
          flag_matched = 1; /* breaks from current flag group */
          break; /* breaks from current flag */
        }
      }
    }
    if ( !flag_matched ) { /* identify if not a single flag matched */
      fprintf(stderr, "invalid argument: %s\n", argv[h]);
      return 1;
    }
  }
  if (tcsetattr(STDIN_FILENO, TCSANOW, &terminal_io) == -1) {
    perror("tcsetattr");
    return -1;
  }
  return 0;
} 
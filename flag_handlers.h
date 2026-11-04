#include <termios.h> /* terminal io */
#include "flags.h" /* flag processing header */

/* assigns a control character (e.g. ^C) */
int assign_cchar(struct termios *terminal_io, tcflag_t, int, char *[], int, int, enum flag_group);
/* assigns integer value (e.g., min, time) */
int assign_int(struct termios *terminal_io, tcflag_t, int, char *[], int, int, enum flag_group);
/* flips state of flag (e.g. -icanon) */
int flip_flag(struct termios *, tcflag_t, int, char *[], int, int, enum flag_group);
/* if flag enabled */
int flag_enabledP(struct termios, char *, tcflag_t);
/* processes stty config */
struct stty build_stty_config();

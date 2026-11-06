#include <termios.h> /* terminal io settings */
#include <sys/ioctl.h> /* io control sys calls */
#include "flags.h" /* stty config */

/* handles printing output */
void print_output(struct stty, struct termios, struct winsize);
/* prints header info */
void print_header(struct termios, struct winsize);
/* prints iflag info */
void print_iflags(struct stty, struct termios);
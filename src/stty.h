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
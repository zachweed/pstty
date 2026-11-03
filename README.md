PSTTY
======

  An implementation of tty, a tool for displaying terminal configuration

SYNOPSIS
--------

  pstty [-a] [arguments]

DESCRIPTION
-----------

  stty provides current tty (teletype) config of the current terminal device

OPTIONS
-------

  -a  Display all options


EXAMPLES
--------
  
  pstty -a
  pstty icanon


COMPILING
---------
  
  make clean
  make


NOTES
-----

  - Works with lstat (not nftw / ftw) and recursion in traversing directories and providing block size,
  - Uniqueness is handled via compound-key of (inode, device_id), &
  - Like du, if an error is found, while the error is returned to main, it does not prevent further traversal

STATUS CODES
------------

  0 Success

  1 Error Status

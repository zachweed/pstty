pstty
======

  An implementation of tty, a tool for displaying terminal configuration

synopsis
--------

  pstty [-a] [arguments]

description
-----------

  stty provides current tty (teletype) config of the current terminal device

options
-------

  -a  Display all options

examples
--------
  
  pstty -a
  pstty icanon


compiling
---------
  
  make clean
  make

notes
-----

  - Works with lstat (not nftw / ftw) and recursion in traversing directories and providing block size,
  - Uniqueness is handled via compound-key of (inode, device_id), &
  - Like du, if an error is found, while the error is returned to main, it does not prevent further traversal

status codes
------------

  0 Success

  1 Error Status

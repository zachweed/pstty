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

status codes
------------

  0 Success

  1 Error Status

# rshell
Purpose
====
rshell is a simple command shell created for my CS100 class at UCR.

How to install
====
```
$ git clone https://github.com/kevinzhen/rshell.git
$ cd rshell
$ make
$ bin/rshell
```
Limits/Bugs/Comments
====
1. Connectors must be space seperated between commands 

For Example
```
ls || echo ; ls && pwd # stuff
``` 
2. Input commands with trailing || or && are treated as syntax errors
```
ls ||
ls &&
```


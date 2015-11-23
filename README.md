# rshell
Kevin Zhen
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
3. Precedence Operators are not functional, but rshell is able detect an error if the opening and closing precendence operators do not match.
```
( ls && ls ) is not supported
( ls && ls ) ) will throw a error because of missing opening precedence operator
```

Design
====
rshell is designed to be a simple command shell that currently supports commands from /bin, test, connectors and comments. rshell will take a commandline from the user and parse it into tokens which are then used to create a vector of command structs which holds data necessary to execute the command line. My code is a little sloppy right now and definitely needs to be organized better. I need to take alot of code from main and put them into helper functions for readability. Furthermore, I believe that if I used a composite design pattern to redo my code, I could save alot of memory that I'm wasting in some of my struct data members.

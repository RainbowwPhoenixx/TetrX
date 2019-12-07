# TetrX - Yet another Tetris clone

This project emerges from my previous Tetris project written in Pascal. I decided to move to C for convenience and much better compatibility with all systems.

Switching to C is also an opportunity to :
- Learn this programming language
- Learn from the past and produce a much more usable Tetris clone
- Get used to proper and thorough programming, by writing code that is as flexible as possible
- Eventually learn AI and multithreading techniques by producing a Tetris AI with this project

## How to play ?

- Clone the project in whatever directory you like
- `cd` into the directory
- Run the command `make TetrX_debug` to compile the program
- To launch the game run `./bin/TetrX_debug`.

You will no doubt have noticed that you can only compile the version with debug symbols as I haven't bothered making proper make recipes for a "normal" version.

## To do

- Make the ncurses library be linked statically for compatibility with computers where ncurses is not installed
- Implement game mechanics
- Create a main program
- Make keys configurable
- Create a graphical interface
- Rip out the ncurses input system and replace it with SDL
- Make a versus mode

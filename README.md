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
- Run the command `make TetrX` to compile the program
- To launch the game run `./bin/TetrX`.

## To do
### General
- Make the ncurses library be linked statically for compatibility with computers where ncurses is not installed
- Make SDL be linked statically for the same reason
- Read the different options from a text file at runtime
- Separate game & bot data structs more
### Game
- Make a versus mode
- Implement gravity
- Implement lock delay
- Implement T-spin detection
- Implement scoring
- Create a recording functionality
- Create a replay functionality
### Interface
- Make keys configurable
- Create a graphical interface
- Implement animations properly
### Bot
- Change botBoard implementation to an array of words
- General type overhaul
- Add Sonic drop to pathfinder & better HD handling
- Implement multiple queues with different priorities aka better tree search
- Implement more metrics for the evaluation function

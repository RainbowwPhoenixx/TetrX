# TetrX - A Tetris AI and its crummy client

This project is an AI that is designed to play Tetris versus, even though the client is singleplayer only. This project presently only compiles and runs on linux, and needs SDL2 to compile (SDL is currently only used for getting inputs from a human player).

## How to use ?

- Clone the project in whatever directory you like
- `cd` into the directory
- Run the command `make TetrX` to compile the program
- To launch the game run `./bin/TetrX <bot|human>`
- When the bot has been run at least once, a logging file and a weights file will appear.
- You can launch another terminal with the logs produced by the bot by running `make view_log`
- You can modify the weights used in the evaluation function by opening *weights.txt* and editing the values

## To do
Note : These are used by me to track my progress, are in no particular order, and do not garantee that they will eventually be implemented.
### General
- Make the ncurses library be linked statically for compatibility with computers where ncurses is not installed
- Make SDL be linked statically for the same reason
- Separate game & bot data structs more
### Game
- Implement scoring
- Create a recording functionality
- Create a replay functionality
- Add APP to stat tracking
### Interface
- Make keys configurable
- Create a graphical interface
- Implement animations properly
### Bot
- General type overhaul
- Optimize memory copying
- Implement more metrics for the evaluation function
- Pattern detection
- Better variable & type names plz
- Fix Tspin detection
- Make pathfinder return the board and the tspin status instead of refiguring it out in the node creation function

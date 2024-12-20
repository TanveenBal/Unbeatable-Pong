# Unbeatable Pong

Unbeatable Pong is a simple Pong game built using the Raylib library in C++. The game features a classic Pong gameplay experience with some additional features and customization options.

## Features

- Classic Pong gameplay with player-controlled paddle and CPU-controlled opponent.
- Keep track of lives remaining as you play.
- Sound effects for paddle hits and lost hearts.
- Start screen with a "Start" button to begin the game and a "Settings" button to access game settings.
- End screen with an option to restart the game or exit.
- Rally score to keep track of consecutive hits.
- Classic Pong ball physics and paddle collisions.

## How to Play

1. **Start Screen**: Upon launching the game, you'll see the start screen with "Start" and "Settings" buttons. Click "Start" to begin playing or "Settings" to access game settings.

2. **Gameplay**: Control the paddle using the W and S keys (move up and down, respectively). Try to hit the ball past the CPU-controlled opponent to score points.

3. **Lives Remaining**: Keep an eye on the lives remaining indicator at the top of the screen. You start with three lives, and each time the opponent scores, you lose a life.

4. **Rally Score**: The game keeps track of your consecutive hits (rally score). Try to achieve the highest rally score possible!

5. **End Screen**: When you run out of lives, the game ends, and you'll see the end screen with options to restart or exit the game.

<p align="center">
  <img src="resources/Pong.gif" alt="Pong">
</p>

## Controls

- W: Move paddle up.
- S: Move paddle down.

## Game Setup

1. Clone the repo (if on windows download just Unbeatable_pong.exe)
- `git clone https://github.com/TanveenBal/Unbeatable-Pong.git`
2. Set Up the Build Environment with CMake
- **Create a Build Directory**, To keep the build files organized, create a separate directory for building:
- `mkdir build`
- `cd build`
- **Run CMake**, Generate the build configuration files with:
- `cmake ..`
- This command looks for the CMakeLists.txt file in the project root and configures the build system
- Ensure Raylib is properly installed and accessible (or adjust paths in CMakeLists.txt if needed)
- `cmake --build .`
3. Run the Game
- **Launch the game**, Once compiled, the executable will be located in the build directory:
- `./UnbeatablePong`      # Linux/MacOS
- `./UnbeatablePong.exe`  # Windows

## Controls for Playing

- **W**: Move the player paddle up.
- **S**: Move the player paddle down.

### Game Features
- **Rally Score**: Track the consecutive hits during a rally, attempt to reach a high rally score before losing a life.
- **Lives**: You start the game with three lives. Each time the CPU scores, you lose a life.
- **End Screen**: At the end of the game, a screen appears with options to restart or exit.
- **Settings**: Modify game speed through the settings menu.

## Credits

This game was created by Tanveen Singh Bal.

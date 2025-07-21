

CROSSY ROAD - 2D Lane-Based Game
==================================

A simple 2D lane-based game inspired by Crossy Road, built using C++ and OpenGL (GLUT).
The player navigates through lanes, avoiding moving cars while advancing forward through levels.


FEATURES
-----------------------------------------------------
- Player movement across predefined lanes (Up, Down, Left, Right)
- Obstacle (car) spawning with random direction and color
- Multiple game states: Start Screen, Playing, Level Complete, Game Over
- Increasing difficulty with levels
- Basic score system
- Visual effects including simple character animation and road graphics
- Text rendering with shadow/outline effects

CONTROLS
-----------------------------------------------------
ARROW KEYS       : Move the player (Up, Down, Left, Right)
SPACE            : Start or restart the game
L (during level) : Load next level
Q                : Quit the game


REQUIREMENTS
-----------------------------------------------------
- C++ compiler (supports C++11 or later)
- OpenGL Utility Toolkit (GLUT or FreeGLUT)


COMPILATION (Linux example)
-----------------------------------------------------
Make sure you have `freeglut` installed:

    sudo apt-get install freeglut3-dev

Then compile with:

    g++ main.cpp -o crossyroad -lGL -lGLU -lglut

Run the game:

    ./crossyroad

COMPILATION (Windows using MinGW)
-----------------------------------------------------
1. Install `freeglut` and set up your include/lib paths.
2. Compile using:

    g++ main.cpp -o crossyroad -lfreeglut -lopengl32 -lglu32


FILE STRUCTURE
-----------------------------------------------------
- Lines.cpp        : Core game logic, rendering, and input handling
- README.txt      : This file


NOTES
-----------------------------------------------------
- This project is meant for educational/demo purposes.
  
## Future Improvements:
  - Sound effects
  - Power-ups
  - Player customization
  - Leaderboard or score saving
  - More detailed animations

AUTHOR
-----------------------------------------------------
Naznin Jahan Noor

GitHub : https://github.com/NJNOOR

LICENSE
-----------------------------------------------------
This project is open-source under the MIT License.


# CRAZY JUMP - A Simple 2D Platformer

## Overview

CRAZY JUMP is a simple and engaging 2D endless platformer game built using OpenGL and GLUT. The player controls a character that must jump between procedurally generated platforms to ascend and achieve the highest possible score. The game features different types of platforms (static, moving, breakable) and collectible power-ups that provide temporary jump boosts, adding layers of challenge and excitement.

This project was created as a practical exercise in applying fundamental computer graphics concepts using the OpenGL library.

## Features

* **Intuitive Controls:** Simple keyboard controls for left/right movement and automatic jumping upon landing.
* **Endless Gameplay:** Platforms are generated dynamically as the player ascends, providing a unique experience each time.
* **Varied Platform Types:**
    * **Static Platforms:** Stationary platforms for stable landings.
    * **Moving Platforms:** Horizontally oscillating platforms requiring timed jumps.
    * **Breakable Platforms:** Platforms that disappear after being landed on once.
* **Power-ups:** Collectible items that grant a temporary boost to the player's jump height.
* **Scorekeeping:** The player's score increases as they successfully jump onto new platforms.
* **Scrolling Camera:** The view dynamically scrolls upwards to keep the player in focus.
* **Game States:** Clear transitions between Menu, Playing, and Game Over screens.
* **High Score Tracking:** Records the highest score achieved.

## Getting Started

### Prerequisites

* **OpenGL:** Your system needs to have OpenGL drivers installed.
* **GLUT (OpenGL Utility Toolkit):** You need to have the GLUT library installed. The installation process varies depending on your operating system.

    * **Linux (Debian/Ubuntu):** `sudo apt-get update && sudo apt-get install freeglut3-dev`
    * **macOS (using Homebrew):** `brew install freeglut`
    * **Windows:** You might need to download the GLUT binaries and configure your compiler/linker to find them.

### Compilation

1.  Save the provided C++ code as `crazy_jump.cpp`.
2.  Open your terminal or command prompt.
3.  Navigate to the directory where you saved the file.
4.  Compile the code using a C++ compiler (like g++) and linking with the GLUT library.

    ```bash
    g++ crazy_jump.cpp -o crazy_jump -lglut -lGL -lGLU
    ```

    * On some systems, you might need to adjust the linking order or flags.

### Running the Game

After successful compilation, you can run the executable:

```bash
./crazy_jump

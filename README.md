# RayLife

A simple implementation of the [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using [raylib](https://www.raylib.com/).

## 🎮 Controls

- **Mouse wheel**: zoom in/out
- **Space Key**: tToggle between play and draw modes. (If the number of generations exceeds the maximum, step to the next generation.)
- **Left mouse button**: toggle cell
- **Right mouse button**: move camera
- **Up Arrow**: Increase generation interval
- **Down Arrow**: Decrease generation interval
- **R Key**: reset all

## 🖥️ Build

You must have [raylib](https://www.raylib.com/) installed.

1. Clone the repository

    ```bash
    git clone https://github.com/Pacatro/raylife.git
    cd raylife
    ```

2. Create a build directory

    ```bash
    mkdir build
    cd build
    ```

3. Run `cmake ..` and `make`

    ```bash
    cmake ..
    make
    ```

4. Run `./raylife`

    ```bash
    ./raylife <generations> # default: 1000
    ```

## 🎥 Example

Here’s a preview of the game:

![RayLife GIF](gif_raylife.gif)

## 🔑 License

[MIT](https://opensource.org/license/mit/) - Created by [**Paco Algar**](https://github.com/Pacatro).

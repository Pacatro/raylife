# RayLife

A simple implementation of the [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) using [raylib](https://www.raylib.com/).

## 🎮 Controls

- **Mouse wheel**: zoom in/out
- **Space Key**: toggle between play and draw modes (If the number of generations exceeds the maximum, step to the next generation)
- **Left mouse button**: toggle cell
- **Right mouse button**: move camera
- **Up Arrow**: Increase generation interval
- **Down Arrow**: Decrease generation interval
- **R Key**: reset all

## 🖥️ Build locally

You must have [raylib](https://www.raylib.com/) installed.

1. Clone the repository

    ```bash
    git clone https://github.com/Pacatro/raylife.git
    cd raylife/
    ```

2. Run `make`

    ```bash
    make
    ```

3. Run `./raylife`

    ```bash
    ./raylife <generations> # default: 500
    ```

## 🐳 Docker

You can use the [Dockerfile](./Dockerfile) to build a Docker image with the project.

1. Build the image

    ```bash
    docker buildx build --tag raylife .
    ```

2. Run the image

    ```bash
    xhost +local:docker
    docker run -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix raylife
    ```

## 🎥 Example

Here’s a preview of the game:

![RayLife GIF](./screenshots/gif_raylife.gif)

## 🔑 License

[MIT](https://opensource.org/license/mit/) - Created by [**Paco Algar**](https://github.com/Pacatro).

# Use the latest base image of Ubuntu
FROM ubuntu:latest

# Set the main working directory
WORKDIR /usr/src

# Install the necessary dependencies
RUN apt update && apt upgrade -y && \
    apt install -y build-essential git cmake libasound2-dev libx11-dev libxrandr-dev \
                   libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev \
                   libxinerama-dev libwayland-dev libxkbcommon-dev \
                   x11-apps && \
    apt autoremove -y && apt clean

# Install raylib
RUN git clone --depth 1 https://github.com/raysan5/raylib.git && \
    mkdir raylib/build && \
    cd raylib/build && \
    cmake -DBUILD_SHARED_LIBS=ON .. && \
    make && make install && \
    ldconfig && \
    cd /usr/src && rm -rf raylib

# Clone the raylife project
RUN git clone --depth 1 https://github.com/Pacatro/raylife.git

# Set the working directory for the project
WORKDIR /usr/src/raylife/

# Build the project using the makefile
RUN make

# Set the `LD_LIBRARY_PATH` to include raylib libraries
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Run the binary
CMD ["./raylife"]

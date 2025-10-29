# Use the latest base image of Ubuntu
FROM ubuntu:22.04

# Set the main working directory
WORKDIR /usr/src

# Install the necessary dependencies
RUN apt-get update && \
  apt-get upgrade -y && \
  apt-get install -y --no-install-recommends \
  build-essential \
  git \
  libasound2-dev \
  libx11-dev \
  libxrandr-dev \
  libxi-dev \
  libgl1-mesa-dev \
  libglu1-mesa-dev \
  libxcursor-dev \
  libxinerama-dev \
  libwayland-dev \
  libxkbcommon-dev \
  x11-apps \
  ca-certificates && \
  apt-get autoremove -y && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

# Install raylib
RUN git clone https://github.com/raysan5/raylib.git

WORKDIR /usr/src/raylib/src

RUN make PLATFORM=PLATFORM_DESKTOP && make install

WORKDIR /usr/src

# Remove raylib repo and clone the raylife project
RUN rm -rf raylib && \
  git clone --depth 1 https://github.com/Pacatro/raylife.git

# Set the working directory for the project
WORKDIR /usr/src/raylife/

# Build the project using the makefile
RUN make

# Set the `LD_LIBRARY_PATH` to include raylib libraries
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

RUN useradd -m -u 1000 raylife && \
  chown -R raylife:raylife /usr/src/raylife

USER raylife

# Run the binary
CMD ["./raylife"]
